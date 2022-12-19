#include <math.h>
#include <iostream>
#include <string>
#include <sstream> 
#include <vector>
#include <chrono>
#include <thread>

#include "..\util.hpp"
#include "iocards.h"
#include <cstdlib>
#include <cassert>
#include "../usb/libusb_interface.hpp"


extern logger LOG;

namespace zcockpit::cockpit::hardware
{

	std::string IOCards::mipIOCardDevice;
	std::string IOCards::fwdOvrheadIOCardDevice;
	std::string IOCards::rearOvrheadIOCardDevice;
	IOCards_bus_and_addr IOCards::iocards_device_list[IOCards::MAX_IOCARDS];

	/* missing values: UCHAR has no missing value. FLT and DBL are the same */
	#define INT_MISS -2000000000
	#define FLT_MISS -2000000000.0
	#define DBL_MISS -2000000000.0


	IOCards::IOCards(std::string deviceBusAddr, std::string name) : iocards_thread(), name(name)
	{
		handle = nullptr;
		isOpen = false;
		isInitialized = false;
		sentWakeupMsg = false;

		openDevice(deviceBusAddr);

	}

	void IOCards::openDevice(std::string deviceBusAddr)
	{
		libusb_device** devs;
		isOpen = false;
		isInitialized = false;

		struct libusb_context* local_ctx = nullptr;

		memset(time_enc, 0, sizeof(time_enc));


		if (deviceBusAddr.empty())
		{
			return;
		}

		auto idList = Util::split(deviceBusAddr, '_');
		if (idList.size() < 2)
		{
			return;
		}
		std::string str = idList[0];
		bus = (unsigned short)std::strtoul(str.c_str(), nullptr, 10);//  toUShort();
		str = idList[1];
		addr = (unsigned short)std::strtoul(str.c_str(), nullptr, 10);

		if (!LibUsbInterface::is_initialized())
		{
			if(!LibUsbInterface::initialize()) {
				return;
			}
		}

		this->ctx = local_ctx;

		int cnt = libusb_get_device_list(local_ctx, &devs);
		if (cnt < 0)
		{
			return;
		}

		int i = 0;
		while ((dev = devs[i++]) != nullptr)
		{
			unsigned short devBus = libusb_get_bus_number(dev);
			unsigned short devAddr = libusb_get_device_address(dev);
			if (devBus == bus && devAddr == addr)
			{
				struct libusb_device_descriptor desc;
				int ret = libusb_get_device_descriptor(dev, &desc);
				if (ret >= 0)
				{
					/* open device */
					ret = libusb_open(dev, &handle);
					if (ret < 0)
					{
						// there was an error
						LOG() << "IOCARDS:  libsub_open failed Bus " << devBus << " addr " << devAddr;
						handle = nullptr;
						break;
					}
					else
					{
						isOpen = true;
						LOG() << "IOCARDS:  libsub_open passed for Bus  " << devBus << " addr " << devAddr;
						break;
					}
				}
			}
		}
		if (!isOpen)
		{
			LOG() << "Failed to find device:  Bus " << bus << " addr " << addr;
		}
		//  Free list and unreference all the devices by setting unref_devices: to 1
		libusb_free_device_list(devs, 1);

	}

	IOCards::~IOCards()
	{
	}

