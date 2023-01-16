#include "HidInterfaceIT.hpp"
#include <locale>
#include <codecvt>
#include "Logger.hpp"
#include <iso646.h>

extern logger LOG;

//HidInterfaceIT* HidInterfaceIT::hidInterfaceITInstance = nullptr;

//HidInterfaceIT::HidInterfaceIT()
//{
//}

void HidInterfaceIT::drop()
{
	// now we can stop the timer
	for(auto& device : Bound_Devices)
	{
		device.tm.drop();
	}
	for(auto& timer_thread : threads)
	{
		if (timer_thread.joinable())
		{
			timer_thread.join();
		}
	}

	static std::mutex mutex;
	std::lock_guard<std::mutex> lock(mutex);
	is_dropped = true;
//	delete hidInterfaceITInstance;
//	hidInterfaceITInstance = 0;
}


HidInterfaceIT::~HidInterfaceIT()
{
	LOG() << "HidInterfaceIT dropped";
	if (!is_dropped) {
		drop();
	}
}

int HidInterfaceIT::pOpenControllers()
{
	if(!Devices.empty())
	{
		return IITAPI_ERR_CONTROLLERS_ALREADY_OPENED;
	}

	struct hid_device_info *devs = hid_enumerate(0x7C0, 0x1583);
	struct hid_device_info *cur_dev = devs;

	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;

	while (cur_dev) {
		if (cur_dev->interface_number == 1 && cur_dev->serial_number != nullptr)
		{
			std::string converted_str = converter.to_bytes(cur_dev->serial_number);

			const std::string controller = std::string("1583").append(converted_str);
			const auto path = std::string(cur_dev->path);
			const InterfaceIT_Devices device{ path, controller};
			Devices.push_back(device);

			const Interfaceit_Bound_Devices bound;
			Bound_Devices.push_back(bound);
		}
		cur_dev = cur_dev->next;
	}
	hid_free_enumeration(devs);

	if(Devices.empty())
	{
		return IITAPI_ERR_NO_CONTROLLERS_FOUND;
	}
	return IITAPI_ERR_OK;
}

int HidInterfaceIT::pCloseControllers()
{
	if (Devices.empty())
	{
		return IITAPI_ERR_CONTROLLERS_NOT_OPENED;
	}
	for(auto& bound : Bound_Devices)
	{
		if (bound.is_open)
		{
			hid_close(bound.handle);
			bound.handle = nullptr;
			bound.is_open = false;
			bound.led_enabled = false;
			bound.sw_poll_enabled = false;
		}
	}

	/* Free static HIDAPI objects. */
	hid_exit();

	return IITAPI_ERR_OK;
}

int HidInterfaceIT::pBind(std::string controller, unsigned long * session)
{
	if (Devices.empty())
	{
		return IITAPI_ERR_CONTROLLERS_NOT_OPENED;
	}
	for(auto i=0; i<Devices.size(); i++)
	{
		const auto& device = Devices[i];
		if(controller == device.controller)
		{
			if(Bound_Devices[i].is_open)
			{
				return IITAPI_ERR_CONTROLLER_ALREADY_BOUND;
			}

			const auto handle = hid_open_path(device.path.c_str());
			if (!handle)
			{
				return IITAPI_ERR_RETRIEVING_CONTROLLER;
			}
			Bound_Devices[i].handle = handle;
			Bound_Devices[i].is_open = true;
			*session = i;

			//TODO When get_info from board works then set sizes based on data from card
			// Temp workaround
			// Default size are set for FDS-Sys3  i.e. MIP controller
			const std::string OVERHEAD_CONTROLLER = "158300000592";
			if (controller == OVERHEAD_CONTROLLER)
			{
				Bound_Devices[i].sw_size = 64;
				Bound_Devices[i].led_size = 256;
			}

			return IITAPI_ERR_OK;
		}
	}
	return IITAPI_ERR_INVALID_CONTROLLER_NAME;
}

int HidInterfaceIT::pUnBind(const unsigned long session)
{
	if(session < Bound_Devices.size())
	{
		auto& bound = Bound_Devices[session];
		if (bound.is_open)
		{
			hid_close(bound.handle);
			bound.handle = nullptr;
			bound.is_open = false;
			bound.led_enabled = false;
			return IITAPI_ERR_OK;
		}
	}
	return IITAPI_ERR_INVALID_CONTROLLER_POINTER;
}

