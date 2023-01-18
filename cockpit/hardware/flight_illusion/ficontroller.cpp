#include "ficontroller.hpp"

#include "shared_types.hpp"
#include "../common/logger.hpp"

extern logger LOG;

namespace zcockpit::cockpit::hardware
{
	const char* FiController::overheadSerialNumber = "A1001HaR";
	const char* FiController::mipSerialNumber = "A1000kNr";

	std::map<int, FiDevice *> FiController::devices_map_by_id;




	FiController::FiController(AircraftModel& ac_model, int updates_per_second) :
		aircraft_model(ac_model), updates_per_second(updates_per_second), ftDeviceHandle(nullptr), serialNumber(nullptr), deviceValidationIndex(0)
	{
		ftd2Devices = Ftd2xxDevices::instance();
	}
	;

	void FiController::addGauge(int gaugeID, FiDevice::DEVICE_Type type, double scaleFactor, int minGaugeValue, int maxGaugeValue, int offset, double K, FiDevice::FI_DEVICE_CMD needleCmd, int max_send_count)
	{
		auto gauge = std::make_unique<FiDevice>(gaugeID, FiDevice::GAUGE, type, scaleFactor, minGaugeValue, maxGaugeValue, offset, K, needleCmd,
		                               FiDevice::NOOP, FiDevice::NOOP, max_send_count);
		gauge->checkStatus(ftDeviceHandle);
		devices_map_by_id.try_emplace(gauge->DeviceID(), gauge.get());
		devices.push_back(std::move(gauge));
	}

	void FiController::addRadio(int radioID, FiDevice::DEVICE_Type type, double scaleFactor, int minValue, int maxValue, FiDevice::FI_DEVICE_CMD radioCmd)
	{
		auto radio =  std::make_unique<FiDevice>(radioID, FiDevice::RADIO, type, scaleFactor, minValue, maxValue, 0, 0, radioCmd);
		radio->checkStatus(ftDeviceHandle);
		devices_map_by_id.try_emplace(radio->DeviceID(), radio.get());
		devices.push_back(std::move(radio));

		number_of_radios += 1;

	}

	void FiController::initialize(const char* deviceSerialNumber, FT_DEVICE_LIST_INFO_NODE* devInfo)
	{
		if (devInfo != nullptr)
		{
			this->devInfo.ID = devInfo->ID;
			this->devInfo.Flags = devInfo->Flags;
			this->devInfo.Type = devInfo->Type;
			this->devInfo.LocId = devInfo->LocId;
			this->devInfo.ftHandle = devInfo->ftHandle;
			serialNumber = deviceSerialNumber;
			valid = true;
			available = false;
			ftDeviceHandle = nullptr;
			deviceValidationIndex = 0;
		}
	}

	FiController::~FiController()
	{
		// stop the timer thread
		drop();
	}

	/*
	  devInfo.Flags:
		The flag value is a 4-byte bit map containing miscellaneous data as defined Appendix A – Type Definitions.
		Bit 0 (least significant bit) of this number indicates if the port is open (1) or closed (0).
		Bit 1 indicates if the device is enumerated as a high-speed USB device (2) or a full-speed USB device (0).
		The remaining bits (2 - 31) are reserved
	*/
	void FiController::open(const char* deviceSerialNumber)
	{
		FT_STATUS ftStatus;
		if (valid)
		{
			if (ftDeviceHandle == nullptr)
			{
				openEx();
			}
			else
			{
				// open failed or something went wrong -- handle is not null so try to close the device
				ftStatus = FT_Close(ftDeviceHandle);
				ftDeviceHandle = nullptr;
				if (ftStatus != FT_OK)
				{
					LOG() << "Closed Failed for Device: (" << serialNumber << ")  -- Error: " << ftStatus;
				}
				reopen(deviceSerialNumber);
			}
		}
		else
		{
			reopen(deviceSerialNumber);
		}
		if (available)
		{
			setTimeouts(500, 100);
			if (available)
			{
				ftStatus = FT_SetBaudRate(ftDeviceHandle, 38400);
				if (ftStatus != FT_OK)
				{
					LOG() << "Cannot set FT baud rate, Error: " << ftStatus;
				}
				if (available)
				{
					// Set 8 data bits, 1 stop bit and no parity 
					ftStatus = FT_SetDataCharacteristics(ftDeviceHandle, FT_BITS_8, FT_STOP_BITS_1, FT_PARITY_NONE);
					if (ftStatus != FT_OK)
					{
						LOG() << "Cannot set FT start stop bits, Error: " << ftStatus;
					}
				}
			}
		}
	}