	IOCards::IOCard_Device IOCards::identify_iocards_usb(unsigned short bus, unsigned short addr)
	{
		LOG() << "Attempting to identify IOCards device, Bus: " << bus << " address: " << addr;

		IOCard_Device found_device = UNKNOWN;
		std::string deviceBusAddr = std::to_string(bus) + "_" + std::to_string(addr);
		auto usb_device = std::make_unique<IOCards>(deviceBusAddr, "unknown");
		if (usb_device->isOpen)
		{
			usb_device->startThread();
			usb_device->worker->initDevice();

			// initialize 4 axis
			if (usb_device->initializeIOCards(4))
			{
				usb_device->initialize_iocardsdata();

				int attempts = 0;
				while (found_device == UNKNOWN && attempts < 3)
				{
					attempts++;
					int buffersize = 8;
					unsigned char send_data[] = { 0x3d,0x00,0x3a,0x01,0x39,0x04,0xff,0xff };
					//
					// Put message in send queue
					int send_status = usb_device->worker->write_usb(send_data, buffersize);

					// if USB is busy status  is zero
					// othrwise, status equal number of bytes sent
					if (send_status < 0)
					{
						if (send_status == -1)
						{
							LOG() << "Error IOCards: USB device is not ready or not connected.";
						}
						else if (send_status == -2)
						{
							LOG() << "Error IOCards: USB device has write buffer size mismatch";
						}
						else
						{
							LOG() << "Error IOCards:USB device has unknow error";
						}
					}
					int axis_read_status = 0;
					// Need to wait for reply
					// 200 ms before reinititializating
					for (int tries = 0; tries < 20; tries++)
					{
						// axis update every 10ms
						std::this_thread::sleep_for(std::chrono::milliseconds(10));

						if (usb_device->receive_mastercard() >= 0)
						{
							int axis_index = usb_device->axis - 1;
							if (axis_index >= 0 && axis_index <= 3)
							{
								axis_read_status |= 1 << axis_index;
								usb_device->axes_old[axis_index] = usb_device->axes[axis_index];

								// when all axes have been read -- look for a match
								if (axis_read_status >= 0X0F)
								{
									if (usb_device->axes_old[0] > 200 && usb_device->axes_old[1] < 50 && usb_device->axes_old[2] > 200 && usb_device->axes_old[3] < 50)
									{
										found_device = MIP;
										LOG() << "Found IOCards Device: MIP,  Number of Attempts: " << attempts << " Delta Time: " << (tries + 1) * 10 << " milliseconds";
										break;
									}
									else if (usb_device->axes_old[0] < 50 && usb_device->axes_old[1] > 200 && usb_device->axes_old[2] < 50 && usb_device->axes_old[3] > 200)
									{
										LOG() << "Found IOCards Device: Rear Overhead,  Number of Attempts: " << attempts << " Delta Time: " << (tries + 1) * 10 << " milliseconds";
										found_device = REAR_OVERHEAD;
										break;
									}
									else if (usb_device->axes_old[0] < 50 && usb_device->axes_old[1] < 50 && usb_device->axes_old[2] > 200 && usb_device->axes_old[3] > 200)
									{
										LOG() << "Found IOCards Device: Forward Overhead,  Number of Attempts: " << attempts << " Delta Time: " << (tries + 1) * 10 << " milliseconds";
										found_device = FWD_OVERHEAD;
										break;
									}

									// reset for next cycle
									axis_read_status = 0;
									for (int count = 0; count < TAXES; count++)
									{
										usb_device->axes_old[count] = 0;
									}
								}
							}
						}
						if (found_device != UNKNOWN)
						{
							LOG() << "IOCards Device is connected.  Closing USB connection.";
							break;
						}
					}
					if (found_device == UNKNOWN)
					{
						LOG() << "Error locating IOCards:  USB re-initializing.  Number of Attempts: " << attempts;
					}
				}
			}
			usb_device->closeDown();
		}

		return found_device;
	}

	std::string IOCards::find_iocard_devices()
	{
		libusb_device** devs;
		std::string devices;

		if(LibUsbInterface::is_initialized()){
			const int cnt = libusb_get_device_list(nullptr, &devs);
			if(cnt < 0)
			{
				LOG() << "Libusb get device list Error: " << cnt;;
				return "Libusb get device list Error: " + std::to_string(cnt);
			}

			libusb_device* dev;
			int i = 0;
			int iocards_cnt = 0;
			memset(iocards_device_list, -1, sizeof(IOCards_bus_and_addr) * MAX_IOCARDS);
			while((dev = devs[i++]) != nullptr)
			{
				libusb_device_descriptor desc{};
				const int ret = libusb_get_device_descriptor(dev, &desc);
				if(ret < 0)
				{
					LOG() << "Libusb get device descriptor Error: " << cnt;;
					return "Libusb get device descriptor Error: " + std::to_string(cnt);
				}

				if(desc.idVendor == 0 && desc.idProduct == 0)
				{
					const uint8_t bus_number = libusb_get_bus_number(dev);
					std::string bus = std::to_string(bus_number);
					const uint8_t device_addr = libusb_get_device_address(dev);
					std::string address = std::to_string(device_addr);
					devices += bus + "_" + address + " - ";
					if(iocards_cnt < 10)
					{
						iocards_device_list[iocards_cnt].bus = bus_number;
						iocards_device_list[iocards_cnt].address = device_addr;
						iocards_cnt++;
					}
				}
			}
			if(devices.empty())
			{
				devices = "NO IOCARDS DEVICES";
			}

			//  Free list and unreference all the devices by setting unref_devices: to 1
			libusb_free_device_list(devs, 1);

			return devices;
		}
	}


	void IOCards::mainThread()
	{
		worker->process();
		LOG() << "Worker thread ended for " << worker->name;
	}

	void IOCards::startThread(void)
	{
		worker = std::make_unique<UsbWorker>(dev, handle, ctx, name);
		// start the thread
		iocards_thread = std::thread(&IOCards::mainThread, this);
	}


	bool IOCards::initializeIOCards(unsigned char number_of_axes)
	{
		isInitialized = false;
		if (worker)
		{
			int buffersize = 8;
			unsigned char send_data[] = { 0x3d,0x00,0x3a,0x01,0x39,0x00,0xff,0xff };

			// card 1
			send_data[3] = 0x01;
			// set number of analog axes ZERO of USB expansion card
			send_data[5] = number_of_axes;

			int send_status = worker->write_usb(send_data, buffersize);
			if (send_status == buffersize)
			{
				isInitialized = true;
				worker->runStop();
			}
		}
		return isInitialized;
	}


	/* saves a copy of all IOCARDS I/O states */
	/* this is needed because, at each step, only the modified values */
	/* are communicated either via TCP/IP or USB to X-Plane and IOCARDS */
	int IOCards::copyIOCardsData(void)
	{
		//memcpy(inputs_old, inputs, sizeof(inputs));
		//memcpy(outputs_old, outputs, sizeof(outputs));
		//memcpy(displays_old, displays, sizeof(displays));

		return (0);
	}


