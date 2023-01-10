#include <math.h>
#include <iostream>
#include <string>
#include <sstream> 
#include <vector>
#include <chrono>
#include <thread>

#include "..\util.hpp"
#include "iocards.hpp"
#include <cstdlib>
#include <cassert>
#include <map>

#include "logger.hpp"
#include "../usb/libusb_interface.hpp"


extern logger LOG;

namespace zcockpit::cockpit::hardware
{
	// STATIC
	std::string IOCards::devices;
	std::string IOCards::mip_bus_addr;
	std::string IOCards::fwd_overhead_bus_addr;
	std::string IOCards::rear_overhead_bus_addr;


	/* missing values: UCHAR has no missing value. FLT and DBL are the same */
	#define INT_MISS -2000000000
	#define FLT_MISS -2000000000.0
	#define DBL_MISS -2000000000.0


	IOCards::IOCards(const std::string deviceBusAddr, const std::string name) : device_name(name) //, iocards_thread() 
	{
		is_okay = open_device(deviceBusAddr);
		if(handle != nullptr){
			auto ret = libusb_claim_interface(handle, 0);
			if(ret < 0) {
				LOG() << "Cannot claim libusb device: error " << ret;
				is_okay = false;
			}
			else {
				is_Claimed = true;
			}
		}
	}

	IOCards::~IOCards()
	{
		if (!has_dropped) {
			drop();
		}

	}

	void IOCards::drop() {
		close_down();
		if (LibUsbInterface::is_initialized()) {
			if (readTransfer) {
				libusb_free_transfer(readTransfer);
				readTransfer = nullptr;
			}
			if (writeTransfer) {
				libusb_free_transfer(writeTransfer);
				writeTransfer = nullptr;
			}


			if(is_Claimed && handle != nullptr) {
				auto ret = libusb_release_interface(handle, 0);
				if(ret < 0) {
					LOG() << "Cannot release libusb device: error " << ret;
				}
			}
			if(is_open && handle != nullptr) {
				libusb_close(handle);
				is_open= false;
			}
		}
		has_dropped = true;
	}

