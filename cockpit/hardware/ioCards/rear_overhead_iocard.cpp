#include "rear_overhead_iocard.hpp"
#include "../../aircraft_model.hpp"
#include "../common/logger.hpp"


extern logger LOG;

namespace zcockpit::cockpit::hardware
{
	std::string RearOverheadIOCard::iocard_bus_addr;
	bool RearOverheadIOCard::running = false;

	RearOverheadIOCard::RearOverheadIOCard(AircraftModel& ac_model, std::string deviceBusAddr)
		: IOCards(deviceBusAddr, "rearOverhead"), aircraft_model(ac_model)
	{
		RearOverheadIOCard::iocard_bus_addr = deviceBusAddr;

		initialize_switches();
	}

	std::unique_ptr<RearOverheadIOCard> RearOverheadIOCard::create_iocard(AircraftModel& ac_model, const std::string& bus_address) 
	{
		RearOverheadIOCard::running = false;

		LOG() << "IOCards: creating Rear Overhead";

		auto card = std::make_unique<RearOverheadIOCard>(ac_model, bus_address);

		if(card->is_open)
		{
			// Did we find the rear overhead device and manage to open usb connection 
			LOG() << "IOCards Rear Overhead is Open";;

			if(card->init_for_async()) {

				// Axes are not used --> set to 0
				constexpr unsigned char number_of_axes = 0;
				if(card->initialize_mastercard(number_of_axes))
				{
					card->clear_buffers();

					// submit first asynchronous read call
					if(libusb_submit_transfer(card->readTransfer) < 0)
					{
						card->event_thread_failed = true;
						LOG() << "IOCards Rear Overhead failed to reading from usb";
					}
					else {
						card->receive_mastercard();
						RearOverheadIOCard::running = true;
						LOG() << "IOCards Rear Overhead is running";
						return card;
					}
				}
				else
				{
					LOG() << "IOCards Rear Overhead failed init";
				}
			}
		}
		else
		{
			LOG() << "Failed to open IOCards Rear Overhead.";
		}
		return nullptr;
	}