	void IOCards::closeDown()
	{
		if (isOpen)
		{
			if (worker)
			{
				worker->abort();
				// give the worker some time to quit
				std::chrono::milliseconds dura(100);
				std::this_thread::sleep_for(dura);

				if (handle != nullptr)
				{
					int is_running = worker->is_running();
					if (is_running != 0)
					{
						do
						{
							if (is_running == 1)
							{
								// still running send request to wake up libusb_handle_events()
								// THE PROBLEM: the worker is conditional wait which could last forever --
								// we need the iocard to send data to the usb to wake it up and worker task continue
								// it will then see the abort and return -- ending the task
								LOG() << "IOCARDS Closing Down worker is still running: " << worker->name;
								int buffersize = 8;
								unsigned char send_data[] = { 0x3d,0x00,0x3a,0x01,0x39,0x00,0xff,0xff };
								if (!sentWakeupMsg)
								{
									sentWakeupMsg = true;
									worker->write_usb(send_data, buffersize);
								}
								std::this_thread::sleep_for(std::chrono::milliseconds(200));
								is_running = worker->is_running();
								LOG() << "IOCARDS Closing Down for: " << worker->name << " is_running = " << is_running;
							}
							else if (is_running == -1)
							{ // cannot get the lock
								std::this_thread::sleep_for(std::chrono::milliseconds(200));
								is_running = worker->is_running();
								LOG() << "IOCARDS Closing Down can't get mutex (trying again ) for: " << worker->name << " is_running = " << is_running;
							}
						} while (is_running != 0);
					}
					else
					{
						LOG() << "IOCARDS ClosingDown -- Deleting thread for: " << worker->name;
					}

					if (iocards_thread.joinable())
					{
						iocards_thread.join();
					}
					LOG() << "IOCARDS ClosingDown -- Deleting worker for: " << worker->name;
				}
			}
		}
	}