	void FiController::setTimeouts(int readTimeout, int writeTimeout)
	{
		FT_STATUS ftStatus;
		// Set read/write timeout i.e. 5000 = 5sec, 1000 = 1sec 
		ftStatus = FT_SetTimeouts(ftDeviceHandle, readTimeout, writeTimeout);
		if (ftStatus != FT_OK)
		{
			LOG() << "Setting Timeout Failed for Device: (" << serialNumber << ")  -- Error: " << ftStatus;
			available = false;
		}
	}

	void FiController::reopen(const char* deviceSerialNumber)
	{
		bool has_timer = false;
		if (timer_is_running)
		{
			has_timer = true;
			// drop the timer
			drop();
		}
		// look for device
		FT_DEVICE_LIST_INFO_NODE* devInfo = ftd2Devices->getDevice(deviceSerialNumber);
		if (devInfo != nullptr)
		{
			initialize(deviceSerialNumber, devInfo);
			openEx();

			// start a new timer thread
			if (has_timer)
			{
				start_timer(ftDeviceHandle);
			}
		}
	}

	void FiController::openEx()
	{
		FT_STATUS ftStatus;
		if ((devInfo.Flags & 0X1) == 0)
		{
			ftStatus = FT_OpenEx((PVOID)serialNumber, FT_OPEN_BY_SERIAL_NUMBER, &ftDeviceHandle);
			if (ftStatus == FT_OK)
			{
				ftStatus = FT_ResetDevice(ftDeviceHandle);
				if (ftStatus == FT_OK)
				{
					available = true;
				}
				else
				{
					available = false;
					ftDeviceHandle = nullptr;
					LOG() << "Reset Failed for Device: (" << serialNumber << ")  -- Error: " << ftStatus;
				}
			}
			else
			{
				available = false;
				ftDeviceHandle = nullptr;
				LOG() << "Open Failed for Device: (" << serialNumber << ")  -- Error: " << ftStatus;
			}
		}
		else
		{
			available = false;
			if (ftDeviceHandle != nullptr)
			{
				FT_Close(ftDeviceHandle);
				ftDeviceHandle = nullptr;
				LOG() << "Tried to open Device: (" << serialNumber << ") that is already open -- Flags:" << devInfo.Flags;
			}
			else if (devInfo.ftHandle != nullptr)
			{
				FT_Close(devInfo.ftHandle);
				LOG() << "Tried to open Device: (" << serialNumber << ") that is already open -- Flags:" << devInfo.Flags;
			}
		}
	}


	void FiController::closeDown()
	{
		if (available)
		{
			devices.clear();

			FT_Close(ftDeviceHandle);
			ftDeviceHandle = nullptr;
		}
	}

	void FiController::drop()
	{
		std::unique_lock<std::mutex> lk(timer_done_mutex);
		{
			if (!timer_is_running)
			{
				// timer is not running -- nothing to do
				return;
			}
			std::lock_guard<std::mutex> lock(flightIllusion_timer_mutex);
			stop_timer = true;
			LOG() << "FLight Illusion Timer has stopped";
		}
		condition.wait(lk, [&]
		{
			return timer_has_stopped;
		});

		if (timer_thread.joinable())
		{
			timer_thread.join();
		}
		timer_is_running = false;
	}

	void FiController::start_timer(FT_HANDLE handle)
	{
		if (handle != nullptr)
		{
			LOG() << "Starting Timer for Flight Illusion Reads";
			timer_thread = std::thread([this, &handle]
			{
				this->timer(handle);
			});
		}
	}

