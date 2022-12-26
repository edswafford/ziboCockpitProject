#include "rearovrheadiocards.hpp"
#include "../DeleteMeHotkey.h"

namespace zcockpit::cockpit::hardware
{
	RearOvrHeadIOCards::RearOvrHeadIOCards(std::string deviceBusAddr)
		: IOCards(deviceBusAddr, "rearOverhead")
	{

		//l_IRU_Off = 0;
		//l_IRU_Align = 0;
		//l_IRU_Nav = 0;
		//l_IRU_Att = 0;
		//r_IRU_Off = 0;
		//r_IRU_Align = 0;
		//r_IRU_Nav = 0;
		//r_IRU_Att = 0;
		//l_IRU_SysDspl = 0;
		//r_IRU_SysDspl = 0;
		//IRU_DsplSelTst = 0;
		//IRU_DsplSelTK = 0;
		//IRU_DsplSelPos = 0;
		//IRU_DsplSelWnd = 0;
		//IRU_DsplSelHdg = 0;
		//Pass_Oxygen = 0;
	    L_EEC_ON = 1;
	    R_EEC_ON = 1;
		Airspd_Warn_1 = 1;
		Airspd_Warn_2 = 0;
		Stall_Warn_1 = 1;
		Stall_Warn_2 = 1;

		disconnect_2 = 0;
	}


	void RearOvrHeadIOCards::fastProcessRearOvrHead()
	{
		LOG() << "Calling Rear";
		process_master_card_inputs(constants::rear_overhead_to_keycmd, constants::rear_ovrhead_keycmd_size);
		LOG() << "Done Rear";
		processRearOvrHead();
	}

		void sendMessageInt(int cmd, int value){}
		bool RightEngRunning(){return true;}

