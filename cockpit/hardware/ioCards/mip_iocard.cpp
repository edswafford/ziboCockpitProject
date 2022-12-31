#include "mip_iocard.hpp"
#include "../sim737_hardware.hpp"
#include "../DeleteMeHotkey.h"
#include "../common/logger.hpp"
extern logger LOG;

namespace zcockpit::cockpit::hardware
{
	std::string MipIOCard::iocard_bus_addr;
	bool MipIOCard::running = false; 

	MipIOCard::MipIOCard(AircraftModel& ac_model, const std::string& device_bus_addr): IOCards(device_bus_addr, "mip"), aircraft_model(ac_model)
	{
		MipIOCard::iocard_bus_addr = device_bus_addr;
		initialize_switches();
	}
	std::unique_ptr<MipIOCard> MipIOCard::create_iocard(AircraftModel& ac_model, const std::string& bus_address)
	{
		MipIOCard::running = false;

		LOG() << "IOCards: creating MIP overhead";

		auto card = std::make_unique<MipIOCard>(ac_model, bus_address);
		if(card->is_open)
		{
			// Did we find the mip device and manage to open usb connection 
			LOG() << "IOCards MIP is Open";;

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
						LOG() << "IOCards: MIP failed to reading from usb";
					}
					else {
						card->receive_mastercard();
						MipIOCard::running = true;
						LOG() << "IOCards MIP is running";
						return card;
					}
				}
				else
				{
					LOG() << "IOCards: IOCards MIP failed init";
				}
			}
		}
		else
		{
			LOG() << "IOCards: Failed to open IOCards MIP.";
		}
		return nullptr;
	}
	void MipIOCard::processMIP()
	{
		static bool power_is_on = false;

		if(power_is_on && !aircraft_model.z738_ac_power_is_on())
		{
			power_is_on = false;
			// turn off relays
			updateRelays(0);
		}
		else if(!power_is_on && aircraft_model.z738_ac_power_is_on())
		{
			power_is_on = true;
			// turn on relays;
			updateRelays(1);
		}

		//
		//
		// All Switch values are initialized to zero. The function mastercard_input(pin_number, value) does two things
		// (1) set value to the switch 0 or 1
		// (2) returns 1 if something changed, and 0 if nothing changed, and -1 if something went wrong 
		//
		// So for example, On the first pass: if pin # 1 is equal to 1 and the initial value of captOutboardPFD == 0
		// Then captOutboardPFD will be set to one and the function returns 1  --> (return 1 ) && (captOutboardPFD of 1) == 1
		// Therefore, the if statement is true and the switch change command is sent to Xplane
		//
		// On second pass pin #1 is still 1 and therefore captOutboardPFD is again set to 1, but the return value is ZERO
		// because nothing changed.  Therefore, the if statement is false and nothing is sent to Xplane.
		//
		//  Capt Main Panel DUs rotary switch only one can change --> use if--else if
		if(mastercard_input(1, &captOutboardPFD) && captOutboardPFD)
		{
			aircraft_model.push_switch_change(iocard_mip_zcockpit_switches[0]);
		}
		else if(mastercard_input(2, &captNormal) && captNormal)
		{
			aircraft_model.push_switch_change(iocard_mip_zcockpit_switches[1]);
		}
		else if(mastercard_input(3, &captEngPrimary) && captEngPrimary)
		{
			aircraft_model.push_switch_change(iocard_mip_zcockpit_switches[2]);
		}
		else if(mastercard_input(4, &captPFD) && captPFD)
		{
			aircraft_model.push_switch_change(iocard_mip_zcockpit_switches[3]);
		}
		else if(captOutboardPFD == 0 && captNormal == 0 && captEngPrimary == 0 && captPFD == 0)
		{
			// MFD 
			aircraft_model.push_switch_change(iocard_mip_zcockpit_switches[4]);
		}


		//  Capt Lower DUs rotary switch only one can change --> use if--else if
		if(mastercard_input(5, &captEngPrimaryLowerDU) && captEngPrimaryLowerDU)
		{
			aircraft_model.push_switch_change(iocard_mip_zcockpit_switches[5]);
		}
		else if(mastercard_input(6, &captPFDLowerDU))
		{
			if(captPFDLowerDU)
			{
				aircraft_model.push_switch_change(iocard_mip_zcockpit_switches[6]);
			}
			else if(captPFDLowerDU == 0 && captEngPrimaryLowerDU == 0)
			{
				// ND
				aircraft_model.push_switch_change(iocard_mip_zcockpit_switches[7]);
			}
		}


		//  FO Main Panel DUs rotary switch only one can change --> use if--else if
		if(mastercard_input(13, &foOutboardPFD) && foOutboardPFD)
		{
			aircraft_model.push_switch_change(iocard_mip_zcockpit_switches[8]);
		}
		else if(mastercard_input(9, &foNormal) && foNormal)
		{
			aircraft_model.push_switch_change(iocard_mip_zcockpit_switches[9]);
		}
		else if(mastercard_input(10, &foEngPrimary) && foEngPrimary)
		{
			aircraft_model.push_switch_change(iocard_mip_zcockpit_switches[10]);
		}
		else if(mastercard_input(11, &foPFD) && foPFD)
		{
			aircraft_model.push_switch_change(iocard_mip_zcockpit_switches[11]);
		}
		else if(mastercard_input(12, &foMFD) && foMFD)
		{
			aircraft_model.push_switch_change(iocard_mip_zcockpit_switches[12]);
		}

		// during init use Capt switch
		auto delayIsOver = Sim737Hardware::has_run_for_one_second();

		//  FO Lower DUs rotary switch only one can change --> use if--else if
		if(mastercard_input(15, &foEngPrimaryLowerDU) && foEngPrimaryLowerDU)
		{
			if(delayIsOver)
			{
			aircraft_model.push_switch_change(iocard_mip_zcockpit_switches[13]);
			}
		}
		else if(mastercard_input(16, &foPFDLowerDU))
		{
			if(delayIsOver)
			{
			aircraft_model.push_switch_change(iocard_mip_zcockpit_switches[14]);
			}
		}
		else if(mastercard_input(14, &foNDLowerDU))
		{
			if(delayIsOver)
			{
			aircraft_model.push_switch_change(iocard_mip_zcockpit_switches[15]);
			}
		}


		// FO Clock
		if(mastercard_input(19, &foClock) && foClock)
		{
			aircraft_model.push_switch_change(iocard_mip_zcockpit_switches[16]);
		}

		// FO Master Fire Warning
		if(mastercard_input(20, &FoMasterFire) && FoMasterFire)
		{
			aircraft_model.push_switch_change(iocard_mip_zcockpit_switches[17]);
		}
		// FO Master Caution
		if(mastercard_input(18, &FoMasteCaution) && FoMasteCaution)
		{
			aircraft_model.push_switch_change(iocard_mip_zcockpit_switches[18]);
		}

		// FO 6-pack
		if(mastercard_input(21, &warningAnn) && warningAnn)
		{
			aircraft_model.push_switch_change(iocard_mip_zcockpit_switches[19]);
		}

		// N1 set
		if(mastercard_input(27, &eng2N2Set) && eng2N2Set)
		{
			aircraft_model.push_switch_change(iocard_mip_zcockpit_switches[20]);
		}
		if(mastercard_input(29, &eng1N2Set) && eng1N2Set)
		{
			aircraft_model.push_switch_change(iocard_mip_zcockpit_switches[21]);
		}
		if(mastercard_input(28, &N1SetAuto) && N1SetAuto)
		{
			aircraft_model.push_switch_change(iocard_mip_zcockpit_switches[22]);
		}
		if(mastercard_input(30, &N1SetBoth) && N1SetBoth)
		{
			aircraft_model.push_switch_change(iocard_mip_zcockpit_switches[23]);
		}


		// AFDS Warnings
		if(mastercard_input(23, &apWarning) && apWarning)
		{
			aircraft_model.push_switch_change(iocard_mip_zcockpit_switches[24]);
		}
		if(mastercard_input(24, &atWarning) && atWarning)
		{
			aircraft_model.push_switch_change(iocard_mip_zcockpit_switches[25]);
		}
		if(mastercard_input(25, &fmcWarning) && fmcWarning)
		{
			aircraft_model.push_switch_change(iocard_mip_zcockpit_switches[26]);
		}

		if(mastercard_input(26, &afdsTest2) && afdsTest2)
		{
			aircraft_model.push_switch_change(iocard_mip_zcockpit_switches[27]);
		}

		if(mastercard_input(22, &afdsTest1) && afdsTest1)
		{
			aircraft_model.push_switch_change(iocard_mip_zcockpit_switches[28]);
		}


		//GPWS
		if(mastercard_input(49, &gpws) && gpws)
		{
			aircraft_model.push_switch_change(iocard_mip_zcockpit_switches[29]);  // GPWS Test
		}

		if(mastercard_input(50, &flapsInhibit))
		{
			aircraft_model.push_switch_change(iocard_mip_zcockpit_switches[30]);
		}

		if(mastercard_input(51, &terrainInhibit))
		{
			aircraft_model.push_switch_change(iocard_mip_zcockpit_switches[31]);
		}


		if(mastercard_input(52, &gearInhibit))
		{
			aircraft_model.push_switch_change(iocard_mip_zcockpit_switches[32]);
		}


		//// Rudder Trim
		//if(mastercard_input(54, &rudderLeft) && rudderLeft)
		//{ 
		//	//    &iflyCmd = 600
		//	sendMessageInt(KEY_COMMAND_FLTCTRL_RUDDER_TRIM_DEC, 0);
		//}

		//if(mastercard_input(57, &rudderRight) && rudderRight)
		//{
		//	//    &iflyCmd = 601
		//	sendMessageInt(KEY_COMMAND_FLTCTRL_RUDDER_TRIM_INC, 0);
		//}

		//// Aileron Trim
		//if(mastercard_input(55, &aileronLeft) && aileronLeft)
		//{
		//	//   &iflyCmd = 602
		//	sendMessageInt(KEY_COMMAND_FLTCTRL_AILERON_TRIM_DEC, 0);
		//}

		//if(mastercard_input(56, &aileronRight) && aileronRight)
		//{
		//	//    &iflyCmd = 603
		//	sendMessageInt(KEY_COMMAND_FLTCTRL_AILERON_TRIM_INC, 0);
		//}

		// Encoders
		double value = 1.0;
		//if(mastercard_encoder(31, &value, 1.0, 1.0) > 0)
		//{
		//	LOG() << "value " << value;
		//	if(value > 0)
		//	{
		//		sendMessageInt(KEY_COMMAND_INSTRUMENT_FO_INBD_DU_BRIGHTNESS_DEC, 0);
		//	}
		//	else
		//	{
		//		sendMessageInt(KEY_COMMAND_INSTRUMENT_FO_INBD_DU_BRIGHTNESS_INC, 0);
		//	}
		//}

		//value = 1.0;
		//if(mastercard_encoder(33, &value, 1.0, 1.0) > 0)
		//{
		//	LOG() << "value " << value;
		//	if(value > 0)
		//	{
		//		sendMessageInt(KEY_COMMAND_INSTRUMENT_FO_OUTBD_DU_BRIGHTNESS_DEC, 0);
		//	}
		//	else
		//	{
		//		sendMessageInt(KEY_COMMAND_INSTRUMENT_FO_OUTBD_DU_BRIGHTNESS_INC, 0);
		//	}
		//}

		//value = 1.0;
		//if(mastercard_encoder(36, &value, 1.0, 1.0) > 0)
		//{
		//	LOG() << "value " << value;
		//	if(value > 0)
		//	{
		//		sendMessageInt(KEY_COMMAND_INSTRUMENT_CAPT_INBD_DU_BRIGHTNESS_INC, 0);
		//	}
		//	else
		//	{
		//		sendMessageInt(KEY_COMMAND_INSTRUMENT_CAPT_INBD_DU_BRIGHTNESS_DEC, 0);
		//	}
		//}
		//value = 1.0;
		//if(mastercard_encoder(42, &value, 1.0, 1.0) > 0)
		//{
		//	LOG() << "value " << value;
		//	if(value > 0)
		//	{
		//		sendMessageInt(KEY_COMMAND_INSTRUMENT_CAPT_OUTBD_DU_BRIGHTNESS_DEC, 0);
		//	}
		//	else
		//	{
		//		sendMessageInt(KEY_COMMAND_INSTRUMENT_CAPT_OUTBD_DU_BRIGHTNESS_INC, 0);
		//	}
		//}

		//value = 1.0;
		//if(mastercard_encoder(40, &value, 1.0, 1.0) > 0)
		//{
		//	LOG() << "value " << value;
		//	if(value > 0)
		//	{
		//		sendMessageInt(KEY_COMMAND_INSTRUMENT_LOWER_DU_BRIGHTNESS_DEC, 0);
		//	}
		//	else
		//	{
		//		sendMessageInt(KEY_COMMAND_INSTRUMENT_LOWER_DU_BRIGHTNESS_INC, 0);
		//	}
		//}
		//value = 1.0;
		//if(mastercard_encoder(38, &value, 1.0, 1.0) > 0)
		//{
		//	LOG() << "value " << value;
		//	if(value > 0)
		//	{
		//		sendMessageInt(KEY_COMMAND_INSTRUMENT_UPPER_DU_BRIGHTNESS_INC, 0);
		//	}
		//	else
		//	{
		//		sendMessageInt(KEY_COMMAND_INSTRUMENT_UPPER_DU_BRIGHTNESS_DEC, 0);
		//	}
		//}

		//// Speed Ref  (auto, V1, Vr, WT, Vref ...
		//value = 1.0;
		//if(mastercard_encoder(45, &value, 1.0, 1.0) > 0)
		//{
		//	LOG() << "value " << value;
		//	if(value > 0)
		//	{
		//		sendMessageInt(KEY_COMMAND_INSTRUMENT_LOWER_SPD_REF_INC, 0);
		//	}
		//	else
		//	{
		//		sendMessageInt(KEY_COMMAND_INSTRUMENT_LOWER_SPD_REF_DEC, 0);
		//	}
		//}

		//// N1 Set
		//value = 1.0;
		//if(mastercard_encoder(47, &value, 1.0, 1.0) > 0)
		//{
		//	LOG() << "value " << value;
		//	if(value > 0)
		//	{
		//		sendMessageInt(KEY_COMMAND_ENGAPU_N1_SET_INC, 0);
		//	}
		//	else
		//	{
		//		sendMessageInt(KEY_COMMAND_ENGAPU_N1_SET_DEC, 0);
		//	}
		//}
	}



	void MipIOCard::updateRelays(int state)
	{
		mastercard_output(49, &state);  // relay #1 Lights left MIP
		mastercard_output(50, &state);  // relay #2 CP flight power to center console
		mastercard_output(51, &state);  // relay #3 Lights Right MIP
		mastercard_output(52, &state);  // relay #4 Throttle servo motors
		mastercard_output(53, &state);  // relay #5 Throttle Lights
		//mastercard_output(54, &state);  // relay #6

		mastercard_output(55, &state);  // relay #7 MCP lights
	}

	
	constexpr int XPLANE_OUTBOARD_PFD = -1;
	constexpr int XPLANE_NORMAL = 0;
	constexpr int XPLANE_ENG_PRIMARY = 1;
	constexpr int XPLANE_PFD = 2;
	constexpr int XPLANE_MFD = 3;

	constexpr int XPLANE_LOWER_ENG_PRIMARY = -1;
	constexpr int XPLANE_LOWER_PFD = 0;
	constexpr int XPLANE_LOWER_ND = 1;

	constexpr int AP_DISENGAGE_LIGHT_TEST_1 = 1;
	constexpr int AP_DISENGAGE_LIGHT_TEST_2 = -1;

	void MipIOCard::initialize_switches()
	{
		iocard_mip_zcockpit_switches[0]  = ZcockpitSwitch(DataRefName::main_pnl_du_capt, common::SwitchType::rotary, 0, XPLANE_OUTBOARD_PFD);
		iocard_mip_zcockpit_switches[1]  = ZcockpitSwitch(DataRefName::main_pnl_du_capt, common::SwitchType::rotary, 0, XPLANE_NORMAL);
		iocard_mip_zcockpit_switches[2]  = ZcockpitSwitch(DataRefName::main_pnl_du_capt, common::SwitchType::rotary, 0, XPLANE_ENG_PRIMARY);
		iocard_mip_zcockpit_switches[3]  = ZcockpitSwitch(DataRefName::main_pnl_du_capt, common::SwitchType::rotary, 0, XPLANE_PFD);
		iocard_mip_zcockpit_switches[4]  = ZcockpitSwitch(DataRefName::main_pnl_du_capt, common::SwitchType::rotary, 0, XPLANE_MFD);

		iocard_mip_zcockpit_switches[5]  = ZcockpitSwitch(DataRefName::lower_du_capt, common::SwitchType::rotary, 0, XPLANE_LOWER_ENG_PRIMARY);
		iocard_mip_zcockpit_switches[6]  = ZcockpitSwitch(DataRefName::lower_du_capt, common::SwitchType::rotary, 0, XPLANE_LOWER_PFD);
		iocard_mip_zcockpit_switches[7]  = ZcockpitSwitch(DataRefName::lower_du_capt, common::SwitchType::rotary, 0, XPLANE_LOWER_ND);

		iocard_mip_zcockpit_switches[8]  = ZcockpitSwitch(DataRefName::main_pnl_du_fo, common::SwitchType::rotary, 0, XPLANE_OUTBOARD_PFD);
		iocard_mip_zcockpit_switches[9]  = ZcockpitSwitch(DataRefName::main_pnl_du_fo, common::SwitchType::rotary, 0, XPLANE_NORMAL);
		iocard_mip_zcockpit_switches[10] = ZcockpitSwitch(DataRefName::main_pnl_du_fo, common::SwitchType::rotary, 0, XPLANE_ENG_PRIMARY);
		iocard_mip_zcockpit_switches[11] = ZcockpitSwitch(DataRefName::main_pnl_du_fo, common::SwitchType::rotary, 0, XPLANE_PFD);
		iocard_mip_zcockpit_switches[12] = ZcockpitSwitch(DataRefName::main_pnl_du_fo, common::SwitchType::rotary, 0, XPLANE_MFD);

		iocard_mip_zcockpit_switches[13] = ZcockpitSwitch(DataRefName::lower_du_fo, common::SwitchType::rotary, 0,XPLANE_LOWER_ENG_PRIMARY);
		iocard_mip_zcockpit_switches[14] = ZcockpitSwitch(DataRefName::lower_du_fo, common::SwitchType::rotary, 0,XPLANE_LOWER_PFD);
		iocard_mip_zcockpit_switches[15] = ZcockpitSwitch(DataRefName::lower_du_fo, common::SwitchType::rotary, 0,XPLANE_LOWER_ND);


		// Pushbuttons
		iocard_mip_zcockpit_switches[16] = ZcockpitSwitch(DataRefName::chrono_fo_et_mode_pb, common::SwitchType::pushbutton, 0, 1);	// FO clock
		iocard_mip_zcockpit_switches[17] = ZcockpitSwitch(DataRefName::fire_bell_light2_pb, common::SwitchType::pushbutton, 0, 1);	// FO Fire Warning
		iocard_mip_zcockpit_switches[18] = ZcockpitSwitch(DataRefName::master_caution2_pb, common::SwitchType::pushbutton, 0, 1);	// FO Master Caution
		iocard_mip_zcockpit_switches[19] = ZcockpitSwitch(DataRefName::fo_six_pack_pb, common::SwitchType::pushbutton, 0, 1);		// FO Six pack

		//iocard_mip_zcockpit_switches[20]  = ZcockpitSwitch(DataRefName::, common::SwitchType::rotary, 0, );
		//iocard_mip_zcockpit_switches[21]  = ZcockpitSwitch(DataRefName::, common::SwitchType::rotary, 0, );
		//iocard_mip_zcockpit_switches[22]  = ZcockpitSwitch(DataRefName::, common::SwitchType::rotary, 0, );
		//iocard_mip_zcockpit_switches[23]  = ZcockpitSwitch(DataRefName::, common::SwitchType::rotary, 0, );

		iocard_mip_zcockpit_switches[24]  = ZcockpitSwitch(DataRefName::ap_light_fo, common::SwitchType::spring_loaded, 0, 1);	// FO AP warning
		iocard_mip_zcockpit_switches[25]  = ZcockpitSwitch(DataRefName::at_light_fo, common::SwitchType::spring_loaded, 0, 1);	// FO AT Warning 
		iocard_mip_zcockpit_switches[26]  = ZcockpitSwitch(DataRefName::fms_light_fo, common::SwitchType::spring_loaded, 0, 1);	// FO FMS warning

		iocard_mip_zcockpit_switches[27]  = ZcockpitSwitch(DataRefName::ap_discon_test2, common::SwitchType::spring_loaded, 0, AP_DISENGAGE_LIGHT_TEST_2);
		iocard_mip_zcockpit_switches[28]  = ZcockpitSwitch(DataRefName::ap_discon_test2, common::SwitchType::spring_loaded, 0, AP_DISENGAGE_LIGHT_TEST_1);


		iocard_mip_zcockpit_switches[29]  = ZcockpitSwitch(DataRefName::gpws_test, common::SwitchType::pushbutton, 0, 1); // GPWS Test
		iocard_mip_zcockpit_switches[30]  = ZcockpitSwitch(DataRefName::gpws_flap, common::SwitchType::toggle, 0, 1);
		iocard_mip_zcockpit_switches[31]  = ZcockpitSwitch(DataRefName::gpws_terr, common::SwitchType::toggle, 0, 1);
		iocard_mip_zcockpit_switches[32]  = ZcockpitSwitch(DataRefName::gpws_gear, common::SwitchType::toggle, 0, 1);



		//iocard_mip_zcockpit_switches[10]  = ZcockpitSwitch(DataRefName::, common::SwitchType::rotary, 0, );
		//iocard_mip_zcockpit_switches[11]  = ZcockpitSwitch(DataRefName::, common::SwitchType::rotary, 0, );
		//iocard_mip_zcockpit_switches[12]  = ZcockpitSwitch(DataRefName::, common::SwitchType::rotary, 0, );
		//iocard_mip_zcockpit_switches[13]  = ZcockpitSwitch(DataRefName::, common::SwitchType::rotary, 0, );
		//iocard_mip_zcockpit_switches[14]  = ZcockpitSwitch(DataRefName::, common::SwitchType::rotary, 0, );
		//iocard_mip_zcockpit_switches[15]  = ZcockpitSwitch(DataRefName::, common::SwitchType::rotary, 0, );
		//iocard_mip_zcockpit_switches[16]  = ZcockpitSwitch(DataRefName::, common::SwitchType::rotary, 0, );
		//iocard_mip_zcockpit_switches[17]  = ZcockpitSwitch(DataRefName::, common::SwitchType::rotary, 0, );
		//iocard_mip_zcockpit_switches[18]  = ZcockpitSwitch(DataRefName::, common::SwitchType::rotary, 0, );
		//iocard_mip_zcockpit_switches[19]  = ZcockpitSwitch(DataRefName::, common::SwitchType::rotary, 0, );



//		iocard_mip_zcockpit_switches[0]  = ZcockpitSwitch();										// no connection	0


	};



}