int HidInterfaceIT::pGetDeviceList(char * buffer, unsigned long * dwSize, char * pBoardType)
{
	if(Devices.empty())
	{
		return IITAPI_ERR_CONTROLLERS_NOT_OPENED;
	}
	if (pBoardType != nullptr)
	{
		auto board_type = std::string(pBoardType);
		if (board_type.size() != 4)
		{
			return IITAPI_ERR_PARAMETER_LENGTH_INCORRECT;
		}

		// TODO add code to get board info then only return data for matching type

	}
	int buf_size = 1;
	for(auto& device : Devices)
	{
		buf_size += device.controller.size() + 1;
	}

	if(buffer != nullptr)
	{
		if(*dwSize < buf_size)
		{
			return IITAPI_ERR_BUFFER_NOT_LARGE_ENOUGH;
		}
		//setup converter
		for (const auto& device : Devices)
		{
			const int num_chars = device.controller.size();
			strncpy_s(buffer, *dwSize, device.controller.c_str(), num_chars);
			buffer += num_chars;
			*buffer = NULL;
			buffer++;
		}
		*buffer = NULL;
	}
	*dwSize = buf_size;

	return IITAPI_ERR_OK;
}

int HidInterfaceIT::pLED_Enable(const unsigned long session, const bool enable)
{
	if (session < Bound_Devices.size())
	{
		auto& bound = Bound_Devices[session];
		if(bound.is_open)
		{
			memset(buf, 0, 8);
			buf[0] = 0x14;

			// ENABLE
			if(enable)
			{
				if (bound.led_enabled)
				{
					return IITAPI_ERR_ALREADY_ENABLED;
				}
				// Send Report to the device
				buf[1] = 0x01;
				if(hid_write(bound.handle, buf, 8) < 0)
				{
					return IITAPI_ERR_FAILED;
				}
				bound.led_enabled = true;

			}
			// DISABLE
			else
			{
				// Send Report to the device
				buf[1] = 0x00;
				if (hid_write(bound.handle, buf, 8) < 0)
				{
					return IITAPI_ERR_FAILED;
				}
				bound.led_enabled = false;
			}

			// set LED state to off
			for(auto i = 0; i<bound.led_size; i++)
			{
				bound.led_state[i] = false;
			}
			for(auto bit : bound.led_buffer)
			{
				bit = 0;
			}
			return IITAPI_ERR_OK;
		}
		else
		{
			return IITAPI_ERR_INVALID_CONTROLLER_POINTER;
		}
	}
	return IITAPI_ERR_INVALID_CONTROLLER_POINTER;
}

int HidInterfaceIT::pLED_Set(const unsigned long session, int nLED, const bool on)
{
	if (session < Bound_Devices.size())
	{
		auto& bound = Bound_Devices[session];
		nLED -= 1;
		if (!bound.led_enabled)
		{
			return IITAPI_ERR_NOT_ENABLED;
		}
		if (bound.is_open && nLED >=0 && nLED < bound.led_size)
		{
			// has the LED changed states
			if (on != bound.led_state[nLED])
			{

				// LEDs are row of columns -- 8 LED to a row
				// buf[1]  == columns 0 - 7
				// buf[2]  == rows 0 - 3  i.e. 32 LED
				//
				// The row byte must include all rows
				// therefore, the value must be maintained between calls.
				// and the value is 'ORed' or 'ANDed' depending on on/off state
				//
				const auto row = nLED / 8;
				const int column = nLED % 8;
				uint32_t mask = 1 << row;

				// Turn ON 
				if (on)
				{
					bound.led_buffer[column] = bound.led_buffer[column] | mask;
				}
				// Turn OFF
				else
				{
					// complement mask
					mask = ~mask;
					bound.led_buffer[column] = bound.led_buffer[column] & mask;
				}

				memset(buf, 0, 8);
				buf[0] = 0x15;
				buf[1] = column;
				buf[2] = 0X00FF & (bound.led_buffer[column]);
				buf[3] = 0X00FF & (bound.led_buffer[column] >> 8);
				buf[4] = 0X00FF & (bound.led_buffer[column] >> 16);
				buf[5] = 0X00FF & (bound.led_buffer[column] >> 24);

				if (hid_write(bound.handle, buf, 8) < 0)
				{
					return IITAPI_ERR_FAILED;
				}

				bound.led_state[nLED] = on;
			}
			return IITAPI_ERR_OK;
		}
		else
		{
			return IITAPI_ERR_INVALID_CONTROLLER_POINTER;
		}
	}
	return IITAPI_ERR_INVALID_CONTROLLER_POINTER;
}