	void RearOvrHeadIOCards::processRearOvrHead()
	{
		const int DEBOUNCE_MAX_COUNT = 3;
		static int gen2Disconnect = 1;
		static int gen2Counter = 0;
		static KEY_COMMAND lastGen2KeyCmd = KEY_COMMAND_ELECTRICAL_GENERATOR_DRIVE_2_DISCONNECT_UP;


		// Elec Gen 2 Disconnect
		////////////////////////////////////////////////////////////////////
		//
		//       N O T I C E
		//
		/////////////////////////////////////////////////////////////////////
		// ran out of available inputs
		// moved to rear overhead board which has spares
		//


		if (RightEngRunning())
		{
			if(mastercard_input(22, &disconnect_2))
			{
				gen2Counter = 0;
			}
			if(disconnect_2 >= 0)
			{
				if(gen2Disconnect != disconnect_2)
				{
					gen2Disconnect = disconnect_2;
					gen2Counter = 0;
				}
				gen2Counter++;

				if(gen2Counter >= DEBOUNCE_MAX_COUNT)
				{
					gen2Counter = DEBOUNCE_MAX_COUNT;
					if(disconnect_2 == 1)
					{
						if (lastGen2KeyCmd != KEY_COMMAND_ELECTRICAL_GENERATOR_DRIVE_2_DISCONNECT_UP)
						{
							lastGen2KeyCmd = KEY_COMMAND_ELECTRICAL_GENERATOR_DRIVE_2_DISCONNECT_UP;
							sendMessageInt(KEY_COMMAND_ELECTRICAL_GENERATOR_DRIVE_2_DISCONNECT_UP, 0);
						}
					}
					else
					{
						if(lastGen2KeyCmd != KEY_COMMAND_ELECTRICAL_GENERATOR_DRIVE_2_DISCONNECT_DOWN)
						{
							lastGen2KeyCmd = KEY_COMMAND_ELECTRICAL_GENERATOR_DRIVE_2_DISCONNECT_DOWN;
							sendMessageInt(KEY_COMMAND_ELECTRICAL_GENERATOR_DRIVE_2_DISCONNECT_DOWN, 0);
						}
					}
				}
			}
		}

		//// LEFT IRU MODE Switch
		//   if(mastercard_input(10, &l_IRU_Off) && l_IRU_Off)
		//   {
		//       sendMessageInt(KEY_COMMAND_FMS_IRS_1_MODE_OFF, 0); //855
		//   }

		//   if(mastercard_input(12, &l_IRU_Align) && l_IRU_Align)
		//   {
		//       sendMessageInt(KEY_COMMAND_FMS_IRS_1_MODE_ALIGN, 0); //856
		//   }

		//   if(mastercard_input(9, &l_IRU_Nav) && l_IRU_Nav)
		//   {
		//       sendMessageInt(KEY_COMMAND_FMS_IRS_1_MODE_NAV, 0); //857
		//   }

		//   if(mastercard_input(11, &l_IRU_Att) && l_IRU_Att)
		//   {
		//       sendMessageInt(KEY_COMMAND_FMS_IRS_1_MODE_ATT, 0); //858
		//   }


		//   // RIGHT IRU MODE Switch
		//   if(mastercard_input(16, &r_IRU_Off) && r_IRU_Off)
		//   {
		//       sendMessageInt(KEY_COMMAND_FMS_IRS_2_MODE_OFF, 0); //861
		//   }

		//   if(mastercard_input(15, &r_IRU_Align) && r_IRU_Align)
		//   {
		//       sendMessageInt(KEY_COMMAND_FMS_IRS_2_MODE_ALIGN, 0); //862
		//   }

		//   if(mastercard_input(13, &r_IRU_Nav) && r_IRU_Nav)
		//   {
		//       sendMessageInt(KEY_COMMAND_FMS_IRS_2_MODE_NAV, 0); //863
		//   }

		//   if(mastercard_input(14, &r_IRU_Att) && r_IRU_Att)
		//   {
		//       sendMessageInt(KEY_COMMAND_FMS_IRS_2_MODE_ATT, 0); //864
		//   }


		//   // IRU Sys Display
		//   if(mastercard_input(0, &l_IRU_SysDspl) && l_IRU_SysDspl)
		//   {
		//       sendMessageInt(KEY_COMMAND_FMS_IRS_SYS_DSPL_L, 0); //839
		//   }

		//   if(mastercard_input(1, &r_IRU_SysDspl) && r_IRU_SysDspl)
		//   {
		//       sendMessageInt(KEY_COMMAND_FMS_IRS_SYS_DSPL_R, 0); //840
		//   }


		//   // IRU Display Select
		//   if(mastercard_input(3, &IRU_DsplSelTst) && IRU_DsplSelTst)
		//   {
		//       sendMessageInt(KEY_COMMAND_FMS_IRS_DSPL_SEL_POS1, 0); //833
		//   }

		//   if(mastercard_input(6, &IRU_DsplSelTK) && IRU_DsplSelTK)
		//   {
		//       sendMessageInt(KEY_COMMAND_FMS_IRS_DSPL_SEL_POS2, 0); //834
		//   }

		//   if(mastercard_input(7, &IRU_DsplSelPos) && IRU_DsplSelPos)
		//   {
		//       sendMessageInt(KEY_COMMAND_FMS_IRS_DSPL_SEL_POS3, 0); //835
		//   }

		//   if(mastercard_input(5, &IRU_DsplSelWnd) && IRU_DsplSelWnd)
		//   {
		//       sendMessageInt(KEY_COMMAND_FMS_IRS_DSPL_SEL_POS4, 0); //836
		//   }

		//   if(mastercard_input(2, &IRU_DsplSelHdg) && IRU_DsplSelHdg)
		//   {
		//       sendMessageInt(KEY_COMMAND_FMS_IRS_DSPL_SEL_POS5, 0); //837
		//   }

		//   if (mastercard_input(27, &Pass_Oxygen) )
		//   {
		//       if ( Pass_Oxygen)
		//       {
		//           sendMessageInt(KEY_COMMAND_GENERAL_PASS_OXYGEN_ON, 0); //43
		//       }
		//       else
		//       {
		//           sendMessageInt(KEY_COMMAND_GENERAL_PASS_OXYGEN_NORMAL, 0); //42  
		//       }
		//   }

		   // EEC On/ATN
	    if(mastercard_input(17, &L_EEC_ON))
	    {
	        if(L_EEC_ON == 1)
	        {
	            sendMessageInt(KEY_COMMAND_ENGAPU_EEC_1_ON, 0); //504
	        }
	        else
	        {
	            sendMessageInt(KEY_COMMAND_ENGAPU_EEC_1_OFF, 0); //505  
	        }
	    }

	    if(mastercard_input(8, &R_EEC_ON))
	    {
	        if(R_EEC_ON == 1)
	        {
	            sendMessageInt(KEY_COMMAND_ENGAPU_EEC_2_ON, 0); //507
	        }
	        else
	        {
	            sendMessageInt(KEY_COMMAND_ENGAPU_EEC_2_OFF, 0); //508  
	        }
	    }

		// Airspeed Warning Test
	    if(mastercard_input(33, &Airspd_Warn_1) && Airspd_Warn_1 == 0)
		{
	        sendMessageInt(KEY_COMMAND_WARNING_AIRSPEED_TEST, 0); //1195
			}
	    if(mastercard_input(31, &Airspd_Warn_2) && Airspd_Warn_2)
		{
	        sendMessageInt(KEY_COMMAND_WARNING_AIRSPEED_TEST, 0); //1195
			}


		// Stall Warning
	    if(mastercard_input(28, &Stall_Warn_1) && Stall_Warn_1 == 0)
		{
	        sendMessageInt(KEY_COMMAND_WARNING_STALL_TEST, 0); //1196
		}
	    if(mastercard_input(30, &Stall_Warn_2) && Stall_Warn_2 == 0)
		{
	        sendMessageInt(KEY_COMMAND_WARNING_STALL_TEST, 0); //1196
		}
	}
}