	int IOCards::receive_mastercard(void)
	{
		int result = 0;
		int recv_status;
		int input[8][8]; /* 8 byte bit storage */

		int index; /* mastercard input number */
		int card; /* mastercard counter: maximum 4 MASTERCARDS per USB expansion card */
		int slot; /* slot counter (each mastercard has 8 slots with 9 inputs) */
		int axisval; /* value of active A/D converter */

		int found; /* found the next slot to be read */
		int readleft; /* yet to be read slot data for first 8 bits */
		int readnine; /* read slot data for last 9th bit in this byte */
		int sumnine; /* cumulated read slot data for last 9th bit */
		int sumslots; /* maximum number of 9bit slots present in this transmission */
		int sumcards; /* maximum number of cards present in this transmission */

		const int buffersize = 8;
		unsigned char recv_data[buffersize]; /* mastercard raw IO data */

		/* PROTOCOL */
		/* Each Mastercard has 72 Inputs, distributed over 8 slots with 9 inputs
		The first 8 inputs of each slot are read first, followed by an optional vector of the 9th input for each slot
		The Mastercard No. is given in the first byte (input 0-3)
		If any slots have inputs set to 1, these slots are set to 1 in byte 1
		The data of the present slots will then follow in the next bytes (bytes 2-7)
		If there are more slots than fit into a single message, a followup message must be read; such a message is
		identified by bit 7 of byte 0 being set.
		Note: if any A/D converters are turned on, by a "naxes" item in the usbiocards.ini file, this is reflected
		in the bits 4-6 of the first byte (1x, 2x, 3x or 4x giving the number of the currently reporting converter).
		In this case, the byte 1 is the output of the A/D converter, and the slot bits are moved to byte 2.
		Data of the present slots will then follow in bytes 3-7.
		Note also that the A/D converters will report continually, every 10msec or so, not only on value change!
		*/


		/* check if we have a connected USB expander card */
		if (isOpen && isInitialized && worker && worker->is_running() == 1)
		{
			/* check whether there is new data on the read buffer */
			recv_status = worker->read_usb(recv_data, buffersize);
			int byteCnt; // byte count 

			if (recv_status > 0)
			{
				int bitCnt; // bit count within byte 
				/* fill the input array by bitshifting the first eight bytes */
				for (byteCnt = 0; byteCnt < 8; byteCnt++)
				{
					int x = recv_data[byteCnt];
					for (bitCnt = 0; bitCnt < 8; bitCnt++)
					{
						if (x & 01)
						{
							input[byteCnt][bitCnt] = 1;
						}
						else
						{
							input[byteCnt][bitCnt] = 0;
						}
						x = x >> 1;
					}
					//LOG() << "LIBIOCARDS: Received " << input[byteCnt][7] << input[byteCnt][6] << input[byteCnt][5] << input[byteCnt][4]
					//	<< input[byteCnt][3] << input[byteCnt][2] << input[byteCnt][1] << input[byteCnt][0];
				}

				/* examine first byte: bits 0-3: Mastercard, 4-6: A/D, 7: continuation */

				/* determine whether an A/D converter is active and read its value */
				axis = (recv_data[0] >> 4) & 7; /* extract the A/D number from bits 6-4 */

				if (axis > 0)
				{
					axisval = recv_data[1];
					axes[axis - 1] = axisval;
				}

				if (axis == 0)
				{
					/* no analog axis value: start slot indicator at second byte */
					byteCnt = 1;
				}
				else
				{
					/* axis value present in second byte: start slot indicator at third byte */
					byteCnt = 2;
				}


				if (input[0][7] == 0)
				{
					/* OPTION #1 */
					/* new transmission (first 8 byte packet) */

					/* clean slot index data */
					for (card = 0; card < MASTERCARDS; card++)
					{
						for (slot = 0; slot < 8; slot++)
						{
							slotdata[slot][card] = 0;
						}
					}

					/* fill slot index data with slot indices of first transmission */
					sumslots = 0;
					sumcards = 0;
					for (card = 0; card < MASTERCARDS; card++)
					{
						if (input[0][card] == 1)
						{
							for (slot = 0; slot < 8; slot++)
							{
								if (input[byteCnt + sumcards][slot] == 1)
								{
									slotdata[slot][card] = 1;
									sumslots++;
								}
							}
							sumcards++;
						}
					}

					//LOG() << "LIBIOCARDS: new transmission with " << sumcards << " cards and " << sumslots << " slots present.";

					card = 0; //for (card = 0; card < MASTERCARDS; card++)
					{
						//LOG() << "card " << card << " slots: " <<
						//    slotdata[7][card] << " " <<
						//    slotdata[6][card] << " " <<
						//    slotdata[5][card] << " " <<
						//    slotdata[4][card] << " " <<
						//    slotdata[3][card] << " " <<
						//    slotdata[2][card] << " " <<
						//    slotdata[1][card] << " " <<
						//    slotdata[0][card];
					}


					/* augment byte count to position of first data packet */
					byteCnt += sumcards;
				}
				else
				{
					/* OPTION #2 */
					/* continuation of previous transmission (another 8 byte packet) */
					LOG() << "option 2";
				}

				while ((byteCnt < 8) && (byteCnt >= 0))
				{
					/* read slotwise input data for first 8 bits of each slot */
					found = 0;
					readleft = 0;
					for (card = 0; card < MASTERCARDS; card++)
					{
						for (slot = 0; slot < 8; slot++)
						{
							if (slotdata[slot][card] == 1)
							{
								readleft++;
								if (!found)
								{
									found = 1;
									slotdata[slot][card] = 2;
									if (card < NCARDS)
									{
										for (int bit = 0; bit < 8; bit++)
										{
											index = 9 * slot + bit;
											if (inputs[index][card] != input[byteCnt][bit])
											{
											//	LOG() << "index=" << index << " slot=" << slot << " bit=" << bit << " bytecnt=" << byteCnt << " old=" 
											//		<< inputs[index][card] << " new="  <<input[byteCnt][bit];
											}
											inputs[index][card] = input[byteCnt][bit];
										}
									}
									else
									{
										// "LIBIOCARDS: card x sent input but is not defined
									}
								}
							}
						}
					} // for cards (0-3)

					/* read slotwise input data for last 9th bit of each slot */
					if (readleft == 0)
					{
						readnine = 0;
						sumnine = 0;
						for (card = 0; card < MASTERCARDS; card++)
						{
							for (slot = 0; slot < 8; slot++)
							{
								if (slotdata[slot][card] == 3)
								{
									sumnine++;
								}
								if (slotdata[slot][card] == 2)
								{
									if (readnine < 8)
									{
										slotdata[slot][card] = 3;
										if (card < NCARDS)
										{
											int bit = (sumnine + readnine) % 8; /* present slots fill up subsequent bytes with their 9th bit data */
											index = 9 * slot + 8;
											inputs[index][card] = input[byteCnt][bit];
										}
									}
									readnine++;
								}
							}
						}
					}
					/* next byte */
					byteCnt++;
				} // end while
				result = recv_status;
			}
		} // if open & init

		return result;
	}

