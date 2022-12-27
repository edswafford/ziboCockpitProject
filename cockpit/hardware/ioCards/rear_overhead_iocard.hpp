#pragma once

#include<string>

#include "fwd_overhead_iocard.hpp"
#include "iocards.hpp"

namespace zcockpit::cockpit::hardware
{

	class RearOverheadIOCard : public IOCards
	{
	public:
		RearOverheadIOCard(std::string deviceBusAddr);
		[[nodiscard]] static std::unique_ptr<RearOverheadIOCard> create_iocard(const std::string& bus_address);

		void fastProcessRearOvrHead();
		void processRearOvrHead();

		static bool is_running(){return running;}
		static std::string get_bus_addr(){return iocard_bus_addr;}
	private:

		static std::string iocard_bus_addr;
		static bool running;

		//int l_IRU_Off;
		//int l_IRU_Align;
		//int l_IRU_Nav;
		//int l_IRU_Att;
		//int r_IRU_Off;
		//int r_IRU_Align;
		//int r_IRU_Nav;
		//int r_IRU_Att;
		//int l_IRU_SysDspl;
		//int r_IRU_SysDspl;
		//int IRU_DsplSelTst;
		//int IRU_DsplSelTK;
		//int IRU_DsplSelPos;
		//int IRU_DsplSelWnd;
		//int IRU_DsplSelHdg;
		//int Pass_Oxygen;
		int L_EEC_ON;
		int R_EEC_ON;
		int Airspd_Warn_1;
		int Airspd_Warn_2;
		int Stall_Warn_1;
		int Stall_Warn_2;

		int disconnect_2;

	};


}
