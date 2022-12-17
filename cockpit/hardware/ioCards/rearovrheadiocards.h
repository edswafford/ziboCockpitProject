#ifndef REAROVRHEADIOCARDS_H
#define REAROVRHEADIOCARDS_H

#include<string>

#include "iocards.h"

namespace zcockpit::cockpit::hardware
{

	class RearOvrHeadIOCards : public IOCards
	{
	public:
		RearOvrHeadIOCards(std::string deviceBusAddr);
		void fastProcessRearOvrHead();
		void processRearOvrHead();

	private:


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
#endif // REAROVRHEADIOCARDS_H