	/* send changes in the outputs array (outputs and displays) to MASTERCARD */
	/* MASTERCARD is connected to USB EXPANSION CARD */
	int IOCards::send_mastercard(void)
	{
		int result = 0;
		int buffersize = 8;
		unsigned char send_data[] = { 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff };
		int send_status = 0;
		int count;
		int firstoutput = 11; /* output channels start at 11, and go to 55 */
		int totchannels = 64; /* total channels per card (second card starts at 64(+11), 3rd at 128(+11), 4th at 192+11))*/
		int channelspersegment = 8;
		int totalSegments = totchannels / channelspersegment;
		int card;
		int channel;
		int changed;
		int segment; /* we have to write full segments of 8 outputs each */

		int power;

		/* check if we have a connected and initialized mastercard */
		if (isOpen && isInitialized && worker && worker->is_running() == 1)
		{
			/* TODO: check if outputs and displays work with multiple cards */

			/* fill send data with output information */
			for (card = 0; card < MASTERCARDS; card++)
			{
				for (segment = 0; segment < (totalSegments); segment++)
				{
					changed = 0;

					if ((segment * channelspersegment) < NUM_OUTPUTS)
					{
						/* 8-byte segment transfers */
						for (count = 0; count < channelspersegment; count++)
						{
							channel = segment * channelspersegment + count;
							power = 1 << count;

							if (channel < NUM_OUTPUTS)
							{
								if (outputs[channel][card] != outputs_old[channel][card])
								{
									changed = 1;
								}
								if (count == 0)
								{
									send_data[1] = outputs[channel][card];
								}
								else
								{
									send_data[1] += outputs[channel][card] * power;
								}
							}
							else
							{
								if (count == 0)
								{
									send_data[1] = 1;
								}
								else
								{
									send_data[1] += power;
								}
							}
						}

						if (changed == 1)
						{
							send_data[0] = card * totchannels + segment * channelspersegment + firstoutput;

							send_status = worker->write_usb(send_data, buffersize);
							if ((send_status) < 0)
							{
								result = send_status;
								break;
							}
							memset(&send_data, 0xff, buffersize);

							for (count = 0; count < channelspersegment; count++)
							{
								channel = count + segment * channelspersegment;
								if (channel < NUM_OUTPUTS)
								{
									printf("LIBIOCARDS: send output to MASTERCARD card=%i output=%i value=%i \n",
										card, firstoutput + channel, outputs[channel][card]);
								}
							}
						}
					}
				}


				/* fill send data with display information */
				//memset(&send_data, 0xff, buffersize);
				//for (count=0; count<NUM_DISPLAYS; count++) {
				//    if (displays[count][card] != displays_old[count][card]) {

				//        // value first then position
				//        send_data[0] = displays[count][card];
				//        send_data[1] = card*MASTERCARDS + count;


				//        send_status = worker->write_usb(send_data, buffersize);
				//        if ((send_status) < 0) {
				//            result = send_status;
				//            break;
				//        }


				//        printf("LIBIOCARDS: send display to MASTERCARD card=%i display=%i value=%i \n",
				//            card, count, displays[count][card]);
				//    }
				//}
			}
		}

		return result;
	}

	int IOCards::mastercard_send_display(unsigned char value, int pos, int card)
	{
		int ret = 0;
		if (worker && worker->is_running() == 1)
		{
			unsigned char send_data[] = { 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff };
			displays[pos][card] = value;
			if (displays[pos][card] != displays_old[pos][card])
			{
				// value first then position
				send_data[0] = displays[pos][card];
				send_data[1] = card * MASTERCARDS + pos;
				ret = worker->write_usb(send_data, 8);
			}
		}
		return ret;
	}

	/* retrieve input value from given input position on MASTERCARD */
	/* Two types : */
	/* 0: pushbutton */
	/* 1: toggle switch */
	int IOCards::mastercard_input(int input, int* value, int card)
	{
		int retval = 0; /* returns 1 if something changed, and 0 if nothing changed, and -1 if something went wrong */

		if (value != nullptr)
		{
			/* check if we have a connected and initialized mastercard */
			if (isOpen && isInitialized)
			{
				if ((card >= 0) && (card < MASTERCARDS))
				{
					if ((input >= 0) && (input < NUM_INPUTS))
					{
						/* simple pushbutton */
						if (inputs_old[input][card] != inputs[input][card])
						{
							/* something changed */
							*value = inputs[input][card];
							retval = 1;
							//
							// save value
							inputs_old[input][card] = inputs[input][card];

							// LOG() << "LIBIOCARDS: Pushbutton     : card=" << card << " input=" << input << " value=%" << inputs[input][card];
						}
						else
						{
							/* nothing changed */
							*value = inputs[input][card];
							retval = 0;
						}
					}
					else
					{
						retval = -1;
						LOG() << "LIBIOCARDS: Invalid MASTERCARD input position detected: " << input;
					}
				}
				else
				{
					retval = -1;
					LOG() << "LIBIOCARDS: Invalid MASTERCARD number detected: " << card;
				}
			}
			else
			{
				retval = -1;
			}
		}

		return (retval);
	}

	void IOCards::process_master_card_inputs(const OnOffKeyCommand keycmd[], int numberOfCmds, int card)
	{
		int* value;
		int retval;
		if (isOpen && isInitialized)
		{
			if ((card >= 0) && (card < MASTERCARDS))
			{
				//for (auto input = 0; input < numberOfCmds; input++)
				//{
				//	if (keycmd[input].on != INVALID)
				//	{
				//		if (inputs_old[input][card] != inputs[input][card])
				//		{
				//			if (inputs[input][card] != 0)
				//			{
				//				sendMessageInt(keycmd[input].on, 0);
				//				LOG() << "send to buffer " << input << " new " << inputs[input][card] << " old " << inputs_old[input][card];
				//			}
				//			else if (keycmd[input].off != INVALID)
				//			{
				//				sendMessageInt(keycmd[input].off, 0);
				//			}
				//			inputs_old[input][card] = inputs[input][card];
				//		}
				//	}
				//}
			}
		}
	}

