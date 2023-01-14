#pragma once

#include<string>

#include "fwd_overhead_iocard.hpp"
#include "iocards.hpp"
#include "rear_overhead_init_data.hpp"

namespace zcockpit::cockpit::hardware
{

	class RearOverheadIOCard : public IOCards
	{
	public:
		RearOverheadIOCard() = delete;
		explicit RearOverheadIOCard(AircraftModel& ac_model, std::string deviceBusAddr);

		[[nodiscard]] static std::unique_ptr<RearOverheadIOCard> create_iocard(AircraftModel& ac_model, const std::string& bus_address);


		void initialize_switches();
		void process_rear_over_head();

		static bool is_running(){return running;}
		static std::string get_bus_addr(){return iocard_bus_addr;}
	private:
		AircraftModel& aircraft_model;

		static std::string iocard_bus_addr;
		static bool running;


		int l_IRU_Off{-1};
		int l_IRU_Align{-1};
		int l_IRU_Nav{-1};
		int l_IRU_Att{-1};
		int r_IRU_Off{-1};
		int r_IRU_Align{-1};
		int r_IRU_Nav{-1};
		int r_IRU_Att{-1};
		int l_IRU_SysDspl{-1};
		int r_IRU_SysDspl{-1};
		int IRU_DsplSelTst{-1};
		int IRU_DsplSelTK{-1};
		int IRU_DsplSelPos{-1};
		int IRU_DsplSelWnd{-1};
		int IRU_DsplSelHdg{-1};
		int Pass_Oxygen{-1};
		int L_EEC_ON{1};
		int R_EEC_ON{1};
		int Airspd_Warn_1{1};
		int Airspd_Warn_2{0};
		int Stall_Warn_1{1};
		int Stall_Warn_2{1};

		int disconnect_2{0};

		common::EnumArray<RearSwitchPosition, ZcockpitSwitch> iocard_rear_overhead_zcockpit_switches;

	};


}
