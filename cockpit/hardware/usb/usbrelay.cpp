#include "usbrelay.hpp"
#include "../common/logger.hpp"
extern logger LOG;

namespace zcockpit::cockpit::hardware
{
	USBRelay::USBRelay(AircraftModel& ac_model, unsigned short vid_, unsigned short pid_) : aircraft_model(ac_model), vid(vid_), pid(pid_)
	{
		open();
	}

	bool USBRelay::open()
	{
		unsigned char buf[9];

		// VID, PID
		struct hid_device_info* devs = hid_enumerate(vid, pid);
		struct hid_device_info* cur_dev = devs;
		if(cur_dev != nullptr && cur_dev->vendor_id == vid && cur_dev->product_id == pid)
		{
			hid_free_enumeration(devs);
			handle = hid_open(vid, pid, nullptr);
			if(handle != nullptr)
			{
				is_open = true;

				// Set the hid_read() function to be non-blocking.
				hid_set_nonblocking(handle, 1);

				relay_state = relay_state | 0xC0;
				// command 
				buf[0] = 0x00;
				buf[1] = relay_state;
				buf[2] = 0x06;
				buf[3] = 0x0E;
				buf[4] = 0x00;
				buf[5] = 0x7A;
				buf[6] = 0x71;
				buf[7] = 0x7F;
				buf[8] = 0x75;
				hid_write(handle, buf, 9);
			}
			else
			{
				is_open = false;
			}
		}
		return is_open;
	}

	constexpr int ENG1_RELAY = 5;
	constexpr int ENG2_RELAY = 4;
	constexpr int YAW_DAMPER = 3;
	constexpr int WING_ANTI_ICE = 2;
	constexpr int REAR_PANEL_LIGHTS = 1;
	constexpr int FWD_PANEL_LIGHTS = 0;
	constexpr int ON = 0X0;
	constexpr int ENG_1_BIT = 0X20;
	constexpr int ENG_2_BIT = 0X10;
	constexpr int YAW_BIT = 0X08;
	constexpr int WING_ANTI_BIT = 0X04;
	constexpr int REAR_LIGHT_BIT = 0X02;
	constexpr int FWD_LIGHT_BIT = 0X01;