	#pragma optimize( "", off )  
	void IOCards::process_master_card_inputs(masterCard_input_state* switch_states[], int numberOfCmds, int card)
	{
		int* value;
		int retval;
		if (isOpen && isInitialized)
		{
			if ((card >= 0) && (card < MASTERCARDS))
			{
				for (auto i = 0; i < numberOfCmds; i++)
				{
					int input = switch_states[i]->input;
					if (inputs_old[input][card] != inputs[input][card])
					{
						switch_states[i]->has_changed = true;
						switch_states[i]->value = inputs[input][card];
					}
					inputs_old[input][card] = inputs[input][card];
				}
			}
		}
	}
	#pragma optimize( "", on )  

	/* fill output value for given output position on MASTERCARD */
	int IOCards::mastercard_output(int output, int* value, int card)
	{
		int retval = 0;
		int firstoutput = 11; /* output channels start at 11, and go to 55 */

		if (value != nullptr)
		{
			/* check if we have a connected and initialized mastercard */
			if (isOpen && isInitialized)
			{
				if (*value != INT_MISS)
				{
					if ((card >= 0) && (card < MASTERCARDS))
					{
						if ((output >= firstoutput) && (output <= (firstoutput + NUM_OUTPUTS)))
						{
							if (*value == 1)
							{
								outputs[output - firstoutput][card] = 1;
							}
							else if (*value == 0)
							{
								outputs[output - firstoutput][card] = 0;
							}
							else
							{
								retval = -1;
							}
						}
						else
						{
							retval = -1;
							printf("LIBIOCARDS: Invalid MASTERCARD output position detected: %i \n", output);
						}
					}
					else
					{
						retval = -1;
						printf("LIBIOCARDS: Invalid MASTERCARD number detected: %i \n", card);
					}
				}
			}
			else
			{
				retval = -1;
				printf("LIBIOCARDS: Device either not a MASTERCARD or not ready \n");
			}
		}

		return (retval);
	}

	static double iocardRound(double number)
	{
		return number < 0.0 ? ceil(number - 0.5) : floor(number + 0.5);
	}

	/* fill display at given output position on MASTERCARD */
	int IOCards::mastercard_display(int pos, int n0, int* value, int hasnegative, int card)
	{
		int retval = 0;
		int single;
		int count;
		int power;
		int tempval;
		int negative = 0;
		int n;

		/* codes for display II card
		10 = Put to "OFF" the digit  --> 10
		11 = Put the "-" sign --> 0xf8
		12 = Put a Special 6
		13 = Put a "t"
		14 = Put a "d"
		15 = Put a "_" (Underscore)
		*/

		if (value != nullptr)
		{
			/* check if we have a connected and initialized mastercard */
			if (isOpen && isInitialized)
			{
				if ((card >= 0) && (card < MASTERCARDS))
				{
					if ((pos >= 0) && ((pos + n0) < NUM_DISPLAYS))
					{
						if (*value != INT_MISS)
						{
							/* generate temporary storage of input value */
							tempval = *value;

							n = n0;

							/* if negative values are allowed, reserve the first display for the minus sign */
							if (hasnegative > 0)
							{
								n = n0 - 1;
							}

							/* reverse negative numbers: find treatment for - sign */
							if (tempval < 0)
							{
								tempval = -tempval;
								negative = 1;
							}

							/*cut high values */
							power = static_cast<int>(iocardRound(pow(10, n)));
							if (tempval >= power)
							{
								tempval = tempval - (tempval / power) * power;
							}

							/* read individual digits from integer */
							for (count = n - 1; count >= 0; count--)
							{
								power = static_cast<int>(iocardRound(pow(10, count)));
								single = tempval / power;
								tempval = tempval - single * power;
								displays[count + pos][card] = single;
							}

							if (hasnegative > 0)
							{
								if (negative == 1)
								{
									/* put a - sign in the frontmost display */
									displays[pos + n][card] = 0xf8;
								}
								else
								{
									displays[pos + n][card] = 0x0a;
								}
							}
						}
					}
					else
					{
						retval = -1;
						printf("LIBIOCARDS: Invalid MASTERCARD display range detected: %i - %i \n", pos, pos + n0);
					}
				}
				else
				{
					retval = -1;
					printf("LIBIOCARDS: Invalid MASTERCARD number detected: %i \n", card);
				}
			}
			else
			{
				retval = -1;
				printf("LIBIOCARDS: Device is not ready:\n");
			}
		}

		return (retval);
	}