	bool IOCards::open_device(const std::string device_bus_addr)
	{
		libusb_device** devs;
		is_open = false;

		memset(time_enc, 0, sizeof(time_enc));


		if (device_bus_addr.empty())
		{
			return false;
		}

		const auto idList = Util::split(device_bus_addr, '_');
		if (idList.size() < 2)
		{
			return false;
		}
		std::string str = idList[0];
		bus = static_cast<unsigned short>(std::strtoul(str.c_str(), nullptr, 10));//  toUShort();
		str = idList[1];
		addr = static_cast<unsigned short>(std::strtoul(str.c_str(), nullptr, 10));

		if (!LibUsbInterface::is_initialized())
		{
			if(!LibUsbInterface::initialize()) {
				return false;
			}
		}

		libusb_device* dev =nullptr;
		const int cnt = static_cast<int>(libusb_get_device_list(LibUsbInterface::ctx, &devs));
		if (cnt < 0)
		{
			return false;
		}

		int i = 0;
		while ((dev = devs[i++]) != nullptr)
		{
			const unsigned short devBus = libusb_get_bus_number(dev);
			const unsigned short devAddr = libusb_get_device_address(dev);
			if (devBus == bus && devAddr == addr)
			{
				libusb_device_descriptor desc{};
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
						is_open = true;
						LOG() << "IOCARDS:  libsub_open passed for Bus  " << devBus << " addr " << devAddr;

						struct libusb_config_descriptor* config;

						// retrieve device information 
						ret = libusb_get_config_descriptor(dev, 0, &config);
						if(ret < 0)
						{
							LOG() << "LIBUSB_INTERFACE: Failed to get config descriptor  error code " << ret;
						}
						else
						{
							const struct libusb_interface* iface = &config->interface[0];
							const struct libusb_interface_descriptor* altsetting = &iface->altsetting[0];

							for(int ep_idx = 0; ep_idx < altsetting->bNumEndpoints; ep_idx++)
							{
								const struct libusb_endpoint_descriptor* ep = &altsetting->endpoint[ep_idx];

								/* cycle through endpoints and set up asynchronous transfers */
								if(ep->bEndpointAddress >= 0x80)
								{
									/* input endpoint, usually 0x81 */
									epIn = ep->bEndpointAddress;
									inBufferSize = ep->wMaxPacketSize;
								}
								else
								{
									/* output endpoint, usually: 0x01 */
									epOut = ep->bEndpointAddress;
									outBufferSize = ep->wMaxPacketSize;
								}
							}
						}
						libusb_free_config_descriptor(config);
						break;
					}
				}
			}
		}
		if (!is_open)
		{
			LOG() << "Failed to find device:  Bus " << bus << " addr " << addr;
		}
		//  Free list and unreference all the devices by setting unref_devices: to 1
		libusb_free_device_list(devs, 1);

		return  is_open;
	}



	//
	// Find Potential IOCards (ManifactureId == 0 AND ProductId == 0)
	//
	std::map<IOCards::IOCard_Device, std::string> IOCards::find_iocard_devices()
	{
		std::map<IOCards::IOCard_Device, std::string>available_iocard_devices;
		libusb_device** devs;

		if(LibUsbInterface::is_initialized()){
			const int cnt = static_cast<int>(libusb_get_device_list(LibUsbInterface::ctx, &devs));
			if(cnt < 0)
			{
				LOG() << "Libusb get device list Error: " << cnt;;
				IOCards::devices = "Libusb get device list Error: " + std::to_string(cnt);
			}
			else{
				libusb_device* dev;
				int i = 0;
				int iocards_cnt = 0;
				IOCards_bus_and_addr iocards_device_list[MAX_IOCARDS];

				memset(iocards_device_list, -1, sizeof(IOCards_bus_and_addr) * MAX_IOCARDS);
				while ((dev = devs[i++]) != nullptr)
				{
					libusb_device_descriptor desc{};
					const int ret = libusb_get_device_descriptor(dev, &desc);
					if (ret < 0)
					{
						LOG() << "Libusb get device descriptor Error: " << cnt;;
						IOCards::devices = "Libusb get device descriptor Error: " + std::to_string(cnt);
						break;
					}
					libusb_device_handle* handle_{ nullptr };	
					libusb_open(dev, &handle_);
					if (handle_) {
						unsigned char string[256];
						libusb_get_string_descriptor_ascii(handle_, desc.iManufacturer, string, sizeof(string));
						if (ret > 0) {
							LOG() << "Manufacturer: " << string << " Vendor id " << desc.idVendor << " Product id " << desc.idProduct;
						}
						libusb_close(handle_);
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

				//  Free list and unreference all the devices by setting unref_devices: to 1
				libusb_free_device_list(devs, 1);

				if(devices.empty())
				{
					IOCards::devices = "NO IOCARDS DEVICES";
				}
				else {
					int number_of_cards_found = 0;
					for(auto i = 0; i<iocards_cnt; i++) {
						const auto& card = iocards_device_list[i];
						if(card.bus != -1)
						{
							auto bus_address = std::to_string(card.bus) + "_" + std::to_string(card.address);

							// Identify IOCards by decoding the 4 Axes values
							// Each card has a unique pattern of hardwired (open/grd) Axes pins 
							const IOCards::IOCard_Device device_name = IOCards::identify_iocards_usb(bus_address);

							if(device_name == IOCards::MIP)
							{
								// initialize MIP
								LOG() << "Identified MIP bus _ addr " << bus_address;
								mip_bus_addr = bus_address;
								available_iocard_devices[IOCard_Device::MIP] = bus_address;
								number_of_cards_found += 1;
							}
							else if(device_name == IOCards::REAR_OVERHEAD)
							{
								// initialize Rear Overhead
								LOG() << "Identified REAR bus _ addr " << bus_address;
								rear_overhead_bus_addr = bus_address;
								available_iocard_devices[IOCard_Device::REAR_OVERHEAD] = bus_address;
								number_of_cards_found += 1;
							}
							else if(device_name == IOCards::FWD_OVERHEAD)
							{
								LOG() << "Identified FWD bus _ addr " << bus_address;
								fwd_overhead_bus_addr = bus_address;
								available_iocard_devices[IOCard_Device::FWD_OVERHEAD] = bus_address;
								number_of_cards_found += 1;
							}
							if(number_of_cards_found >= 3)
							{
								break;
							}
						}
					}					
				}
			}
		}
		return available_iocard_devices;
	}

	//
	// Identify IOCards by decoding the 4 Axes values
	// Each card has a unique pattern of hardwired (open/grd) Axes pins 
	IOCards::IOCard_Device IOCards::identify_iocards_usb(const std::string& bus_address)
	{
		LOG() << "Attempting to identify IOCards device, Bus/Address: " << bus_address;

		IOCard_Device found_device = UNKNOWN;

		IOCards usb_device(bus_address, "unknown");
		if (usb_device.is_open)
		{
			int attempts = 0;
			//initialize 4 axis
			if(usb_device.initialize_mastercard(4)){
				while (found_device == UNKNOWN && attempts < 3)
				{
					attempts++;
					int axis_read_status = 0;
					for (int tries = 0; tries < 20; tries++)
					{
						if (usb_device.receive_mastercard_synchronous() == 0)
						{
							const int axis_index = usb_device.axis - 1;
							if (axis_index >= 0 && axis_index <= 3)
							{
								axis_read_status |= 1 << axis_index;
								usb_device.axes_old[axis_index] = usb_device.axes[axis_index];

								// after all axes have been read -- look for a match
								if (axis_read_status >= 0X0F)
								{
									if (usb_device.axes_old[0] > 200 && usb_device.axes_old[1] < 50 && usb_device.axes_old[2] > 200 && usb_device.axes_old[3] < 50)
									{
										found_device = MIP;
										usb_device.set_iocard_device(MIP);
										LOG() << "Found IOCards Device: MIP,  Number of Attempts: " << attempts;
										break;
									}
									else if (usb_device.axes_old[0] < 50 && usb_device.axes_old[1] > 200 && usb_device.axes_old[2] < 50 && usb_device.axes_old[3] > 200)
									{
										LOG() << "Found IOCards Device: Rear Overhead,  Number of Attempts: " << attempts;
										found_device = REAR_OVERHEAD;
										break;
									}
									else if (usb_device.axes_old[0] < 50 && usb_device.axes_old[1] < 50 && usb_device.axes_old[2] > 200 && usb_device.axes_old[3] > 200)
									{
										LOG() << "Found IOCards Device: Forward Overhead,  Number of Attempts: " << attempts;
										found_device = FWD_OVERHEAD;
										break;
									}

									// reset for next cycle
									axis_read_status = 0;
									for(int& count : usb_device.axes_old) {
										count = 0;
									}
								}
							}
						}

						if (found_device != UNKNOWN)
						{
							LOG() << "IOCards Device is connected.  Closing USB connection.";
							break;
						}
					} // for tries
				} //while
			}
			if (found_device == UNKNOWN)
			{
				LOG() << "Error locating IOCards:  USB re-initializing.  Number of Attempts: " << attempts;
			}
			
		} // isOpen

		return found_device;
	}

	int IOCards::receive_mastercard_synchronous(void)
	{
		int recv_status = -1;

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


		// check if we have a connected USB expander card
		if (is_open && isInitialized)
		{
			constexpr int buffersize = 9;
			unsigned char recv_data[buffersize];
			int transfered;
			recv_status = libusb_interrupt_transfer(handle, epIn, recv_data, sizeof(recv_data), &transfered, 1000);

			if (recv_status == 0)
			{
				int byteCnt;
				int slot;
				int card;
				int index;
				int input[8][8]{{0}};
				/* fill the input array by bitshifting the first eight bytes */
				for (byteCnt = 0; byteCnt < 8; byteCnt++)
				{
					int x = recv_data[byteCnt];
					for (int bitCnt = 0; bitCnt < 8; bitCnt++)
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
					int axisval = recv_data[1];
					if((axis-1) < TAXES){
						axes[axis - 1] = axisval;
					}
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
					int sumslots = 0;
					int sumcards = 0;
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
					int found = 0;
					int readleft = 0;
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
						int readnine = 0;
						int sumnine = 0;
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
			}
		} // if open & init

		return recv_status;
	}



	// sends initialization string to the MASTERCARD
	bool IOCards::initialize_mastercard(unsigned char number_of_axes)
	{
		isInitialized = false;

			int buffersize = 8;
			unsigned char send_data[] = { 0x3d,0x00,0x3a,0x01,0x39,0x00,0xff,0xff };

			// card 1
			send_data[3] = 0x01;
			// set number of analog axes ZERO of USB expansion card
			send_data[5] = number_of_axes;
			int transfered;
		const auto ret = libusb_interrupt_transfer(handle, epOut, send_data, sizeof(send_data), &transfered, 1000);
		if(ret < 0){
			LOG() << "Failed to transmit IOCards initialization message.  libusb error " << ret;
		}
		if(sizeof(send_data) <= transfered) {
			LOG() << "Failed to transmit all data.  Expected " << sizeof(send_data) << " bytes, but only " << transfered << " was sent";
		}
		if(ret == 0 && sizeof(send_data) <= transfered) {
			isInitialized = true;
		}
		is_okay = is_okay == true? isInitialized : false;
		return isInitialized;
	}

	bool IOCards::init_for_async()
	{
		bool status = false;
		readTransfer = libusb_alloc_transfer(0);
		if(readTransfer)
		{
			auto size = inBuffer.size();
			if(size < inBufferSize) {
				inBuffer.resize(inBufferSize+1);
			}
			libusb_fill_interrupt_transfer(readTransfer, handle, epIn, inBuffer.data(), inBufferSize, read_callback, this, 0);

			writeTransfer = libusb_alloc_transfer(0);
			if(writeTransfer) {
				size = outBuffer.size();
				if(size < outBufferSize) {
					outBuffer.resize(outBufferSize+1);
				}
				libusb_fill_interrupt_transfer(writeTransfer, handle, epOut, outBuffer.data(), outBufferSize, write_callback, this, 0);
				status = true;
			}
		}
		is_okay = is_okay == true? status : false;
		return status;
	}



	// Runs in libusb thread
	void LIBUSB_CALL IOCards::read_callback(struct libusb_transfer* transfer)
	{
		// explicitly cast to a pointer to TClassA
		const auto mySelf = reinterpret_cast<IOCards*>(transfer->user_data);
		mySelf->read_callback_cpp(transfer);
	}

	// Runs in libusb thread
	void IOCards::read_callback_cpp(const struct libusb_transfer* transfer)
	{
		if(transfer->status != LIBUSB_TRANSFER_COMPLETED)
		{
			// read callback was not successful: exit!
			readTransfer = nullptr;
			std::lock_guard<std::mutex> lock(usb_mutex);
			event_thread_failed = true;
			LOG() << "libusb: read callback failed";
		}
		else
		{
			const auto length = transfer->actual_length;
			if(length > 0){
				std::vector<unsigned char> buffer(transfer->buffer, transfer->buffer + length);
				inQueue.push(std::move(buffer));
				LOG() << "libusb: Read callback pushed " << length << ", " << device_name;
			}

			std::lock_guard<std::mutex> lock(usb_mutex);
			if(!event_thread_failed){
				if(libusb_submit_transfer(readTransfer) < 0)
				{
					event_thread_failed = true;
					LOG() << "libusb: Submit transfer read failed";
				}
				else {
					LOG() << "libusb: Submit transfer read succeded " << device_name;
				}
			}
		}
		return;
	}

	// Runs in libusb thread
	void LIBUSB_CALL IOCards::write_callback(struct libusb_transfer* transfer)
	{
		const auto mySelf = reinterpret_cast<IOCards*>(transfer->user_data);
		mySelf->write_callback_cpp(transfer);
	}

	// Runs in libusb thread
	void IOCards::write_callback_cpp(const struct libusb_transfer* transfer)
	{
		{
			std::lock_guard<std::mutex> lock(usb_mutex);
		}
		if(transfer->status != LIBUSB_TRANSFER_COMPLETED)
		{
			// write callback was not successful: exit!
			writeTransfer = nullptr;
			std::lock_guard<std::mutex> lock(usb_mutex);
			event_thread_failed = true;
			LOG() << "libusb: write callback failed";
		}
		else {
			LOG() << "libusb: write callback completed " << device_name << " queue size = " << outQueue.size();

			if (outQueue.size() > 0) {
				if (const auto maybe_vector = outQueue.pop()) {

					std::lock_guard<std::mutex> lock(usb_mutex);
					if (maybe_vector && !event_thread_failed) {
						auto buffer = *maybe_vector;
						writeTransfer->buffer = buffer.data();
						writeTransfer->length = static_cast<int>(buffer.size());

						write_callback_running = true;
						if (libusb_submit_transfer(writeTransfer) < 0)
						{
							event_thread_failed = true;
							write_callback_running = false;
							LOG() << "libusb: write submit transfer failed";
						}
						LOG() << "libusb: Write transfer succeded";

					}
					else {
						write_callback_running = false;
					}
				}
				else {
					write_callback_running = false;
				}
			}
			else {
				write_callback_running = false;
			}

		}
	}


	bool IOCards::is_usb_thread_healthy()
	{
		std::lock_guard<std::mutex> lock(usb_mutex);
		return !event_thread_failed;
	}


	// saves a copy of all IOCARDS I/O states 
	// this is needed because, at each step, only the modified values
	// are communicated either via TCP/IP or USB to X-Plane and IOCARDS
	int IOCards::copyIOCardsData(void)
	{
//		memcpy(inputs_old, inputs, sizeof(inputs));
//		memcpy(outputs_old, outputs, sizeof(outputs));
//		memcpy(displays_old, displays, sizeof(displays));

		return (0);
	}

	void IOCards::close_down()
	{
		if (is_open)
		{
				//bool is_blocking = false;
				//{
				//	std::lock_guard<std::mutex> lock(usb_mutex);
				//	is_blocking = libusb_is_blocking;
				//}
				//while (is_blocking) {
				//	// still running send request to wake up libusb_handle_events()
				//	// THE PROBLEM: the worker is conditional wait which could last forever --
				//	// we need the iocard to send data to the usb to wake it up and worker task continue
				//	// it will then see the abort and return -- ending the task
				//	LOG() << "IOCARDS usblib sleeping --  trying to wake-up: ";
				//	std::vector<unsigned char> send_data { 0x3d,0x00,0x3a,0x01,0x39,0x00,0xff,0xff };
				//	writeTransfer->buffer = send_data.data();
				//	writeTransfer->length = static_cast<int>(send_data.size());
				//	if(libusb_submit_transfer(writeTransfer) < 0)
				//	{
				//		event_thread_failed = true;
				//	}

				//	std::this_thread::sleep_for(std::chrono::milliseconds(200));
				//	std::lock_guard<std::mutex> lock(usb_mutex);
				//	is_blocking = libusb_is_blocking;
				//}

				if (LibUsbInterface::is_initialized()) {
					if (is_Claimed && handle != nullptr) {
						auto ret = libusb_release_interface(handle, 0);
						if (ret < 0) {
							LOG() << "Cannot release libusb device: error " << ret;
						}
						is_Claimed = false;
					}
					if (handle != nullptr) {
						libusb_close(handle); // This wakes up libusb_handle_events()
						handle = nullptr;
					}
				}
			
		}
	}


	int IOCards::receive_mastercard(void)
	{
		int ret = -1;
		int result = 0;

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


		// check if we have a connected USB expander card
		if (is_okay)
		{
			while (inQueue.size() > 0 ) {
				ret = inQueue.size();
				const auto maybe_vector = inQueue.pop();
				if (maybe_vector) {
					auto recv_data = *maybe_vector;
			
					int byteCnt;
					int input[8][8];
					int index;
					int card;
					int slot;
					/* fill the input array by bitshifting the first eight bytes */
					for (byteCnt = 0; byteCnt < 8; byteCnt++)
					{
						int x = recv_data[byteCnt];
						for (int bitCnt = 0; bitCnt < 8; bitCnt++)
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
						const int axisval = recv_data[1];
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
						int sumslots = 0;
						int sumcards = 0;
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
						int found = 0;
						int readleft = 0;
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
												//if (index == 53 && card == 0)
												{
														LOG() << "reveive_mastercard: card:index =" << card << ":" << index << " slot=" << slot << " bit=" << bit << " bytecnt=" << byteCnt << " old="
															<< inputs[index][card] << " new="  <<input[byteCnt][bit];

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
							int readnine = 0;
							int sumnine = 0;
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
												
												LOG() << "reveive_mastercard BIT 9: card:index =" << card << ":" << index << " slot=" << slot << " bit=" << bit << " bytecnt=" << byteCnt << " old="
													<< inputs[index][card] << " new=" << input[byteCnt][bit];

												inputs[index][card] = input[byteCnt][bit];
											}
										}
										readnine++;
									}
								}
							}
						}
						// next byte
						byteCnt++;
					} // end while
				} 
			} // pop
		} // is_okay
		return ret;
	}


	// send changes in the outputs array (outputs and displays) to MASTERCARD
	// MASTERCARD is connected to USB EXPANSION CARD
	void IOCards::send_mastercard(void)
	{

		int send_status = 0;
		static int queue_size = 0;
		// check if we have a connected and initialized mastercard
		if (is_okay) {
			int count;
			constexpr int totchannels = 64; // total channels per card (second card starts at 64(+11), 3rd at 128(+11), 4th at 192+11))
			constexpr int channelspersegment = 8;
			constexpr int totalSegments = totchannels / channelspersegment;
			int channel;
			// TODO: check if outputs and displays work with multiple cards

			// fill send data with output information
			for (int card = 0; card < MASTERCARDS; card++)
			{
				for (int segment = 0; segment < (totalSegments); segment++)
				{
					std::vector<unsigned char> send_data = { 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff };
					if ((segment * channelspersegment) < NUM_OUTPUTS)
					{
						int changed = 0;
						// 8-byte segment transfers
						for (count = 0; count < channelspersegment; count++)
						{
							channel = segment * channelspersegment + count;
							const int power = 1 << count;

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
							constexpr int firstoutput = 11;
							send_data[0] = card * totchannels + segment * channelspersegment + firstoutput;
							outQueue.push(std::move(send_data));


							for (count = 0; count < channelspersegment; count++)
							{
								channel = count + segment * channelspersegment;
								if (channel < NUM_OUTPUTS)
								{
									LOG() << "LIBIOCARDS: send output to MASTERCARD card output [" << card << "][" << firstoutput + channel << "] " << outputs[channel][card];
								}
							}
							memcpy(outputs_old, outputs, sizeof(outputs));
						}
					}
				}
			}
			auto current_queue_size = outQueue.size();
			if (current_queue_size > 0) {
				bool is_writing = true;
				{
					std::lock_guard<std::mutex> guard(usb_mutex);
					is_writing = write_callback_running;

				}
				while(is_writing) {
					std::this_thread::sleep_for(std::chrono::milliseconds(10));
					{
						std::lock_guard<std::mutex> guard(usb_mutex);
						is_writing = write_callback_running;
					}
				}

				if (outQueue.size() > 0) {
					if (const auto maybe_vector = outQueue.pop()) {

						std::lock_guard<std::mutex> lock(usb_mutex);
						if (maybe_vector && !event_thread_failed) {
							auto buffer = *maybe_vector;
							writeTransfer->buffer = buffer.data();
							writeTransfer->length = static_cast<int>(buffer.size());

							write_callback_running = true;
							if (libusb_submit_transfer(writeTransfer) < 0)
							{
								event_thread_failed = true;
								write_callback_running = false;
								LOG() << "write submit transfer failed";
							}
							LOG() << "Write transfer succeded";

						}
						else {
							write_callback_running = false;
						}
					}
					else {
						write_callback_running = false;
					}
				}
			}
			else {
				// queue is empty
				queue_size = 0;
			}
		}
	}

	void IOCards::mastercard_send_display(unsigned char value, int pos, int card)
	{
		std::vector<unsigned char> send_data = { 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff };
		displays[pos][card] = value;
		if (displays[pos][card] != displays_old[pos][card])
		{
			// value first then position
			send_data[0] = displays[pos][card];
			send_data[1] = card * MASTERCARDS + pos;
			outQueue.push(send_data);
		}
	}

	// retrieve input value from given input position on MASTERCARD
	// Two types :
	// 0: pushbutton 
	// 1: toggle switch 
	int IOCards::mastercard_input(int input, int* value, int card)
	{
		int retval = 0; /* returns 1 if something changed, and 0 if nothing changed, and -1 if something went wrong */

		if (value != nullptr)
		{
			/* check if we have a connected and initialized mastercard */
			if (is_open && isInitialized)
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
							
							LOG() << "LIBIOCARDS: Pushbutton     : card=" << card << " input=" << input << " old value=" << inputs_old[input][card] << " new value=" << inputs[input][card];
							//
							// save value
							inputs_old[input][card] = inputs[input][card];

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

//	void IOCards::process_master_card_inputs(const OnOffKeyCommand keycmd[], int numberOfCmds, int card)
//	{
//		int* value;
//		int retval;
//		if (isOpen && isInitialized)
//		{
//			if ((card >= 0) && (card < MASTERCARDS))
//			{
//				//for (auto input = 0; input < numberOfCmds; input++)
//				//{
//				//	if (keycmd[input].on != INVALID)
//				//	{
//				//		if (inputs_old[input][card] != inputs[input][card])
//				//		{
//				//			if (inputs[input][card] != 0)
//				//			{
//				//				sendMessageInt(keycmd[input].on, 0);
//				//				LOG() << "send to buffer " << input << " new " << inputs[input][card] << " old " << inputs_old[input][card];
//				//			}
//				//			else if (keycmd[input].off != INVALID)
//				//			{
//				//				sendMessageInt(keycmd[input].off, 0);
//				//			}
//				//			inputs_old[input][card] = inputs[input][card];
//				//		}
//				//	}
//				//}
//			}
//		}
//	}
//
	#pragma optimize( "", off )  
	void IOCards::process_master_card_inputs(masterCard_input_state* switch_states[], int numberOfCmds, int card)
	{
		if (is_okay)
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

	// fill output value for given output position on MASTERCARD
	int IOCards::mastercard_output(int output, int* value, int card)
	{
		int retval = 0;
		int firstoutput = 11; // output channels start at 11, and go to 55

		if (value != nullptr)
		{
			/* check if we have a connected and initialized mastercard */
			if (is_okay)
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
							LOG() << "LIBIOCARDS: Invalid MASTERCARD output position detected: " << output;
						}
					}
					else
					{
						retval = -1;
						LOG() << "LIBIOCARDS: Invalid MASTERCARD number detected:" << card;
					}
				}
			}
			else
			{
				retval = -1;
				LOG() << "LIBIOCARDS: Device either not a MASTERCARD or not ready";
			}
		}

		return (retval);
	}