	void USBRelay::process()
	{
		if(aircraft_model.z738_ac_power_is_on())
		{
			// ON == ZERO
			if((relay_state & 0X07) != ON)
			{
				// turn on relays 0, 1 & 2  (bit 0-2 set to 0)
				const unsigned short new_relay_state = (relay_state & 0XF8) | 0XC0;
				set_relays(new_relay_state);
			}
		}
		else
		{
			// OFF == 1
			if((relay_state & 0X0F) != 0X0F)
			{
				const unsigned short new_relay_state = (relay_state | 0X0F) | 0XC0;
				set_relays(new_relay_state);
			}
		}

		if(aircraft_model.z738_ac_power_is_on())
		{
			if(yaw_damper == 0)
			{
				//LOG() << "yaw_damper = 0 relay state = " << (relay_state & 0X20);
				yaw_damper++;
				if((relay_state & YAW_BIT) != ON)
				{
					set_relay(YAW_DAMPER, 1);
					LOG() << "yaw damper relay state to 1 ";
				}
			}
			else if(yaw_damper == 1)
			{
				//LOG() << "yaw damper = 1 relay state = " << (relay_state & 0X20);
				if (aircraft_model.xplane_switch_data.yaw_dumper_pos != 0)
				{
					yaw_damper++;
					LOG() << "XPlane yaw damper  is still zero == " << yaw_damper;
				}
				if((relay_state & YAW_BIT) != ON)
				{
					LOG() << "#2 setting relay state to   current state =  " << (relay_state & 0X20);
					set_relay(YAW_DAMPER, 1);
				}
			}
			else if(yaw_damper == 2)
			{
				LOG() << "yaw damper = 2 relay state = " << (relay_state & 0X20);
				if (aircraft_model.xplane_switch_data.yaw_dumper_pos == 0)
				{
					LOG() << "yaw damper #3 XPlane FCTL_YawDamper_Sw = 0 turning off relay";
					if ((relay_state & YAW_BIT) != YAW_BIT)
					{
						set_relay(YAW_DAMPER, 0);
					}
					else
					{
						yaw_damper = 0;
					}
				}
			}


			if(eng1_mag == 0)
			{
				//  LOG() << "#1 eng1_mag = 0 relay state = " << (relay_state & ENG_1_BIT);
				eng1_mag++;
				if((relay_state & ENG_1_BIT) != ON)
				{
					set_relay(ENG1_RELAY, 1);
					//    LOG() << "#1 set eng1 relay state to 1 ";
				}
			}
			else if(eng1_mag == 1)
			{
				//LOG() << "#2 eng1_mag = 1 relay state = " << (relay_state & 0X20);
	            if(aircraft_model.xplane_switch_data.starter1_pos == 0)
				{
					eng1_mag++;
					//  LOG() << "#2 incrementing eng1_mag == " << eng1_mag;
				}
				if((relay_state & ENG_1_BIT) != ON)
				{
					//  LOG() << "#2 setting relay state to   current state =  " << (relay_state & 0X20);
					set_relay(ENG1_RELAY, 1);
				}
			}
			else if(eng1_mag == 2)
			{
				//LOG() << "#3 eng1_mag = 2 relay state = " << (relay_state & 0X20);
	            if(aircraft_model.xplane_switch_data.starter1_pos > 0)
				{
					//  LOG() << "#3 packData eng 1start >0 setting eng1_mag to 0  setting relay to off,  packed data == " << client->shareMemSDK->Engine_1_Start_Switch_Status;
					if((relay_state & ENG_1_BIT) != ENG_1_BIT)
					{
						set_relay(ENG1_RELAY, 0);
					}
					else
					{
						eng1_mag = 0;
					}
				}
			}

			if(eng2_mag == 0)
			{
				// LOG() << "#4 eng1_mag = 0 incrementing to 1,  relay state = " << (relay_state & 0X20);
				if((relay_state & ENG_2_BIT) != ON)
				{
					eng2_mag++;
					set_relay(ENG2_RELAY, 1);
					//   LOG() << "#4 setting relay to on,  relay state = " << (relay_state & 0X20);
				}
			}
			else if(eng2_mag == 1)
			{
	            if(aircraft_model.xplane_switch_data.starter2_pos == 0)
				{
					eng2_mag++;
				}
				if((relay_state & ENG_2_BIT) != ON)
				{
					eng2_mag++;
					set_relay(ENG2_RELAY, 1);
				}
			}
			else if(eng2_mag == 2)
			{
	            if(aircraft_model.xplane_switch_data.starter2_pos > 0)
				{
					eng2_mag = 0;
					set_relay(ENG2_RELAY, 0);
				}
			}
		}
		else
		{
			// no power  turn them off
			if((relay_state & ENG_1_BIT) != ENG_1_BIT)
			{
				set_relay(ENG1_RELAY, 0);
				//LOG() << "#5 no power setting relay to off,  relay state = " << (relay_state & 0X20);
			}
			if((relay_state & ENG_2_BIT) != ENG_2_BIT)
			{
				set_relay(ENG2_RELAY, 0);
			}
		}
	}

	void USBRelay::set_relays(unsigned short relays)
	{
		relay_state = relays | 0xC0;
		unsigned char buf[9] = {0,0,0,0,0,0,0,0,0};
		buf[1] = relay_state;
		hid_write(handle, buf, 9);
	}

	void USBRelay::set_relay(int pos, unsigned short value)
	{
		value = value & 1;
		bool turnOn = true;
		if(value == 0)
		{
			turnOn = false;
		}
		value = 1 << pos;
		if(turnOn)
		{
			value = value ^ 0XFF;
			relay_state = (relay_state & value) | 0xC0;
		}
		else
		{
			relay_state = relay_state | value | 0xC0;
		}
		unsigned char buf[9] = {0,0,0,0,0,0,0,0,0};
		buf[1] = relay_state;
		hid_write(handle, buf, 9);
	}


	void USBRelay::close_down() const
	{
		hid_close(handle);
		/* Free static HIDAPI objects. */
		hid_exit();
	}

	bool USBRelay::check_errors()
	{
		if(hid_error(handle) != nullptr)
		{
			is_open = false;
		}
		return is_open;
	}
}