int HidInterfaceIT::pSwitch_Enable_Poll(const unsigned long session, const bool enable)
{
	if (session < Bound_Devices.size())
	{
		auto& bound = Bound_Devices[session];
		if (bound.is_open)
		{
			memset(buf, 0, 8);
			buf[0] = 0x18;

			// ENABLE
			if (enable)
			{
				if (bound.sw_poll_enabled)
				{
					return IITAPI_ERR_ALREADY_ENABLED;
				}
				// Send Report to the device
				buf[1] = 0x01;
				if (hid_write(bound.handle, buf, 8) < 0)
				{
					return IITAPI_ERR_FAILED;
				}
				bound.sw_poll_enabled = true;

			}
			// DISABLE
			else
			{
				// Send Report to the device
				buf[1] = 0x00;
				if (hid_write(bound.handle, buf, 8) < 0)
				{
					return IITAPI_ERR_FAILED;
				}
				bound.sw_poll_enabled = false;
			}
			return IITAPI_ERR_OK;
		}
		else
		{
			return IITAPI_ERR_INVALID_CONTROLLER_POINTER;
		}
	}
	return IITAPI_ERR_INVALID_CONTROLLER_POINTER;
}

int HidInterfaceIT::pSwitch_Get_Item(unsigned long session, int * switch_num, int * direction)
{

	if (session < Bound_Devices.size())
	{
		auto& bound = Bound_Devices[session];

		if (bound.is_open)
		{
			memset(buf, 0, 8);
			buf[0] = 0x19;


			if(bound.thread_running)
			{
				std::lock_guard<std::mutex> lock(bound.tm.interfaceIT_timer_mutex);
				{

					if (bound.tm.switch_stack.empty())
					{
						return IITAPI_ERR_NO_ITEMS;
					}
					auto& state = bound.tm.switch_stack.top();
					bound.tm.switch_stack.pop();
					*switch_num = std::get<0>(state);
					*direction = std::get<1>(state);
				}
				if(*switch_num > bound.sw_size)
				{
					return IITAPI_ERR_NO_ITEMS;
				}
				return IITAPI_ERR_OK;
			}

			if (!bound.sw_poll_enabled)
			{
				return IITAPI_ERR_NOT_ENABLED;
			}
			if (hid_write(bound.handle, buf, 8) < 0)
			{
				return IITAPI_ERR_FAILED;
			}

			// start the thread
			//
			LOG() << "Starting Timer for InterfaceIT Reads";
			// start timer thread
			std::thread timer_thread = std::thread([&bound]
			{
				bound.tm.timer(bound.handle);
			});

			threads.push_back(std::move(timer_thread));				
			bound.thread_running = true;

			return IITAPI_ERR_NO_ITEMS;
		}
		else
		{
			return IITAPI_ERR_INVALID_CONTROLLER_POINTER;
		}
	}
	return IITAPI_ERR_INVALID_CONTROLLER_POINTER;
}

int HidInterfaceIT::pSwitch_Get_State(unsigned long session, int switch_num, int * state)
{
	uint64_t sw_state = 0;
	std::lock_guard<std::mutex> lock(Bound_Devices[session].tm.interfaceIT_timer_mutex);
	{
		if (Bound_Devices[session].tm.ls_sw_state_ptr == nullptr || Bound_Devices[session].tm.ms_sw_state_ptr == nullptr)
		{
			return IITAPI_ERR_NO_ITEMS;
		}
		// TODO fix warning on left shift -- too large
		sw_state = Bound_Devices[session].tm.ls_sw_state | (Bound_Devices[session].tm.ms_sw_state << 32);
	}
	const uint64_t mask = 1 << switch_num;
	if(sw_state & mask)
	{
		*state = 1;
	}
	else
	{
		*state = 0;
	}
	return IITAPI_ERR_OK;
}

int HidInterfaceIT::pEnable_Device_Change_CallBack(bool enable, INTERFACEIT_DEVICE_CHANGE_NOTIFY_PROC proc)
{
	return 0;
}