	/* retrieve encoder value and for given encoder type from given input position on MASTERCARD */
	/* three type of encoders: */
	/* 0: 1x12 rotary switch, wired like demonstrated on OpenCockpits website */
	/* 1: optical rotary encoder using the Encoder II card */
	/* 2: 2 bit gray type mechanical encoder */
	/* 3: optical rotary encoder without the Encoder II card */
	int IOCards::mastercard_encoder(int input, double* value, double multiplier, double accelerator, int type/*=2*/, int card/*=0*/)
	{
		int oldcount, newcount; /* encoder integer counters */
		int updown = 0; /* encoder direction */
		int retval = 0; /* returns 1 if something changed, 0 if nothing changed and -1 if something went wrong */
		int obits[2]; /* bit arrays for 2 bit gray encoder */
		int nbits[2]; /* bit arrays for 2 bit gray encoder */

		int acceleration;

		if (value != nullptr)
		{
			/* check if we have a connected and initialized mastercard */
			if (isOpen && isInitialized)
			{
				if ((card >= 0) && (card < MASTERCARDS))
				{
					if (*value != FLT_MISS)
					{
						if (((input >= 0) && (input < (NUM_INPUTS - 2)) && (type == 0)) ||
							((input >= 0) && (input < (NUM_INPUTS - 1)) && (type > 0)))
						{
							if (type == 0)
							{
								assert(false);
								//
								// Not supported -- inputs_old needs to be saved while processing

								/* simulated encoder out of a 1x12 rotary switch */

								if ((inputs[input][card] +
									inputs[input + 1][card] +
									inputs[input + 2][card]) == 0)
									/* 0 0 0 is a wrong measurement due to switch mechanics: do not count */
								{
									inputs[input][card] = inputs_old[input][card];
									inputs[input + 1][card] = inputs_old[input + 1][card];
									inputs[input + 2][card] = inputs_old[input + 2][card];
								}
								else
								{
									if (((inputs[input][card] != inputs_old[input][card]) ||
										(inputs[input + 1][card] != inputs_old[input + 1][card]) ||
										(inputs[input + 2][card] != inputs_old[input + 2][card])) &&
										(inputs_old[input][card] != -1) &&
										(inputs_old[input + 1][card] != -1) &&
										(inputs_old[input + 2][card] != -1))
									{
										/* something has changed */


										LOG() << "LIBIOCARDS: Rotary Encoder    1x12 Type :  card= " << card << " inputs= " << input << "-" << input + 2 << " values= " <<
											inputs[input][card] << " " << inputs[input + 1][card] << " " << inputs[input + 2][card];

										newcount = inputs[input][card] +
											inputs[input + 1][card] * 2 + inputs[input + 2][card] * 3;
										oldcount = inputs_old[input][card] +
											inputs_old[input + 1][card] * 2 + inputs_old[input + 2][card] * 3;

										if (newcount > oldcount)
										{
											updown = 1;
										}
										else
										{
											updown = -1;
										}

										if ((oldcount == 3) && (newcount == 1))
										{
											updown = 1;
										}
										if ((oldcount == 1) && (newcount == 3))
										{
											updown = -1;
										}

										if (updown != 0)
										{
											*value = *value + (float)(updown * get_acceleration(card, input, accelerator)) * multiplier;
											retval = 1;
										}
									}
								}
							}

							if (type == 1)
							{
								assert(false);
								//
								// Not supported -- inputs_old needs to be saved while processing


								/* optical rotary encoder using the Encoder II card */

								if (((inputs[input][card] != inputs_old[input][card]) ||
									(inputs[input + 1][card] != inputs_old[input + 1][card]))
									&& (inputs_old[input][card] != -1) && (inputs_old[input + 1][card] != -1))
								{
									/* something has changed */


									printf("LIBIOCARDS: Rotary Encoder Optical Type : card=%i inputs=%i-%i values=%i %i \n",
										card, input, input + 1, inputs[input][card], inputs[input + 1][card]);


									if (inputs[input + 1][card] == 1)
									{
										updown = 1;
									}
									else
									{
										updown = -1;
									}

									if (updown != 0)
									{
										LOG() << "value = " << *value << "upDown " << updown;
										*value = *value + (float)(updown * get_acceleration(card, input, accelerator)) * multiplier;
										retval = 1;
									}
								}
							}

							if (type == 2)
							{
								/* 2 bit gray type encoder */

								//printf("%i %i %i %i\n",inputs_old[input][card],inputs_old[input+1][card],inputs[input][card],inputs[input+1][card]);
								if (((inputs[input][card] != inputs_old[input][card]) ||
									(inputs[input + 1][card] != inputs_old[input + 1][card]))
									&& (inputs_old[input][card] != -1) && (inputs_old[input + 1][card] != -1))
								{
									/* something has changed */


									//               printf("LIBIOCARDS: Rotary Encoder    Gray Type : card=%i inputs=%i-%i values=%i %i \n",
									//                   card,input,input+1,inputs[input][card],inputs[input+1][card]);


									/* derive last encoder count */
									obits[0] = inputs_old[input][card];
									obits[1] = inputs_old[input + 1][card];
									oldcount = obits[0] + 2 * obits[1];

									/* derive new encoder count */
									nbits[0] = inputs[input][card];
									nbits[1] = inputs[input + 1][card];
									newcount = nbits[0] + 2 * nbits[1];

									/* forward */
									if (((oldcount == 0) && (newcount == 1)) ||
										((oldcount == 1) && (newcount == 3)) ||
										((oldcount == 3) && (newcount == 2)) ||
										((oldcount == 2) && (newcount == 0)))
									{
										updown = 1;
									}

									/* backward */
									if (((oldcount == 2) && (newcount == 3)) ||
										((oldcount == 3) && (newcount == 1)) ||
										((oldcount == 1) && (newcount == 0)) ||
										((oldcount == 0) && (newcount == 2)))
									{
										updown = -1;
									}

									if (updown != 0)
									{
										acceleration = get_acceleration(card, input, accelerator);
										//	LOG() << "value = " << *value << " upDown " << updown << " accel " << acceleration;
										*value = *value + (updown * acceleration * multiplier);
										retval = 1;
									}

									//inputs_old[input][card] = inputs[input][card];
									//inputs_old[input + 1][card] = inputs[input + 1][card];
								}
								inputs_old[input][card] = inputs[input][card];
								inputs_old[input + 1][card] = inputs[input + 1][card];
							}

							if (type == 3)
							{
								assert(false);
								//
								// Not supported -- inputs_old needs to be saved while processing


								/* 2 bit optical encoder: phase e.g. EC11 from ALPS */

								if (((inputs[input][card] != inputs_old[input][card]) ||
									(inputs[input + 1][card] != inputs_old[input + 1][card]))
									&& (inputs_old[input][card] != -1) && (inputs_old[input + 1][card] != -1))
								{
									/* something has changed */


									printf("LIBIOCARDS: Rotary Encoder  Phased Type : card=%i inputs=%i-%i values=%i %i \n",
										card, input, input + 1, inputs[input][card], inputs[input + 1][card]);

									/* derive last encoder count */
									obits[0] = inputs_old[input][card];
									obits[1] = inputs_old[input + 1][card];
									/* derive new encoder count */
									nbits[0] = inputs[input][card];
									nbits[1] = inputs[input + 1][card];

									if ((obits[0] == 0) && (obits[1] == 1) && (nbits[0] == 0) && (nbits[1] == 0))
									{
										updown = -1;
									}
									else if ((obits[0] == 0) && (obits[1] == 1) && (nbits[0] == 1) && (nbits[1] == 1))
									{
										updown = 1;
									}
									else if ((obits[0] == 1) && (obits[1] == 0) && (nbits[0] == 1) && (nbits[1] == 1))
									{
										updown = -1;
									}
									else if ((obits[0] == 1) && (obits[1] == 0) && (nbits[0] == 0) && (nbits[1] == 0))
									{
										updown = 1;
									}

									if (updown != 0)
									{
										*value = *value + (float)(updown * get_acceleration(card, input, accelerator)) * multiplier;
										retval = 1;
									}
								}
							}
						}
						else
						{
							retval = -1;
							if (type == 0)
							{
								printf("LIBIOCARDS: Invalid MASTERCARD input position detected: %i - %i \n", input, input + 2);
							}
							else
							{
								printf("LIBIOCARDS: Invalid MASTERCARD input position detected: %i - %i \n", input, input + 1);
							}
						}
					} /* input encoder value not missing */
				}
				else
				{
					retval = -1;
					LOG() << "LIBIOCARDS: Invalid MASTERCARD number detected: " << card;
				}

				if ((type < 0) || (type > 3))
				{
					retval = -1;
					LOG() << "LIBIOCARDS: Invalid encoder type detected: " << type;
				}
			}
			else
			{
				retval = -1;
			}
		}

		return (retval);
	}

