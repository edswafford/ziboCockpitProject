#pragma once

#include <mutex>

#include "HidInterfaceIT.hpp"
#include "../zcockpit_switch.hpp"
#include "../../aircraft_model.hpp"
#include <map>

namespace zcockpit::cockpit::hardware
{
	struct InterfaceITSwitch {
		common::SwitchType type;
		int state_to_value[2];  //   state[0] = off value, state[1] = on value

		InterfaceITSwitch(common::SwitchType type, int off_value, int on_value): type(type), state_to_value{off_value, on_value}{}
	};

	class InterfaceIT
	{
	public:
		static bool Avaliable()
		{
			return avaliable;
		}

		static void Avaliable(bool val)
		{
			avaliable = val;
		}

		static bool OverHeadAvailable()
		{
			return overHeadAvailable;
		}

		static void OverHeadAvailable(bool val)
		{
			overHeadAvailable = val;
		}

		static bool MipAvailable()
		{
			return mipAvailable;
		}

		static void MipAvailable(bool val)
		{
			mipAvailable = val;
		}

		void drop();

		void timer();

		static void validateDevices();
		static void initialize();
		static void closeInterfaceITController();
		void processMipState();
		void processOverHeadState();

		void updateOverheadLEDs()const;
		void turnOffMipLEDs() const;
		void turnOnMipLEDs() const;
		void turnOffOverheadLEDs() const;
		void turnOnOverheadLEDs() const;
		void updateMipLEDs()const;

		static void unbindMip();
		static void unbindOverhead();
		void syncMipSwitches();
		static void syncOverheadSwitches();

		static const InterfaceITSwitch overhead_switches[];
		static const InterfaceITSwitch mip_switches[];

		static constexpr int OVERHEAD_SW_SIZE = 64 + 1;
		static constexpr int MIP_SW_SIZE = 32 + 1;


		bool Bat_is_on() const
		{
			return bat_is_on;
		}

		void Bat_is_on(bool val)
		{
			bat_is_on = val;
		}

		// static unsigned char * MIP_SWITCHES_STATES[2][2];

		InterfaceIT(AircraftModel& ac_model, std::unique_ptr<HidInterfaceIT> hid_interface_it);
		InterfaceIT() = delete;
		~InterfaceIT()
		{
			closeInterfaceITController();
		}

		InterfaceIT(const InterfaceIT&) = delete;
		InterfaceIT& operator=(const InterfaceIT&) = delete;
		InterfaceIT(const InterfaceIT&&) = delete;
		InterfaceIT& operator=(const InterfaceIT&&) = delete;

	private:
		void simconnect_send_event_data(int mapped, int i){}




		int KeyPressedProc(int nGetRet, const char* controller, unsigned long* session, int nSwitch, int nState);



		void updateOverheadSwitches(int nswitch, int state);
		void updateMipSwitches(int nswitch, int state);

		static int bindController(const char* controller, unsigned long* session);
		static void checkController();

		static BOOL CALLBACK DeviceChangeProc(int nAction);
		AircraftModel& aircraft_model;
		bool bat_is_on{ false };
		bool lightTestToggleMip{ false };
		bool lightTestToggleOverhead{ false };

		static bool checkDeviceInProgress;

		static std::unique_ptr<HidInterfaceIT> hid;

		static const char* mip;
		static const char* overhead;
		static bool avaliable;
		static bool overHeadAvailable;
		static bool mipAvailable;
		static bool mipFound;
		static bool overHeadFound;
		static unsigned long mipSession;
		static unsigned long overHeadSession;


		int gear_lever_dn{-1};
		int gear_lever_up{-1};

		ZcockpitSwitch overhead_zcockpit_switches[OVERHEAD_SW_SIZE]{};
		ZcockpitSwitch mip_zcockpit_switches[MIP_SW_SIZE]{};

	};
}