void Timer_Data::timer(hid_device * handle)
{

	unsigned char buffer[64];
	memset(buffer, 0, 8);


	stop_timer = false;
	timer_has_stopped = false;
	int bytes_read = -1;
	if (handle != nullptr)
	{
		while (true)
		{
			// do we need to shut down
			{
				std::lock_guard<std::mutex> lock(interfaceIT_timer_mutex);
				if (stop_timer)
				{
					LOG() << "InterfaceIT Read timer is stopping";

					break;
				}
			}

			try
			{
				do
				{

					// read switch state
					buffer[0] = 0;
					if (bytes_read = hid_read_timeout(handle, buffer, 8, 50) < 0)
					{
						throw std::runtime_error("unable to Read USB buffer ");
					}

					// process data
					// 0X19 == switches 1 - 32
					if (buffer[0] == 0x19)
					{
						uint32_t* bits = reinterpret_cast<uint32_t*>(&buffer[1]);
						if (ls_sw_state_ptr == nullptr)
						{
							//
							// First time through --> Initialize state, send all switches
							//
							ls_sw_state_ptr = &ls_sw_state;

							uint32_t mask = 1;
							std::lock_guard<std::mutex> lock(interfaceIT_timer_mutex);
							{
								for (int i = 0; i < 32; i++)
								{
									bool state = false;
									if (*bits & mask)
									{
										state = true;
									}
									switch_stack.push(std::make_tuple(i + 1, state));
									//LOG() << "sw " << i << " = " << state;
									mask <<= 1;
								}
							}
						}
						else
						{
							uint32_t mask = 1;
							for (int i = 0; i < 32; i++)
							{ 
								//iterate through bit mask
								uint32_t bit_state = *bits & mask;
								if (bit_state != (*ls_sw_state_ptr & mask))
								{
									// switch changed
									bool state = false;
									if (bit_state)
									{
										state = true;
									}
									//LOG() << "sw " << i << " = " << state;
									std::lock_guard<std::mutex> lock(interfaceIT_timer_mutex);
									{
										switch_stack.push(std::make_tuple(i + 1, (state)));
									}
								}
								mask <<= 1;
							}
						}
						*ls_sw_state_ptr = *bits;
					}

					//
					// Switches 33 - 64
					if (buffer[0] == 0x1A)
					{
						uint32_t* bits = reinterpret_cast<uint32_t*>(&buffer[1]);
						if (ms_sw_state_ptr == nullptr)
						{
							//
							// First time through --> Initialize state, send all switches
							//
							ms_sw_state_ptr = &ms_sw_state;

							uint32_t mask = 1;
							std::lock_guard<std::mutex> lock(interfaceIT_timer_mutex);
							{
								for (int i = 0; i < 32; i++)
								{
									bool state = false;
									if (*bits & mask)
									{
										state = true;
									}
									switch_stack.push(std::make_tuple(i + 33, state));
									//LOG() << "sw " << i << " = " << state;
									mask <<= 1;
								}
							}
						}
						else
						{
							uint32_t mask = 1;
							for (int i = 0; i < 32; i++)
							{ //iterate through bit mask
								uint32_t bit_state = *bits & mask;
								if (bit_state != (*ms_sw_state_ptr & mask))
								{
									// switch changed
									bool state = false;
									if (bit_state)
									{
										state = true;
									}
									//LOG() << "sw " << i << " = " << state;
									std::lock_guard<std::mutex> lock(interfaceIT_timer_mutex);
									{
										switch_stack.push(std::make_tuple(i + 33, (state)));
									}
								}
								mask <<= 1;
							}
						}
						*ms_sw_state_ptr = *bits;
					}
				} while (bytes_read > 0);
			}
			catch (std::exception& e)
			{
				LOG() << "InterfaceIT Exception: " << e.what();
			}

			bytes_read = -1;

			//
			// sleep
			//
			// update rate is 20Hz
			std::this_thread::sleep_for(std::chrono::milliseconds(UPDATE_RATE));

		}
	}
	// timer  has stopped
	LOG() << "InterfaceIT Read timer has stopped";

	std::unique_lock<std::mutex> lk(timer_done_mutex);
	timer_has_stopped = true;
	lk.unlock();
	condition.notify_one();
	LOG() << "InterfaceIT Read timer returning";
}

void Timer_Data::drop()
{
	std::unique_lock<std::mutex> lk(timer_done_mutex);
	{
		std::lock_guard<std::mutex> lock(interfaceIT_timer_mutex);
		stop_timer = true;
		LOG() << "InterfaceIT Timer to terminate";
	}
	condition.wait(lk, [&]
	{
		return timer_has_stopped;
	});

}