//
//	static double iocardRound(double number)
//	{
//		return number < 0.0 ? ceil(number - 0.5) : floor(number + 0.5);
//	}
//
//	/* fill display at given output position on MASTERCARD */
//	int IOCards::mastercard_display(int pos, int n0, int* value, int hasnegative, int card)
//	{
//		int retval = 0;
//		int single;
//		int count;
//		int power;
//		int tempval;
//		int negative = 0;
//		int n;
//
//		/* codes for display II card
//		10 = Put to "OFF" the digit  --> 10
//		11 = Put the "-" sign --> 0xf8
//		12 = Put a Special 6
//		13 = Put a "t"
//		14 = Put a "d"
//		15 = Put a "_" (Underscore)
//		*/
//
//		if (value != nullptr)
//		{
//			/* check if we have a connected and initialized mastercard */
//			if (isOpen && isInitialized)
//			{
//				if ((card >= 0) && (card < MASTERCARDS))
//				{
//					if ((pos >= 0) && ((pos + n0) < NUM_DISPLAYS))
//					{
//						if (*value != INT_MISS)
//						{
//							/* generate temporary storage of input value */
//							tempval = *value;
//
//							n = n0;
//
//							/* if negative values are allowed, reserve the first display for the minus sign */
//							if (hasnegative > 0)
//							{
//								n = n0 - 1;
//							}
//
//							/* reverse negative numbers: find treatment for - sign */
//							if (tempval < 0)
//							{
//								tempval = -tempval;
//								negative = 1;
//							}
//
//							/*cut high values */
//							power = static_cast<int>(iocardRound(pow(10, n)));
//							if (tempval >= power)
//							{
//								tempval = tempval - (tempval / power) * power;
//							}
//
//							/* read individual digits from integer */
//							for (count = n - 1; count >= 0; count--)
//							{
//								power = static_cast<int>(iocardRound(pow(10, count)));
//								single = tempval / power;
//								tempval = tempval - single * power;
//								displays[count + pos][card] = single;
//							}
//
//							if (hasnegative > 0)
//							{
//								if (negative == 1)
//								{
//									/* put a - sign in the frontmost display */
//									displays[pos + n][card] = 0xf8;
//								}
//								else
//								{
//									displays[pos + n][card] = 0x0a;
//								}
//							}
//						}
//					}
//					else
//					{
//						retval = -1;
//						printf("LIBIOCARDS: Invalid MASTERCARD display range detected: %i - %i \n", pos, pos + n0);
//					}
//				}
//				else
//				{
//					retval = -1;
//					printf("LIBIOCARDS: Invalid MASTERCARD number detected: %i \n", card);
//				}
//			}
//			else
//			{
//				retval = -1;
//				printf("LIBIOCARDS: Device is not ready:\n");
//			}
//		}
//
//		return (retval);
//	}
//
	// retrieve encoder value and for given encoder type from given input position on MASTERCARD
	// three type of encoders:
	// 0: 1x12 rotary switch, wired like demonstrated on OpenCockpits website
	// 1: optical rotary encoder using the Encoder II card
	// 2: 2 bit gray type mechanical encoder
	// 3: optical rotary encoder without the Encoder II card
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
			// check if we have a connected and initialized mastercard
			if (is_okay)
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

								// simulated encoder out of a 1x12 rotary switch

								if ((inputs[input][card] +
									inputs[input + 1][card] +
									inputs[input + 2][card]) == 0)
									// 0 0 0 is a wrong measurement due to switch mechanics: do not count
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
										// something has changed


									//	LOG() << "LIBIOCARDS: Rotary Encoder    1x12 Type :  card= " << card << " inputs= " << input << "-" << input + 2 << " values= " <<
									//		inputs[input][card] << " " << inputs[input + 1][card] << " " << inputs[input + 2][card];

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
											*value = *value + static_cast<float>(updown * get_acceleration(card, input, accelerator)) * multiplier;
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

									//LOG() << "LIBIOCARDS: Rotary Encoder Optical Type card = " << card <<" inputs = " << input << "-" <<  input + 1 << " values= " << inputs[input][card] << " " << inputs[input + 1][card];


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
										*value = *value + static_cast<float>(updown * get_acceleration(card, input, accelerator)) * multiplier;
										retval = 1;
									}
								}
							}

							if (type == 2)
							{
								/* 2 bit gray type encoder */

								//LOG() << "Encoder " << input + 1 << " and " << input << " = " << inputs[input+1][card] << " " << inputs[input][card];

								oldcount = (inputs_old[input + 1][card] << 1) | (inputs_old[input][card]);
								newcount = (inputs[input + 1][card] << 1) | inputs[input][card];
								if(oldcount != newcount && oldcount != -1)
								{
									/* something has changed */
									//LOG() << "Encoder old " << inputs_old[input+1][card] << " " << inputs_old[input][card] << " new " << inputs[input+1][card] << " " << inputs[input][card];

									/* forward */
									if (((oldcount == 0) && (newcount == 1)) ||
										((oldcount == 1) && (newcount == 3)) ||
										((oldcount == 3) && (newcount == 2)) ||
										((oldcount == 2) && (newcount == 0)))
									{
										updown = 1;
									}

									/* backward */
									else if (((oldcount == 2) && (newcount == 3)) ||
										((oldcount == 3) && (newcount == 1)) ||
										((oldcount == 1) && (newcount == 0)) ||
										((oldcount == 0) && (newcount == 2)))
									{
										updown = -1;
									}
									else {
										LOG() << "Encoder new value is an invalid gray code " << newcount << " old value " << oldcount;
									}

									if (updown != 0)
									{
										acceleration = get_acceleration(card, input, accelerator);
										*value = *value + (updown * acceleration * multiplier);
										//LOG() << "value = " << *value << " upDown " << updown << " accel " << acceleration << " oldCnt " << oldcount << " newCnt " << newcount;

										inputs_old[input][card] = inputs[input][card];
										inputs_old[input + 1][card] = inputs[input + 1][card];

										retval = 1;
									}
								}
								else {

									//LOG() << "Encoder Nothing Changed:   old " << inputs_old[input+1][card] << " " << inputs_old[input][card] << " new " << inputs[input+1][card] << " " << inputs[input][card] << " " << oldcount << ":" << newcount;

									inputs_old[input][card] = inputs[input][card];
									inputs_old[input + 1][card] = inputs[input + 1][card];
								}
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

								//	LOG() << "LIBIOCARDS: Rotary Encoder Phased Type card = " << card <<" inputs = " << input << "-" <<  input + 1 << " values= " << inputs[input][card] << " " << inputs[input + 1][card];


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
										*value = *value + static_cast<float>(updown * get_acceleration(card, input, accelerator)) * multiplier;
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
								LOG() << "LIBIOCARDS: Invalid MASTERCARD input position detected: " <<  input << "-" << input + 2;
							}
							else
							{
								LOG() << "LIBIOCARDS: Invalid MASTERCARD input position detected: " <<  input << "-" << input + 1;
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

	// initialize data arrays with default values
	// flight data for  USB and TCP/IP communication
	void IOCards::clear_buffers()
	{
		const auto time_new = std::chrono::high_resolution_clock::now();

		LOG() << "LIBIOCARDS: Initialization started \n";


		/* reset all inputs, axes, outputs and displays */
		for (int card = 0; card < MASTERCARDS; card++)
		{
			for (int count = 0; count < NUM_INPUTS; count++)
			{
				inputs[count][card] = -1;
				inputs_old[count][card] = -1;
				time_enc[count][card] = time_new;
			}
			for (int count = 0; count < TAXES; count++)
			{
				axes[count] = 0;
				axes_old[count] = 0;
			}
			for (int count = 0; count < NUM_OUTPUTS; count++)
			{
				outputs[count][card] = 0;
				outputs_old[count][card] = -1;
			}
			for (int count = 0; count < NUM_DISPLAYS; count++)
			{
				displays[count][card] = 0;
				displays_old[count][card] = -1;
			}
		}
	}

	// this routine calculates the acceleration of rotary encoders based on last rotation time
	int IOCards::get_acceleration(int card, int input, double accelerator)
	{
		constexpr double mintime = 0.001;
		constexpr double maxtime = 0.8;

		/* get new time */
		const auto t1 = std::chrono::high_resolution_clock::now();
		const auto t2 = time_enc[input][card];

		const auto dt_ms = std::chrono::duration<double, std::milli>(t2 - t1);
		double dt = -dt_ms.count() / 1000.0;

		if (dt < mintime)
		{
			dt = mintime;
		}

		const int acceleration = 1 + static_cast<int>((maxtime / dt) * accelerator);

		//LOG() << "LIBIOCARDS: difference= " << dt << " [seconds], acceleration=" << acceleration << " for input= " << input << " card= " << card;


		time_enc[input][card] = t1;

		return acceleration;
	}
}