	/* initialize data arrays with default values */
	/* flight data for  USB and TCP/IP communication */
	int IOCards::initialize_iocardsdata(void)
	{
		int count;
		int card;
		auto time_new = std::chrono::high_resolution_clock::now();

		LOG() << "LIBIOCARDS: Initialization started \n";

		/* get new time */
		//    update.start();

		/* reset all inputs, axes, outputs and displays */
		for (card = 0; card < MASTERCARDS; card++)
		{
			for (count = 0; count < NUM_INPUTS; count++)
			{
				inputs[count][card] = -1;
				inputs_old[count][card] = -1;
				time_enc[count][card] = time_new;
			}
			for (count = 0; count < TAXES; count++)
			{
				axes[count] = 0;
				axes_old[count] = 0;
			}
			for (count = 0; count < NUM_OUTPUTS; count++)
			{
				outputs[count][card] = 0;
				outputs_old[count][card] = -1;
			}
			for (count = 0; count < NUM_DISPLAYS; count++)
			{
				displays[count][card] = 0;
				displays_old[count][card] = -1;
			}
		}


		return 0;
	}

	/* this routine calculates the acceleration of rotary encoders based on last rotation time */
	int IOCards::get_acceleration(int card, int input, double accelerator)
	{
		//	long long t2 = 0;
		//	long long t1 = 0;
		double dt;
		int acceleration;

		double mintime = 0.001;
		double maxtime = 0.8;

		/* get new time */
		auto t1 = std::chrono::high_resolution_clock::now();
		auto t2 = time_enc[input][card];

		auto dt_ms = std::chrono::duration<double, std::milli>(t2 - t1);
		dt = -dt_ms.count() / 1000.0;

		if (dt < mintime)
		{
			dt = mintime;
		}

		acceleration = 1 + (int)((maxtime / dt) * accelerator);

		//LOG() << "LIBIOCARDS: difference= " << dt << " [seconds], acceleration=" << acceleration << " for input= " << input << " card= " << card;


		time_enc[input][card] = t1;

		return acceleration;
	}
}