	void FiController::timer(FT_HANDLE handle)
	{
		const int MAX_BUFFER_SIZE = 500;
		const int EXPECTED_MESSAGE_SIZE = 25;
		uint8_t buffer[MAX_BUFFER_SIZE];
		uint8_t saved_msg[EXPECTED_MESSAGE_SIZE];
		uint8_t * current_msg = nullptr;
		int partical_message_cnt = -1;

		memset(buffer, 0, MAX_BUFFER_SIZE);


		stop_timer = false;
		timer_has_stopped = false;
		int bytes_read = -1;
		if (handle != nullptr)
		{
			timer_is_running = true;
			while (true)
			{
				// do we need to shut down
				{
					std::lock_guard<std::mutex> lock(flightIllusion_timer_mutex);
					if (stop_timer)
					{
						LOG() << "Flight Illusion Read timer is stopping";
						break;
					}
				}

				try
				{
					do {
						//
						// Read will return 0, or a least 25 bytes
						//
						bytes_read = FiDevice::read(handle, buffer, MAX_BUFFER_SIZE);

						auto actual_bytes_read = bytes_read;
						if (bytes_read > MAX_BUFFER_SIZE)
						{
							// there are more message than the buffer can hold --> the buffer is full and needs to be read again
							actual_bytes_read = MAX_BUFFER_SIZE;
						}
						if (actual_bytes_read > 0)
						{
							// look for the message
							auto device_iter = devices_map_by_id.find(buffer[1]);
							if (buffer[0] == 0 && device_iter != devices_map_by_id.end())
							{
								//
								// Start of buffer == start of Message
								//

								auto device = device_iter->second;
								memcpy(device->get_buffer(), buffer, 25);
								device->bytes_read_by_timer_thread = 25;
								device->timeout_counter = 0;
								device->valid = true;
								device->fresh_data = true;

								// normal case -- one complete message
								if (actual_bytes_read == EXPECTED_MESSAGE_SIZE)
								{
									partical_message_cnt = -1;
								}
								else
								{
									// there is more than one messages in the buffer
									partical_message_cnt = -1;
									int i = 25;
									while (i < (actual_bytes_read - 1))
									{
										device_iter = devices_map_by_id.find(buffer[i + 1]);
										if (buffer[i] == 0 && device_iter != devices_map_by_id.end())
										{
											// found next message
											const auto remaining_bytes = actual_bytes_read - i;
											if (remaining_bytes >= EXPECTED_MESSAGE_SIZE)
											{
												device = device_iter->second;
												memcpy(device->get_buffer(), &buffer[i], 25);
												device->bytes_read_by_timer_thread = 25;
												device->timeout_counter = 0;
												device->valid = true;
												device->fresh_data = true;
												i += 25;
											}
											else
											{
												// only part of a message --> save it for later
												LOG() << "ERROR should nevr get part of a message";
												memcpy(saved_msg, &buffer[i], remaining_bytes);
												partical_message_cnt = remaining_bytes;
												break;
											}
										}
										else
										{
											// not at start of next message -- keep looking
											i += 1;
										}
									}
								}
							}
							else
							{
								//
								// we are in the middle of a message IF the partical_message_cnt != -1
								//
								if (partical_message_cnt != -1)
								{
									// so we are expecting at least 25 - partical_message_cnt bytes
									const auto needed_bytes = EXPECTED_MESSAGE_SIZE - partical_message_cnt;
									if (actual_bytes_read >= needed_bytes)
									{
										memcpy(&saved_msg[partical_message_cnt], buffer, needed_bytes);
										device_iter = devices_map_by_id.find(saved_msg[1]);
										if (saved_msg[0] == 0 && device_iter != devices_map_by_id.end())
										{
											auto device = device_iter->second;
											memcpy(device->get_buffer(), saved_msg, 25);
											device->bytes_read_by_timer_thread = 25;
											device->timeout_counter = 0;
											device->valid = true;
											device->fresh_data = true;
										}
										//
										// there is more than one messages in the buffer
										//
										partical_message_cnt = -1;
										int i = 25;
										while (i < (actual_bytes_read - 1))
										{
											device_iter = devices_map_by_id.find(buffer[i + 1]);
											if (buffer[i] == 0 && device_iter != devices_map_by_id.end())
											{
												// found next message
												const auto remaining_bytes = actual_bytes_read - i;
												if (remaining_bytes >= EXPECTED_MESSAGE_SIZE)
												{
													auto device = device_iter->second;
													memcpy(device->get_buffer(), &buffer[i], 25);
													device->bytes_read_by_timer_thread = 25;
													device->timeout_counter = 0;
													device->valid = true;
													device->fresh_data = true;
													i += 25;
												}
												else
												{
													// only part of a message --> save it for later
													memcpy(saved_msg, &buffer[i], remaining_bytes);
													partical_message_cnt = remaining_bytes;
													break;
												}
											}
											else
											{
												// not at start of next message -- keep looking
												i += 1;
											}
										}
									}
									else
									{
										// we got a few more bytes but not enought
										// copy what we have
										memcpy(&saved_msg[partical_message_cnt], buffer, actual_bytes_read);
										partical_message_cnt += actual_bytes_read;
									}
								}
								else
								{
									//
									// We have received data but it is not the start of a message OR the rest of a previous message
									// See if we can find a message in the data

									int i = 0;
									while (i < (actual_bytes_read - 1))
									{
										device_iter = devices_map_by_id.find(buffer[i + 1]);
										if (buffer[i] == 0 && device_iter != devices_map_by_id.end())
										{
											// found next message
											const auto remaining_bytes = actual_bytes_read - i;
											if (remaining_bytes >= EXPECTED_MESSAGE_SIZE)
											{
												auto device = device_iter->second;
												memcpy(device->get_buffer(), &buffer[i], 25);
												device->bytes_read_by_timer_thread = 25;
												device->timeout_counter = 0;
												device->valid = true;
												device->fresh_data = true;
												i += 25;
											}
											else
											{
												// only part of a message --> save it for later
												memcpy(saved_msg, &buffer[i], remaining_bytes);
												partical_message_cnt = remaining_bytes;
												break;
											}
										}
										else
										{
											// not at start of next message -- keep looking
											i += 1;
										}
									}
								}
							}
						}

					} while (bytes_read > MAX_BUFFER_SIZE);
				}
				catch (std::exception& e)
				{
					LOG() << "Flight Illusion Exception: " << e.what();
				}


				//
				// sleep
				//
				// update rate is 20Hz
				std::this_thread::sleep_for(std::chrono::milliseconds(common::UPDATE_RATE));

			}
		}
		// timer  has stopped
		LOG() << "Flight Illusion Read timer has stopped";

		std::unique_lock<std::mutex> lk(timer_done_mutex);
		timer_has_stopped = true;
		lk.unlock();
		condition.notify_one();
		LOG() << "Flight Illusion Read timer returning";
	}