	void RearOverheadIOCard::initialize_switches()
	{
		constexpr int GENERATOR_DISCONNECT_UP = 1;
		constexpr int GENERATOR_DISCONNECT_DOWN = 0;
		constexpr int ANTIICE_ENG_2_OFF = 0;
		constexpr int ANTIICE_ENG_2_ON = 1;
		constexpr int IRS_MODE_OFF = 0;
		constexpr int IRS_MODE_ALIGN = 1;
		constexpr int IRS_MODE_NAV = 2;
		constexpr int IRS_MODE_ATT = 3;
		constexpr int IRS_SYS_DSPL_L = 0;
		constexpr int IRS_SYS_DSPL_R = 1;

		constexpr int IRS_DSPL_TEST = 0;
		constexpr int IRS_DSPL_TRACK_GRD_SPEED = 1;
		constexpr int IRS_DSPL_POSITION = 2;
		constexpr int IRS_DSPL_WIND = 3;
		constexpr int IRS_DSPL_HEADING = 4;
		constexpr int PASS_OXYGEN_ON = 1;
		constexpr int PASS_OXYGEN_NORMAL = 0;
		constexpr int MACH_AIRSPD_WARNING_PRESSED = 1;
		constexpr int MACH_AIRSPD_WARNING_RELEASED = 0;
		constexpr int STALL_WARNING_PRESSED = 1;
		constexpr int STALL_WARNING_RELEASED = 1;

		iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::drive_disconnect2_pos_generator_disconnect_up]   = ZcockpitSwitch(DataRefName::drive_disconnect2_pos, common::SwitchType::toggle, GENERATOR_DISCONNECT_UP );
		iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::drive_disconnect2_pos_generator_disconnect_down]   = ZcockpitSwitch(DataRefName::drive_disconnect2_pos, common::SwitchType::toggle, GENERATOR_DISCONNECT_DOWN);


		//engine 2 anti ice switch: pin 23
		iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::eng2_heat_pos_off] =ZcockpitSwitch(DataRefName::eng2_heat_pos,      common::SwitchType::toggle, ANTIICE_ENG_2_OFF);
		iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::eng2_heat_pos_on] =ZcockpitSwitch(DataRefName::eng2_heat_pos,      common::SwitchType::toggle, ANTIICE_ENG_2_ON);

		iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::irs_l_off] =ZcockpitSwitch(DataRefName::irs_left,      common::SwitchType::rotary_2_commands, IRS_MODE_OFF);
		iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::irs_l_align] =ZcockpitSwitch(DataRefName::irs_left,      common::SwitchType::rotary_2_commands, IRS_MODE_ALIGN);
		iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::irs_l_nav] =ZcockpitSwitch(DataRefName::irs_left,      common::SwitchType::rotary_2_commands, IRS_MODE_NAV);
		iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::irs_l_att] =ZcockpitSwitch(DataRefName::irs_left,      common::SwitchType::rotary_2_commands, IRS_MODE_ATT);

		iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::irs_r_off] =ZcockpitSwitch(DataRefName::irs_right,      common::SwitchType::rotary_2_commands, IRS_MODE_OFF);
		iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::irs_r_align] =ZcockpitSwitch(DataRefName::irs_right,      common::SwitchType::rotary_2_commands, IRS_MODE_ALIGN);
		iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::irs_r_nav] =ZcockpitSwitch(DataRefName::irs_right,      common::SwitchType::rotary_2_commands, IRS_MODE_NAV);
		iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::irs_r_att] =ZcockpitSwitch(DataRefName::irs_right,      common::SwitchType::rotary_2_commands, IRS_MODE_ATT);

		iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::irs_dspl_l] =ZcockpitSwitch(DataRefName::irs_sys_dspl,      common::SwitchType::toggle, IRS_SYS_DSPL_L);
		iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::irs_dspl_r] =ZcockpitSwitch(DataRefName::irs_sys_dspl,      common::SwitchType::toggle, IRS_SYS_DSPL_R);

		iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::irs_dspl_sel_test] =ZcockpitSwitch(DataRefName::irs_dspl_sel,      common::SwitchType::rotary_2_commands, IRS_DSPL_TEST);
		iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::irs_dspl_sel_tk_gs] =ZcockpitSwitch(DataRefName::irs_dspl_sel,      common::SwitchType::rotary_2_commands, IRS_DSPL_TRACK_GRD_SPEED);
		iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::irs_dspl_sel_pos] =ZcockpitSwitch(DataRefName::irs_dspl_sel,      common::SwitchType::rotary_2_commands, IRS_DSPL_POSITION);
		iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::irs_dspl_sel_wind] =ZcockpitSwitch(DataRefName::irs_dspl_sel,      common::SwitchType::rotary_2_commands, IRS_DSPL_WIND);
		iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::irs_dspl_sel_heading] =ZcockpitSwitch(DataRefName::irs_dspl_sel,      common::SwitchType::rotary_2_commands, IRS_DSPL_HEADING);

		iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::pass_oxygen_on] =ZcockpitSwitch(DataRefName::pax_oxy_pos, common::SwitchType::rotary_multi_commands, PASS_OXYGEN_ON);
		iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::pass_oxygen_normal] =ZcockpitSwitch(DataRefName::pax_oxy_pos, common::SwitchType::rotary_multi_commands, PASS_OXYGEN_NORMAL);

		iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::mach_airspd_warning_1_pressed] =ZcockpitSwitch(DataRefName::mach_warn1_pos, common::SwitchType::spring_loaded,MACH_AIRSPD_WARNING_PRESSED);
		iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::mach_airspd_warning_1_released] =ZcockpitSwitch(DataRefName::mach_warn1_pos, common::SwitchType::spring_loaded,MACH_AIRSPD_WARNING_RELEASED);
		iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::mach_airspd_warning_2_pressed] =ZcockpitSwitch(DataRefName::mach_warn2_pos, common::SwitchType::spring_loaded,MACH_AIRSPD_WARNING_PRESSED);
		iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::mach_airspd_warning_2_released] =ZcockpitSwitch(DataRefName::mach_warn2_pos, common::SwitchType::spring_loaded,MACH_AIRSPD_WARNING_RELEASED);

		iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::stall_warning_1_pressed] =ZcockpitSwitch(DataRefName::stall_test1, common::SwitchType::spring_loaded,STALL_WARNING_PRESSED);
		iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::stall_warning_1_released] =ZcockpitSwitch(DataRefName::stall_test1, common::SwitchType::spring_loaded,STALL_WARNING_RELEASED);
		iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::stall_warning_2_pressed] =ZcockpitSwitch(DataRefName::stall_test2, common::SwitchType::spring_loaded,STALL_WARNING_PRESSED);
		iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::stall_warning_2_released] =ZcockpitSwitch(DataRefName::stall_test2, common::SwitchType::spring_loaded,STALL_WARNING_RELEASED);

	}

		bool RightEngRunning(){return true;}

	void RearOverheadIOCard::process_rear_over_head()
	{
		constexpr int GENERATOR_DISCONNECT_UP = 1;
		constexpr int GENERATOR_DISCONNECT_DOWN = 0;

		constexpr int DEBOUNCE_MAX_COUNT = 3;
		static int gen2Disconnect = 1;
		static int gen2Counter = 0;
		static int lastGenCmd = -1;

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
					if(disconnect_2 == 0)
					{
						if(lastGenCmd != GENERATOR_DISCONNECT_UP)
						{
							lastGenCmd = GENERATOR_DISCONNECT_UP;
							aircraft_model.push_switch_change(iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::drive_disconnect2_pos_generator_disconnect_up]);
						}
					}
					else
					{
						if(lastGenCmd != GENERATOR_DISCONNECT_DOWN)
						{
							lastGenCmd = GENERATOR_DISCONNECT_DOWN;
							aircraft_model.push_switch_change(iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::drive_disconnect2_pos_generator_disconnect_down]);
						}
					}
				}
			}
		}

		// IRU Sys Display
	   if(mastercard_input(0, &l_IRU_SysDspl) && l_IRU_SysDspl)
	   {
			aircraft_model.push_switch_change(iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::irs_dspl_l]);
	   }

	   if(mastercard_input(1, &r_IRU_SysDspl) && r_IRU_SysDspl)
	   {
			aircraft_model.push_switch_change(iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::irs_dspl_r]);
	   }

	   // IRU Display Select
	   if(mastercard_input(3, &IRU_DsplSelTst) && IRU_DsplSelTst)
	   {
			aircraft_model.push_switch_change(iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::irs_dspl_sel_test]);
	   }

	   if(mastercard_input(6, &IRU_DsplSelTK) && IRU_DsplSelTK)
	   {
			aircraft_model.push_switch_change(iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::irs_dspl_sel_tk_gs]);
	   }

	   if(mastercard_input(7, &IRU_DsplSelPos) && IRU_DsplSelPos)
	   {
		    aircraft_model.push_switch_change(iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::irs_dspl_sel_pos]);
	   }

	   if(mastercard_input(5, &IRU_DsplSelWnd) && IRU_DsplSelWnd)
	   {
		    aircraft_model.push_switch_change(iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::irs_dspl_sel_wind]);
	   }

	   if(mastercard_input(2, &IRU_DsplSelHdg) && IRU_DsplSelHdg)
	   {
		   	aircraft_model.push_switch_change(iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::irs_dspl_sel_heading]);
	   }

		// LEFT IRU MODE Switch
		   if(mastercard_input(10, &l_IRU_Off) && l_IRU_Off)
		   {
				aircraft_model.push_switch_change(iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::irs_l_off]);
		   }

		   if(mastercard_input(12, &l_IRU_Align) && l_IRU_Align)
		   {
				aircraft_model.push_switch_change(iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::irs_l_align]);
		   }

		   if(mastercard_input(9, &l_IRU_Nav) && l_IRU_Nav)
		   {
				aircraft_model.push_switch_change(iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::irs_l_nav]);
		   }

		   if(mastercard_input(11, &l_IRU_Att) && l_IRU_Att)
		   {
				aircraft_model.push_switch_change(iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::irs_l_att]);
		   }


		   // RIGHT IRU MODE Switch
		   if(mastercard_input(16, &r_IRU_Off) && r_IRU_Off)
		   {
				aircraft_model.push_switch_change(iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::irs_r_off]);
		   }

		   if(mastercard_input(15, &r_IRU_Align) && r_IRU_Align)
		   {
				aircraft_model.push_switch_change(iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::irs_r_align]);
		   }

		   if(mastercard_input(13, &r_IRU_Nav) && r_IRU_Nav)
		   {
				aircraft_model.push_switch_change(iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::irs_r_nav]);
		   }

		   if(mastercard_input(14, &r_IRU_Att) && r_IRU_Att)
		   {
				aircraft_model.push_switch_change(iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::irs_r_att]);
		   }


		// -------------------------------------------------------------------------------------------
		//
		//  NOT SUPPORTED
		//
		// -------------------------------------------------------------------------------------------
		
