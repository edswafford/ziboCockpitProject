#pragma once

#include "hidapi.h"
#include "../../aircraft_model.hpp"


namespace zcockpit::cockpit::hardware
{
	class USBRelay
	{
	public:
		USBRelay(AircraftModel& ac_model, unsigned short vid, unsigned short pid);
		USBRelay() = delete;
		~USBRelay()= default;
		bool open();

		bool is_running() const
		{
			return is_open;
		}

		void process(void);
		void close_down() const;
		bool check_errors();

	private:
		void set_relays(unsigned short relays);
		void set_relay(int pos, unsigned short value);

		zcockpit::cockpit::AircraftModel& aircraft_model;
		unsigned short vid;
		unsigned short pid;

		bool is_open{false};
		unsigned char relay_state = 0xFF;
		hid_device* handle{nullptr};
	;
		int eng1_mag{0};
		int eng2_mag{0};
		int yaw_damper{0};
	};
}