	//
	// Called once per second
	// it cycles through each device and verifies
	// it responds to status command
	//
	void FiController::validateGauges()
	{
		const int numberOfGauges = static_cast<int>(devices.size());
		if (deviceValidationIndex >= numberOfGauges)
		{
			deviceValidationIndex = 0;
		}
		const bool status = devices[deviceValidationIndex]->checkStatus(ftDeviceHandle);
		if (!status)
		{
			// See if all devices on this card are invalid
			// if so try reseting the card

			unsigned numberOfInvalidGauges = 0;
			for (unsigned i = 0; i < devices.size(); i++)
			{
				if (!devices[i]->valid)
				{
					numberOfInvalidGauges++;
				}
			}
			if (numberOfInvalidGauges >= devices.size())
			{
				available = false;
				reopen(this->serialNumber);
			}
		}
		deviceValidationIndex++;
	}


	void FiController::updateLights(int light_state)
	{
		for (const auto& device : devices)
		{
			if (device->CATEGORY() == FiDevice::RADIO || device->CATEGORY() == FiDevice::CLOCK)
			{
				device->sendCommand(ftDeviceHandle, FiDevice::FI_DEVICE_CMD::SET_LIGHTS, light_state);
			}
		}
	}

	/* Standard iterative function to convert 16-bit integer to BCD */
	uint32_t dec2bcd(uint16_t dec)
	{
		uint32_t result = 0;
		int shift = 0;

		while (dec)
		{
			result += (dec % 10) << shift;
			dec = dec / 10;
			shift += 4;
		}
		return result;
	}
	static DWORD adf_freq = 1000;
	bool FiController::updateRadios()
	{

		static int read_cycle = 0;
		static int radio_index = 0;

		FiDevice* radio = nullptr;

		if (radio_index >= number_of_radios)
		{
			radio_index = 0;
		}

		int current_radio = 0;
		for (const auto& radio_device : devices)
		{
			if (radio_device->CATEGORY() == FiDevice::RADIO)
			{
				if(radio_index == current_radio) {
					radio = radio_device.get();
					break;
				}
				current_radio += 1;
			}
		}

		bool status = false;
		if(radio != nullptr){
			 status = VALID;
			DWORD bcd_value = 0;

			radio->timeout_counter += 1;
			if (radio->timeout_counter > updates_per_second)
			{
				radio->timeout_counter = updates_per_second;
				radio->valid = false;
			}

			int bytes_read = 0;

			try
			{
				if (read_cycle == 0)
				{
					radio->sendCommand(ftDeviceHandle, FiDevice::FI_DEVICE_CMD::STATUS, 0);
					read_cycle = 1;
				}
				else
				{
					bytes_read = radio->bytes_read_by_timer_thread;
					read_cycle = 0;
					radio_index += 1;
				}
			}
			catch (std::runtime_error& err)
			{
				LOG() << "Flight Illusions exception reading radios : " << err.what();
				return false;
			}

			if (bytes_read > 16 && radio->fresh_data)
			{
				radio->fresh_data = false;

				uint32_t active_display = radio->get_display_value(FiDevice::ACTIVE);
				uint32_t inactive_display = radio->get_display_value(FiDevice::INACTIVE);

				// round up and get int form of 1xxyy
				// value 22000 corresponds to 122.00
				active_display = (active_display) / 10;
				inactive_display = (inactive_display) / 10;

				const FiDevice::DEVICE_Type radio_type = radio->Type();
				switch (radio_type)
				{
				case FiDevice::CAPT_COMM:
				case FiDevice::FO_COMM:
				{
					// convert to BCD
					bcd_value = dec2bcd(active_display);
	//				DWORD current_radio_value = radio_type == FiDevice::CAPT_COMM ? FsxSimConnect::comm1_active : FsxSimConnect::comm2_active;
	//				if (bcd_value != current_radio_value)
					{
						bcd_value += 0X10000;
						//const EVENT_ID event_id = radio_type == FiDevice::CAPT_COMM ? EVENT_COM_RADIO_SET : EVENT_COM2_RADIO_SET;
						//if (SimConnect_TransmitClientEvent(FsxSimConnect::hSimConnect, 0, event_id, bcd_value,
						//	SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY) != S_OK)
						//{
						//	LOG() << "SimConnect failed to transmit Event";
						//}
					}

					// convert to BCD
					bcd_value = dec2bcd(inactive_display);
	//				current_radio_value = radio_type == FiDevice::CAPT_COMM ? FsxSimConnect::comm1_standby : FsxSimConnect::comm2_standby;
	//				if (bcd_value != current_radio_value)
					{
						bcd_value += 0X10000;
						//const EVENT_ID event_id = radio_type == FiDevice::CAPT_COMM ? EVENT_COM_STBY_RADIO_SET : EVENT_COM2_STBY_RADIO_SET;
						//if (SimConnect_TransmitClientEvent(FsxSimConnect::hSimConnect, 0, event_id, bcd_value,
						//	SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY) != S_OK)
						//{
						//	LOG() << "SimConnect failed to transmit Event";
						//}
					}
				}
				break;

				case FiDevice::CAPT_NAV:
				case FiDevice::FO_NAV:
				{
					// convert to BCD
					bcd_value = dec2bcd(active_display);
	//				auto radio_value = (radio_type == FiDevice::CAPT_NAV ? FsxSimConnect::nav1_active : FsxSimConnect::nav2_active);
	//				DWORD current_radio_value = dec2bcd(static_cast<uint32_t>(radio_value * 100.0));
	//				if ((bcd_value + 0X10000) != current_radio_value)
					{
						//const EVENT_ID event_id = radio_type == FiDevice::CAPT_NAV ? EVENT_NAV1_RADIO_SET : EVENT_NAV2_RADIO_SET;
						//if (SimConnect_TransmitClientEvent(FsxSimConnect::hSimConnect, 0, event_id, bcd_value,
						//	SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY) != S_OK)
						//{
						//	LOG() << "SimConnect failed to transmit Event";
						//}
					}

					// convert to BCD
					bcd_value = dec2bcd(inactive_display);
	//				radio_value = (radio_type == FiDevice::CAPT_NAV ? FsxSimConnect::nav1_standby : FsxSimConnect::nav2_standby);
	//				current_radio_value = dec2bcd(static_cast<uint32_t>(radio_value * 100.0));
	//				if ((bcd_value + 0X10000) != current_radio_value)
					{
						//const EVENT_ID event_id = radio_type == FiDevice::CAPT_NAV ? EVENT_NAV1_STBY_SET : EVENT_NAV2_STBY_SET;
						//if (SimConnect_TransmitClientEvent(FsxSimConnect::hSimConnect, 0, event_id, bcd_value,
						//	SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY) != S_OK)
						//{
						//	LOG() << "SimConnect failed to transmit Event";
						//}
					}
				}
				break;

				case FiDevice::ADF:
				{
					// convert to BCD
					bcd_value = dec2bcd(active_display);
	//				const auto radio_value = FsxSimConnect::adf1_active;
	//				DWORD current_radio_value = radio_value / 0x1000;
	//				if ((bcd_value * 0x10) != current_radio_value)
					{
						//if (SimConnect_TransmitClientEvent(FsxSimConnect::hSimConnect, 0, EVENT_ADF_SET, bcd_value,
						//	SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY) != S_OK)
						//{
						//	LOG() << "SimConnect failed to transmit Event";
						//}
					}

				}
				break;
				default:
					break;
				}
				
			}
		}
		return status;
	}