////		constexpr int EEC_1_ON  = 
//// 		constexpr int EEC_1_OFF =
////		constexpr int EEC_2_ON  =
//// 		constexpr int EEC_2_OFF =
////
////		   // EEC On/ATN
////	    if(mastercard_input(17, &L_EEC_ON))
////	    {
////	        if(L_EEC_ON == 1)
////	        {
////		   		aircraft_model.push_switch_change(iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::eec_1_on]);
////	        }
////	        else
////	        {
////		   		aircraft_model.push_switch_change(iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::eec_1__off]);
////	        }
////	    }
////
////	    if(mastercard_input(8, &R_EEC_ON))
////	    {
////	        if(R_EEC_ON == 1)
////	        {
////		   		aircraft_model.push_switch_change(iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::eec_2_on]);
////	        }
////	        else
////	        {
////		   		aircraft_model.push_switch_change(iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::eec_2_off]);
////	        }
////	    }




		// Engine 2 AntiIce
		//engine 1 anti ice switch: pin 23
		if(mastercard_input(23, &eng2_heat))
		{
			if(eng2_heat == 1)
			{
				aircraft_model.push_switch_change(iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::eng2_heat_pos_off]);
			}
			else {
				aircraft_model.push_switch_change(iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::eng2_heat_pos_on]); 
			}
		}

	   if (mastercard_input(27, &Pass_Oxygen) )
	   {
	       if ( Pass_Oxygen)
	       {
		   	aircraft_model.push_switch_change(iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::pass_oxygen_on]);
	       }
	       else
	       {
		   	aircraft_model.push_switch_change(iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::pass_oxygen_normal]);
	       }
	   }


		// Airspeed Warning Test
	    if(mastercard_input(33, &mach_airspd_warn_1) )
		{
			if( mach_airspd_warn_1 == 0){
				aircraft_model.push_switch_change(iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::mach_airspd_warning_1_pressed]);
			}
			else {
				aircraft_model.push_switch_change(iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::mach_airspd_warning_1_released]);
			}

		}
	    if(mastercard_input(31, &Airspd_Warn_2) )
		{
			if(Airspd_Warn_2) {
				aircraft_model.push_switch_change(iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::mach_airspd_warning_2_pressed]);
			}
			else {
				aircraft_model.push_switch_change(iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::mach_airspd_warning_2_released]);
			}
		}


		// Stall Warning
	    if(mastercard_input(28, &stall_warn_1) )
		{
			if(stall_warn_1 == 0){
				aircraft_model.push_switch_change(iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::stall_warning_1_pressed]);
			}
			else {
				aircraft_model.push_switch_change(iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::stall_warning_1_released]);
			}
		}
	    if(mastercard_input(30, &Stall_Warn_2) )
		{
			if(Stall_Warn_2 == 0){
				aircraft_model.push_switch_change(iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::stall_warning_2_pressed]);
			}
			else {
				aircraft_model.push_switch_change(iocard_rear_overhead_zcockpit_switches[RearSwitchPosition::stall_warning_2_released]);
			}
		}
	}
}