	//======================================================================

	// Returns interpolated value at x from parallel arrays ( xData, yData )
	//   Assumes that xData has at least two elements, is sorted and is strictly monotonic increasing
	//   boolean argument extrapolate determines behaviour beyond ends of array (if needed)

	double interpolate_flaps (const double x)
	{
		// pmdg static const vector<double> flap_value =  { 0.0,	22.2,	45.8,	62.5,	77.8,	83.3,	87.5,	91.7,	100 };
		// trailing edge flaps 0-100               up		1		2		5		10		15		25		30		40  
		static const vector<double> flap_value = { 0.0,		4.0,	7.0,	11.0,	14.0,	17.0,	27.0,	33.0,	49.0 };
		static const vector<double> gauge_value = { 0.0,	130.0,	250.0,	380.0,	500.0,	590.0,	670.0,	740.0,	820.0};

		static const int size = flap_value.size();

		int i = 0;                                                                  // find left end of interval for interpolation
		for(i=0; i< size-2; i++)
		{
			if(x >= flap_value[i] && x < flap_value[i+1])
			{
				break;
			}
		}
		const double xL = flap_value[i];
		const double yL = gauge_value[i];
		const double xR = flap_value[i + 1];
		const double yR = gauge_value[i + 1];      // points on either side 

		const double dydx = (yR - yL) / (xR - xL);                                // gradient

		return yL + dydx * (x - xL);                                              // linear interpolation
	}

	bool FiController::updateGauges()
	{
		bool status = VALID;
		for (const auto& gauge : devices)
		{
			if (gauge->CATEGORY() == FiDevice::GAUGE)
			{
				gauge->timeout_counter += 1;
				if (gauge->timeout_counter > updates_per_second)
				{
					gauge->timeout_counter = updates_per_second;
					gauge->valid = false;
				}

				switch (gauge->Type())
				{
				case FiDevice::YAW_DAMPER:
				{
					// rudder position range -1.0 to 1.0
//					double rudder = (FsxSimConnect::rudder_position + 1.0) * 1000.0;
//					status = gauge->sendValue(ftDeviceHandle, gauge->Cmd(), rudder);
				}
					break;

				case FiDevice::FLAP:
				{
					status = gauge->sendValue(ftDeviceHandle, gauge->Cmd(), interpolate_flaps(aircraft_model.z737InData.flap_indicator[0]));
				}
					break;

				case FiDevice::BRAKE_PRESS:
					status = gauge->sendValue(ftDeviceHandle, gauge->Cmd(), aircraft_model.z737InData.brake_press);
					break;

				case FiDevice::APU_EGT:
					status = gauge->sendValue(ftDeviceHandle, gauge->Cmd(), aircraft_model.z737InData.apu_temp);
					break;

				case FiDevice::PRESS_DIFF:
					status = gauge->sendValue(ftDeviceHandle, gauge->Cmd(), aircraft_model.z737InData.cabin_pressure_diff);
					break;

				case FiDevice::CABIN_ALT:
					status = gauge->sendValue(ftDeviceHandle, gauge->Cmd(), aircraft_model.z737InData.cabin_alt);
					break;

				case FiDevice::CABIN_CLIMB:
					status = gauge->sendValue(ftDeviceHandle, gauge->Cmd(), aircraft_model.z737InData.cabin_vvi);
					break;

				case FiDevice::LEFT_DUCT:
					status = gauge->sendValue(ftDeviceHandle, gauge->Cmd(), aircraft_model.z737InData.duct_press_L);
					break;

				case FiDevice::RIGHT_DUCT:
					status = gauge->sendValue(ftDeviceHandle, gauge->Cmd(), aircraft_model.z737InData.duct_press_R);
					break;

				case FiDevice::FUEL_TEMP:
					status = gauge->sendValue(ftDeviceHandle, gauge->Cmd(), aircraft_model.z737InData.fuel_temp);
					break;

				case FiDevice::CABIN_TEMP:
					status = gauge->sendValue(ftDeviceHandle, gauge->Cmd(), aircraft_model.z737InData.cabin_temp);
					break;

				case FiDevice::CREW_OXYGEN:
					//	NOT Supported
					status = gauge->sendValue(ftDeviceHandle, gauge->Cmd(), 8430.0);
					break;
				default: ;
				}
				if (status != VALID)
				{
					gauge->checkStatus(ftDeviceHandle);
				}
			}
		}

		return available;
	}
}