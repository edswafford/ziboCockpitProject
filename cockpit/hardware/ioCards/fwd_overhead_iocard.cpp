#include <array>
#include "fwd_overhead_iocard.hpp"
#include "../common/logger.hpp"
#include "../DeleteMeHotkey.h"

extern logger LOG;

namespace zcockpit::cockpit::hardware
{
	std::string ForwardOverheadIOCard::iocard_bus_addr;
	bool ForwardOverheadIOCard::running = false; 

	ForwardOverheadIOCard::ForwardOverheadIOCard(AircraftModel& ac_model, const std::string deviceBusAddr)
		: IOCards(deviceBusAddr, "fwdOverhead"), aircraft_model(ac_model)
	{
		ForwardOverheadIOCard::iocard_bus_addr = deviceBusAddr;

		compMode_altn = 0;
		compMode_auto = 0;
		compMode_man = 0;
		pos_steady = 0;
		pos_strobe = 0;
		outflowOpen = 0;
		outflowClosed = 0;
		dc_stbyPwr = 0;
		dc_batBus = 0;
		dc_bat = 0;
		dc_auxBat = 0;
		dc_tr1 = 0;
		dc_tr2 = 0;
		dc_tr3 = 0;
		dc_test = 0;
		disconnect_1 = 0;
		elec_maint = 0;
		ac_stbyPwr = 0;
		ac_gndPwr = 0;
		ac_gen1 = 0;
		ac_gen2 = 0;
		ac_apuGen = 0;
		ac_test = 0;
		ac_inv = 0;
		start_1_gnd = 0;
		start_1_cont = 0;
		start_1_flt = 0;
		start_2_gnd = 0;
		start_2_cont = 0;
		start_2_flt = 0;
		wing_aitiIce = 0;
		eng1_antiIce = 0;
		eng2_antiIce = 0;
		lRecirc_fan = 0;
		rRecirc_fan = 0;
		leftPack_off = 0;
		left_pack_high = 0;
		isolationClose = 0;
		isolationOpen = 0;
		right_pack_off = 0;
		right_pack_hi = 0;
		eng_1_bleed = 0;
		apu_bleed = 0;
		eng_2_bleed = 0;
		ovht_test = 0;
		trip_reset = 0;
		fltctrl_a_stby = 0;
		fltctrl_a_on = 0;
		fltctrl_b_stby = 0;
		fltctrl_b_on = 0;
		flap_Arm = 0;
		altFlapUp = 0;
		altFlapDn = 0;
		spoiler_a = 0;
		spoiler_b = 0;
		wiper_l_pk = 0;
		wiper_l_int = 0;
		wiper_l_low = 0;
		wiper_r_pk = 0;
		wiper_r_int = 0;
		wiper_r_low = 0;
		vhfNavBoth1 = 0;
		vhfNavBoth2 = 0;
		irsBothLeft = 0;
		irsBothRight = 0;
		displaySrc1 = 0;
		displaySrcAuto = 0;

		mag_eng1_start = 0;
		mag_eng2_start = 0;

		eng1_start_gnd = masterCard_input_state{27, 0, false};
		eng1_start_cont = masterCard_input_state{28, 0, false};
		eng1_start_flt = masterCard_input_state{29, 0, false};

		eng2_start_gnd = masterCard_input_state{30, 0, false};
		eng2_start_cont = masterCard_input_state{31, 0, false};
		eng2_start_flt = masterCard_input_state{32, 0, false};

		engine_start_switches[0] = &eng1_start_gnd;
		engine_start_switches[1] = &eng1_start_cont;
		engine_start_switches[2] = &eng1_start_flt;
		engine_start_switches[3] = &eng2_start_gnd;
		engine_start_switches[4] = &eng2_start_cont;
		engine_start_switches[5] = &eng2_start_flt;

		eng1_start_debounce = ENG_START_DEBOUNCE_COUNT;
		eng2_start_debounce = ENG_START_DEBOUNCE_COUNT;

	}

	std::unique_ptr<ForwardOverheadIOCard> ForwardOverheadIOCard::create_iocard(AircraftModel& ac_model, const std::string& bus_address)
	{
		ForwardOverheadIOCard::running = false;

		LOG() << "IOCards: creating Forward Overhead";
		auto card = std::make_unique<ForwardOverheadIOCard>(ac_model, bus_address);
		if(card->is_open)
		{
			// Did we find the fwd overhead device and manage to open usb connection 
			LOG() << "IOCards Forward Overhead is Open";;

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
						LOG() << "IOCards Forward Overhead  failed to reading from usb";
					}
					else{
						card->receive_mastercard();
						ForwardOverheadIOCard::running = true;
						LOG() << "fwd oveehead is running";
						return card;
					}
				}
				else
				{
					LOG() << "IOCards Forward Overhead  failed init";
				}
			}
		}
		else
		{
			LOG() << "Failed to open IOCards Forward Overhead .";
		}
		return nullptr;
	}


	void ForwardOverheadIOCard::update_displays()
	{
		static int selector = 0;
		// Displays
		// 

		if(selector == 0)
		{
			update_electrical_display();
		}
		else if(selector == 2)
		{
			update_flight_alt_display();
		}
		else if(selector == 4)
		{
			update_landing_alt_display();
		}
		selector = (selector + 1) % 6;
	}

	void ForwardOverheadIOCard::update_electrical_display()
	{
		// AC Volts
		int const AC_VOLTS_1 = 21;
		int const AC_VOLTS_10 = 22;
		int const AC_VOLTS_100 = 23;
		//mastercard_send_display(Ifly737::shareMemSDK->AC_VOLTS_1_Status, AC_VOLTS_1);
		//mastercard_send_display(Ifly737::shareMemSDK->AC_VOLTS_10_Status, AC_VOLTS_10);
		//mastercard_send_display(Ifly737::shareMemSDK->AC_VOLTS_100_Status, AC_VOLTS_100);


		//// AC Freq
		//int const AC_FREQ_1 = 16;
		//int const AC_FREQ_10 = 17;
		//int const AC_FREQ_100 = 18;
		//mastercard_send_display(Ifly737::shareMemSDK->CPS_FREQ_1_Status, AC_FREQ_1);
		//mastercard_send_display(Ifly737::shareMemSDK->CPS_FREQ_10_Status, AC_FREQ_10);
		//mastercard_send_display(Ifly737::shareMemSDK->CPS_FREQ_100_Status, AC_FREQ_100);

		//// DC AMPS
		//int const DC_AMPS_1 = 19;
		//int const DC_AMPS_10 = 20;

		//mastercard_send_display(Ifly737::shareMemSDK->DC_AMPS_1_Status, DC_AMPS_1);
		//mastercard_send_display(Ifly737::shareMemSDK->DC_AMPS_10_Status, DC_AMPS_10);

		//// AC AMPS
		//int const AC_AMPS_1 = 24;
		//int const AC_AMPS_10 = 25;
		//mastercard_send_display(Ifly737::shareMemSDK->AC_AMPS_1_Status, AC_AMPS_1);
		//mastercard_send_display(Ifly737::shareMemSDK->AC_AMPS_10_Status, AC_AMPS_10);

		//// DC Volts
		//int const DC_VOLTS_1 = 26;
		//int const DC_VOLTS_10 = 27;
		//mastercard_send_display(Ifly737::shareMemSDK->DC_VOLTS_1_Status, DC_VOLTS_1);
		//mastercard_send_display(Ifly737::shareMemSDK->DC_VOLTS_10_Status, DC_VOLTS_10);
	}

	void ForwardOverheadIOCard::update_landing_alt_display()
	{
		// Landing Altitude
		int const LAND_ALT_1 = 5;
		int const LAND_ALT_10 = 6;
		int const LAND_ALT_100 = 7;
		int const LAND_ALT_1000 = 8;
		int const LAND_ALT_10_000 = 9;
		//mastercard_send_display(ifly737->landing_altitude[0], LAND_ALT_1);
		//mastercard_send_display(ifly737->landing_altitude[1], LAND_ALT_10);
		//mastercard_send_display(ifly737->landing_altitude[2], LAND_ALT_100);
		//mastercard_send_display(ifly737->landing_altitude[3], LAND_ALT_1000);
		//unsigned char land_alt;// = ifly737->landing_altitude[4];
		//if(land_alt == 11)
		//{
		//	land_alt = 0xf8;
		//}
	//	mastercard_send_display(land_alt, LAND_ALT_10_000);
	}


	void ForwardOverheadIOCard::update_flight_alt_display()
	{
		// Flight Altitude
		int const FLT_ALT_1 = 0;
		int const FLT_ALT_10 = 1;
		int const FLT_ALT_100 = 2;
		int const FLT_ALT_1000 = 3;
		int const FLT_ALT_10_000 = 4;

		//mastercard_send_display(ifly737->flight_altitude[0], FLT_ALT_1);
		//mastercard_send_display(ifly737->flight_altitude[1], FLT_ALT_10);
		//mastercard_send_display(ifly737->flight_altitude[2], FLT_ALT_100);
		//mastercard_send_display(ifly737->flight_altitude[3], FLT_ALT_1000);
		//unsigned char land_alt;// = ifly737->flight_altitude[4];
		//if(land_alt == 11)
		//{
		//	land_alt = 0xf8;
		//}
		//mastercard_send_display(land_alt, FLT_ALT_10_000);
	}

	bool ForwardOverheadIOCard::is_display_blank(std::array<unsigned char, NUMBER_OF_ALTITUDE_DIGITS> digits)
	{
		for(auto i = 0; i < NUMBER_OF_ALTITUDE_DIGITS; i++)
		{
			if(digits[i] != 10)
			{
				return false;
			}
		}
		return true;
	}

	long ForwardOverheadIOCard::convert_digits_to_long(std::array<unsigned char, NUMBER_OF_ALTITUDE_DIGITS> digits)
	{
		long val = 0;
		int scaling = 1;
		for(int i = 0; i < NUMBER_OF_ALTITUDE_DIGITS; i++)
		{
			// LOG() << "Flt Alt [" << i << "] " << digits[i];
			if(digits[i] >= 0 && digits[i] < 10)
			{
				val += digits[i] * scaling;
			}
			scaling *= 10;
		}

		// is last digit negative
		if(digits[NUMBER_OF_ALTITUDE_DIGITS - 1] == 11)
		{
			val = -val;
		}
		return val;
	}

	void ForwardOverheadIOCard::processEncoders()
	{
		// Encoders

		double value = 0.0;
		//if(mastercard_encoder(0, &value, 1.0, 1.0) > 0)
		//{
		//	if(value > 0)
		//	{
		//		sendMessageInt(KEY_COMMAND_AIRSYSTEM_FLT_ALT_INC, 0);
		//	}
		//	else
		//	{
		//		sendMessageInt(KEY_COMMAND_AIRSYSTEM_FLT_ALT_DEC, 0);
		//	}
		//}

		//value = 0.0;
		//if(mastercard_encoder(2, &value, 1.0, 1.0) > 0)
		//{
		//	if(value > 0)
		//	{
		//		sendMessageInt(KEY_COMMAND_AIRSYSTEM_LDG_ALT_INC, 0);
		//	}
		//	else
		//	{
		//		sendMessageInt(KEY_COMMAND_AIRSYSTEM_LDG_ALT_DEC, 0);
		//	}
		//}

		//if(Ifly737::FltAltDisplayChanged())
		//{
		//	update_flight_alt_display();
		//	Ifly737::FltAltDisplayChanged(false);
		//}
		//if(Ifly737::LandAltDisplayChanged())
		//{
		//	update_landing_alt_display();
		//	Ifly737::LandAltDisplayChanged(false);
		//}
	}

	void ForwardOverheadIOCard::initialize_switches()
	{
		iocard_fwd_overhead_zcockpit_switches[0]  = ZcockpitSwitch(DataRefName::starter1_pos, common::SwitchType::rotary, GND);
		iocard_fwd_overhead_zcockpit_switches[1]  = ZcockpitSwitch(DataRefName::starter1_pos, common::SwitchType::rotary, OFF);
		iocard_fwd_overhead_zcockpit_switches[2]  = ZcockpitSwitch(DataRefName::starter1_pos, common::SwitchType::rotary, CONT);
		iocard_fwd_overhead_zcockpit_switches[3]  = ZcockpitSwitch(DataRefName::starter1_pos, common::SwitchType::rotary, FLT);

		iocard_fwd_overhead_zcockpit_switches[4]  = ZcockpitSwitch(DataRefName::starter2_pos, common::SwitchType::rotary, GND);
		iocard_fwd_overhead_zcockpit_switches[5]  = ZcockpitSwitch(DataRefName::starter2_pos, common::SwitchType::rotary, OFF);
		iocard_fwd_overhead_zcockpit_switches[6]  = ZcockpitSwitch(DataRefName::starter2_pos, common::SwitchType::rotary, CONT);
		iocard_fwd_overhead_zcockpit_switches[7]  = ZcockpitSwitch(DataRefName::starter2_pos, common::SwitchType::rotary, FLT);


	}

	void ForwardOverheadIOCard::fastProcessOvrHead()
	{
		const unsigned char pwr = aircraft_model.z738_ac_power_is_on()? 1 : 0;


		update_displays();

		//
		// Process Switches
		//
		LOG() << "Calling Forward";
//		process_master_card_inputs(constants::fwd_ovrHead_to_keycmd, constants::fwd_ovrhead_keycmd_size);

		LOG() << "Calling Engine Start";
		process_master_card_inputs(engine_start_switches, NUMBER_OF_ENGINE_SWITCH_STATES);
		LOG() << "Done forward";

		processOvrHead();

		//
		// Odd ball switches
		//
		//       | 30 | 31 | 32
		// GND   |  1 |  1 |  1
		// OFF   |  0 |  0 |  0
		// CONT  |  0 |  1 |  0
		// FLT   |  0 |  X |  1
		//

		//
		// Engine 1
		//
		if(eng1_start_gnd.has_changed || eng1_start_cont.has_changed || eng1_start_flt.has_changed)
		{
			eng1_start_debounce = ENG_START_DEBOUNCE_COUNT;
			if(eng1_start_gnd.value == 1 && eng1_start_cont.value == 1 && eng1_start_flt.value == 1)
			{
				// GND
				mag_eng1_start = pwr != 0 ? 1 : 0;
				engine1_state = GND;
			}
			else if(eng1_start_gnd.value == 0 && eng1_start_cont.value == 0 && eng1_start_flt.value == 0)
			{
				// OFF
				mag_eng1_start = 0;
				engine1_state = OFF;
			}
			else if(eng1_start_gnd.value == 0 && eng1_start_cont.value == 1 && eng1_start_flt.value == 0)
			{
				// CONT
				mag_eng1_start = 0;
				engine1_state = CONT;
			}
			else if(eng1_start_gnd.value == 0 && eng1_start_flt.value == 1)
			{
				// FLT
				mag_eng1_start = 0;
				engine1_state = FLT;
			}
			else
			{
				LOG() << "Invalid engine start";
			}

			// clear states
			eng1_start_gnd.has_changed = false;
			eng1_start_cont.has_changed = false;
			eng1_start_flt.has_changed = false;
		}
		else
		{
			if(eng1_start_debounce <= 0)
			{

				auto starter_1 = static_cast<int*>(aircraft_model.get_z_cockpit_switch_data(DataRefName::starter1_pos));

				if(starter_1 != nullptr && *starter_1 != engine1_state)
				{
					switch(engine1_state)
					{
						case GND:
							aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[0]);
							break;

						case OFF:
							aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[1]);
							break;

						case CONT:
							aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[2]);
							break;

						case FLT:
							aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[3]);
							break;
						default:
							break;
					}
				}
			}
			else
			{
				eng1_start_debounce -= 1;
			}
		}

		//
		// Odd ball switches
		//
		//       | 30 | 31 | 32
		// GND   |  1 |  0 |  1
		// OFF   |  0 |  0 |  0
		// CONT  |  0 |  1 |  0
		// FLT   |  0 |  0 |  1
		//
		// Engine 2
		//
		if(eng2_start_gnd.has_changed || eng2_start_cont.has_changed || eng2_start_flt.has_changed)
		{
			eng2_start_debounce = ENG_START_DEBOUNCE_COUNT;
			if(eng2_start_gnd.value == 1 && eng2_start_flt.value == 1)
			{
				// GND
				mag_eng2_start = pwr != 0 ? 1 : 0;
				engine2_state = GND;
			}
			else if(eng2_start_gnd.value == 0 && eng2_start_cont.value == 0 && eng2_start_flt.value == 0)
			{
				// OFF
				mag_eng2_start = 0;
				engine2_state = OFF;
			}
			else if(eng2_start_gnd.value == 0 && eng2_start_cont.value == 1 && eng2_start_flt.value == 0)
			{
				// CONT
				mag_eng2_start = 0;
				engine2_state = CONT;
			}
			else if(eng2_start_gnd.value == 0 && eng2_start_flt.value == 1)
			{
				// FLT
				mag_eng2_start = 0;
				engine2_state = FLT;
			}
			else
			{
				LOG() << "Invalid engine start";
			}

			// clear states
			eng2_start_gnd.has_changed = false;
			eng2_start_cont.has_changed = false;
			eng2_start_flt.has_changed = false;
		}
		else
		{
			if(eng2_start_debounce <= 0)
			{
	//			if(Ifly737::shareMemSDK->Engine_2_Start_Switch_Status != engine2_state)
				{
					//switch(engine2_state)
					//{
					//	case GND:
					//	sendMessageInt(KEY_COMMAND_ENGAPU_ENG_2_START_POS1, 0);
					//		break;

					//	case OFF:
					//	sendMessageInt(KEY_COMMAND_ENGAPU_ENG_2_START_POS2, 0);
					//		break;

					//	case CONT:
					//	sendMessageInt(KEY_COMMAND_ENGAPU_ENG_2_START_POS3, 0);
					//		break;

					//	case FLT:
					//	sendMessageInt(KEY_COMMAND_ENGAPU_ENG_2_START_POS4, 0);
					//		break;
					//}
				}
			}
			else
			{
				eng2_start_debounce -= 1;
			}
		}


		// Elec Gen 1 Disconnect
		const int DEBOUNCE_MAX_COUNT = 3;
		static int gen1Disconnect = 1;
		static int gen1Counter = 0;
		static KEY_COMMAND lastGen1KeyCmd = KEY_COMMAND_ELECTRICAL_GENERATOR_DRIVE_1_DISCONNECT_UP;


	//	if(Ifly737::LeftEngRunning())
		{
			if(mastercard_input(17, &disconnect_1))
			{
				gen1Counter = 0;
			}
			if(disconnect_1 >= 0)
			{
				if(gen1Disconnect != disconnect_1)
				{
					gen1Disconnect = disconnect_1;
					gen1Counter = 0;
				}
				gen1Counter++;

				if(gen1Counter >= DEBOUNCE_MAX_COUNT)
				{
					gen1Counter = DEBOUNCE_MAX_COUNT;

					if(disconnect_1 == 1)
					{
						if(lastGen1KeyCmd != KEY_COMMAND_ELECTRICAL_GENERATOR_DRIVE_1_DISCONNECT_UP)
						{
							lastGen1KeyCmd = KEY_COMMAND_ELECTRICAL_GENERATOR_DRIVE_1_DISCONNECT_UP;
			//				sendMessageInt(KEY_COMMAND_ELECTRICAL_GENERATOR_DRIVE_1_DISCONNECT_UP, 0);
						}
					}
					else
					{
						if(lastGen1KeyCmd != KEY_COMMAND_ELECTRICAL_GENERATOR_DRIVE_1_DISCONNECT_DOWN)
						{
							lastGen1KeyCmd = KEY_COMMAND_ELECTRICAL_GENERATOR_DRIVE_1_DISCONNECT_DOWN;
		//					sendMessageInt(KEY_COMMAND_ELECTRICAL_GENERATOR_DRIVE_1_DISCONNECT_DOWN, 0);
						}
					}
				}
			}
		}

		// Elec Gen 2 Disconnect
		////////////////////////////////////////////////////////////////////
		//
		//       N O T I C E
		//
		/////////////////////////////////////////////////////////////////////
		// ran out of available inputs
		// moved to rear overhead board which has spares
		//
	}


	void ForwardOverheadIOCard::processOvrHead()
	{
		//
		// The outflow valve is spring loaded and must to held open
		// for this to work we need to keep sending the open/close command for ifly
		// unil the switch is released
		// 135 KEY_COMMAND_AIRSYSTEM_OUTFLOW_VALVE_CLOSE
		// 136 KEY_COMMAND_AIRSYSTEM_OUTFLOW_VALVE_MIDDLE
		// 137 KEY_COMMAND_AIRSYSTEM_OUTFLOW_VALVE_OPEN

		// pressurization manual valve
		if(outflowOpen == 1)
		{
			//	sendMessageInt(KEY_COMMAND_AIRSYSTEM_OUTFLOW_VALVE_OPEN, 0);
		}
		if(mastercard_input(51, &outflowOpen))
		{
			if(outflowOpen == 0)
			{
				//			sendMessageInt(KEY_COMMAND_AIRSYSTEM_OUTFLOW_VALVE_MIDDLE, 0);
			}
			LOG() << "outflow = " << outflowOpen;
		}

		// pressurization manual valve
		if(outflowClosed == 1)
		{
			//	sendMessageInt(KEY_COMMAND_AIRSYSTEM_OUTFLOW_VALVE_CLOSE, 0);
		}
		if(mastercard_input(52, &outflowClosed))
		{
			if(outflowOpen == 0)
			{
				//			sendMessageInt(KEY_COMMAND_AIRSYSTEM_OUTFLOW_VALVE_MIDDLE, 0);
			}
		}


		// KEY_COMMAND_FLTCTRL_ALTERNATE_FLAPS_MASTER       584
		//  KEY_COMMAND_FLTCTRL_ALTERNATE_FLAPS_MASTER_OFF  585
		//  KEY_COMMAND_FLTCTRL_ALTERNATE_FLAPS_MASTER_ARM  586
		// Flaps Master ARM switch position
		if(mastercard_input(53, &flap_Arm))
		{
			//if(flap_Arm == 1)
			//{
			//	sendMessageInt(KEY_COMMAND_FLTCTRL_ALTERNATE_FLAPS_MASTER, 1);
			//	sendMessageInt(KEY_COMMAND_FLTCTRL_ALTERNATE_FLAPS_MASTER_ARM, 1);
			//	//flapDelay = DELAY 1 100
			//}
			//else
			//{
			//	sendMessageInt(KEY_COMMAND_FLTCTRL_ALTERNATE_FLAPS_MASTER_OFF, 0);
			//}
		}

		// flight control B ON switch position
		if(mastercard_input(54, &fltctrl_b_on))
		{
			//if(fltctrl_b_on == 1)
			//{
			//	sendMessageInt(KEY_COMMAND_FLTCTRL_FLIGHT_CONTROL_B_ON, 0);
			//}
			//else
			//{
			//	if(fltctrl_b_stby != 1)
			//	{
			//		sendMessageInt(KEY_COMMAND_FLTCTRL_FLIGHT_CONTROL_B_OFF, 0);
			//	}
			//}
		}


		// KEY_COMMAND_FLTCTRL_FLIGHT_CONTROL_B_STBYRUD 572
		// KEY_COMMAND_FLTCTRL_FLIGHT_CONTROL_B_OFF     573
		//	KEY_COMMAND_FLTCTRL_FLIGHT_CONTROL_B_ON      574
		// flight control B STBY switch position
		if(mastercard_input(55, &fltctrl_b_stby))
		{
			//if(fltctrl_b_stby == 1)
			//{
			//	sendMessageInt(KEY_COMMAND_FLTCTRL_FLIGHT_CONTROL_B_STBYRUD, 0);
			//}
			//else
			//{
			//	if(fltctrl_b_on != 1)
			//	{
			//		sendMessageInt(KEY_COMMAND_FLTCTRL_FLIGHT_CONTROL_B_OFF, 0);
			//	}
			//}
		}


		// KEY_COMMAND_FLTCTRL_FLIGHT_CONTROL_A_STBYRUD 567
		// KEY_COMMAND_FLTCTRL_FLIGHT_CONTROL_A_OFF     568
		//	KEY_COMMAND_FLTCTRL_FLIGHT_CONTROL_A_ON      569
		// flight control A STBY switch position
		//if(mastercard_input(56, &fltctrl_a_stby))
		//{
		//	if(fltctrl_a_stby == 1)
		//	{
		//		sendMessageInt(KEY_COMMAND_FLTCTRL_FLIGHT_CONTROL_A_STBYRUD, 0);
		//	}
		//	else
		//	{
		//		if(fltctrl_a_on != 1)
		//		{
		//			sendMessageInt(KEY_COMMAND_FLTCTRL_FLIGHT_CONTROL_A_OFF, 0);
		//		}
		//	}
		//}

		//// flight control A ON switch position
		//if(mastercard_input(57, &fltctrl_a_on))
		//{
		//	if(fltctrl_a_on == 1)
		//	{
		//		sendMessageInt(KEY_COMMAND_FLTCTRL_FLIGHT_CONTROL_A_ON, 0);
		//	}
		//	else
		//	{
		//		if(fltctrl_a_stby != 1)
		//		{
		//			sendMessageInt(KEY_COMMAND_FLTCTRL_FLIGHT_CONTROL_A_OFF, 0);
		//		}
		//	}
		//}
		//  KEY_COMMAND_FLTCTRL_ALTERNATE_FLAPS_POS1  589
		//  KEY_COMMAND_FLTCTRL_ALTERNATE_FLAPS_POS2  590
		//  KEY_COMMAND_FLTCTRL_ALTERNATE_FLAPS_POS3  591
		// Alternate Flaps UP switch position

		// Alternate Flaps DOWN switch position
		//if(mastercard_input(58, &altFlapDn))
		//{
		//	if(altFlapDn == 1)
		//	{
		//		sendMessageInt(KEY_COMMAND_FLTCTRL_ALTERNATE_FLAPS_POS3, 0);
		//	}
		//	else
		//	{
		//		if(altFlapUp != 1)
		//		{
		//			sendMessageInt(KEY_COMMAND_FLTCTRL_ALTERNATE_FLAPS_POS2, 0);
		//		}
		//	}
		//}
		//if(mastercard_input(59, &altFlapUp))
		//{
		//	if(altFlapUp == 1)
		//	{
		//		sendMessageInt(KEY_COMMAND_FLTCTRL_ALTERNATE_FLAPS_POS1, 0);
		//	}
		//	else
		//	{
		//		if(altFlapDn != 1)
		//		{
		//			sendMessageInt(KEY_COMMAND_FLTCTRL_ALTERNATE_FLAPS_POS2, 0);
		//		}
		//	}
		//}


		// KEY_COMMAND_ANTIICE_RIGHT_WINDSHIELD_WIPER_PARK 180
		// KEY_COMMAND_ANTIICE_RIGHT_WINDSHIELD_WIPER_INT  181
		// KEY_COMMAND_ANTIICE_RIGHT_WINDSHIELD_WIPER_LOW  182
		// KEY_COMMAND_ANTIICE_RIGHT_WINDSHIELD_WIPER_HIGH 183
		// Right Wiper Park switch position
		// 
		//  NOTE  ******************************
		//
		// ONLY WORKS IN VC
		//
		//if(mastercard_input(60, &wiper_r_pk))
		//{
		//	if(wiper_r_pk == 1)
		//	{
		//		sendMessageInt(KEY_COMMAND_ANTIICE_RIGHT_WINDSHIELD_WIPER_PARK, 0);
		//	}
		//}
		////// Right Wiper Intermittent switch position
		//if(mastercard_input(61, &wiper_r_int))
		//{
		//	if(wiper_r_int == 1)
		//	{
		//		sendMessageInt(KEY_COMMAND_ANTIICE_RIGHT_WINDSHIELD_WIPER_INT, 0);
		//	}
		//}
		////// Right Wiper Low switch position
		//if(mastercard_input(62, &wiper_r_low))
		//{
		//	if(wiper_r_low == 1 && wiper_r_int == 0 && wiper_r_pk == 0)
		//	{
		//		sendMessageInt(KEY_COMMAND_ANTIICE_RIGHT_WINDSHIELD_WIPER_LOW, 0);
		//	}
		//	else
		//	{
		//		if(wiper_r_low == 0 && wiper_r_int == 0 && wiper_r_pk == 0)
		//		{
		//			sendMessageInt(KEY_COMMAND_ANTIICE_RIGHT_WINDSHIELD_WIPER_HIGH, 0);
		//		}
		//	}
		//}


		//	KEY_COMMAND_INSTRUMENT_DISPLAYS_SOURCE_1    713
		// KEY_COMMAND_INSTRUMENT_DISPLAYS_SOURCE_AUTO 714
		// KEY_COMMAND_INSTRUMENT_DISPLAYS_SOURCE_2    715
		// Display Source 1 switch position
		//if(mastercard_input(63, &displaySrc1))
		//{
		//	if(displaySrc1 == 1)
		//	{
		//		sendMessageInt(KEY_COMMAND_INSTRUMENT_DISPLAYS_SOURCE_1, 0);
		//	}
		//	else
		//	{
		//		if(displaySrcAuto != 1)
		//		{
		//			sendMessageInt(KEY_COMMAND_INSTRUMENT_DISPLAYS_SOURCE_2, 0);
		//		}
		//	}
		//}

		//// Display Source Auto switch position
		//if(mastercard_input(64, &displaySrcAuto))
		//{
		//	if(displaySrcAuto == 1)
		//	{
		//		sendMessageInt(KEY_COMMAND_INSTRUMENT_DISPLAYS_SOURCE_AUTO, 0);
		//	}
		//	else
		//	{
		//		if(displaySrc1 != 1)
		//		{
		//			sendMessageInt(KEY_COMMAND_INSTRUMENT_DISPLAYS_SOURCE_2, 0);
		//		}
		//	}
		//}
		//// IRS Both on Right switch position
		//if(mastercard_input(65, &irsBothRight))
		//{
		//	if(irsBothRight == 1)
		//	{
		//		sendMessageInt(KEY_COMMAND_FMS_IRS_TFR_R, 0);
		//	}
		//	else
		//	{
		//		if(irsBothLeft != 1)
		//		{
		//			sendMessageInt(KEY_COMMAND_FMS_IRS_TFR_NORMAL, 0);
		//		}
		//	}
		//}

		//// IRS Both on Left switch position
		//if(mastercard_input(66, &irsBothLeft))
		//{
		//	if(irsBothLeft == 1)
		//	{
		//		sendMessageInt(KEY_COMMAND_FMS_IRS_TFR_L, 0);
		//	}
		//	else
		//	{
		//		if(irsBothRight != 1)
		//		{
		//			sendMessageInt(KEY_COMMAND_FMS_IRS_TFR_NORMAL, 0);
		//		}
		//	}
		//}

		//// KEY_COMMAND_FMS_VHF_NAV_1       949
		//// KEY_COMMAND_FMS_VHF_NAV_NORMAL  950
		//// KEY_COMMAND_FMS_VHF_NAV_2       951
		//// VHF NAV Both on 1 switch position
		//if(mastercard_input(67, &vhfNavBoth1))
		//{
		//	if(vhfNavBoth1 == 1)
		//	{
		//		sendMessageInt(KEY_COMMAND_FMS_VHF_NAV_1, 0);
		//	}
		//	else
		//	{
		//		if(vhfNavBoth2 != 1)
		//		{
		//			sendMessageInt(KEY_COMMAND_FMS_VHF_NAV_NORMAL, 0);
		//		}
		//	}
		//}
		//// VHF NAV Both on 2 switch position
		//if(mastercard_input(68, &vhfNavBoth2))
		//{
		//	if(vhfNavBoth2 == 1)
		//	{
		//		sendMessageInt(KEY_COMMAND_FMS_VHF_NAV_2, 0);
		//	}
		//	else
		//	{
		//		if(vhfNavBoth1 != 1)
		//		{
		//			sendMessageInt(KEY_COMMAND_FMS_VHF_NAV_NORMAL, 0);
		//		}
		//	}
		//}

		//// KEY_COMMAND_ANTIICE_LEFT_WINDSHIELD_WIPER_PARK  174
		//// KEY_COMMAND_ANTIICE_LEFT_WINDSHIELD_WIPER_INT   175
		//// KEY_COMMAND_ANTIICE_LEFT_WINDSHIELD_WIPER_LOW   176
		//// KEY_COMMAND_ANTIICE_LEFT_WINDSHIELD_WIPER_HIGH  177
		//// Left Wiper Park switch position
		//// 
		////  NOTE  ******************************
		////
		//// ONLY WORKS IN VC
		////
		//if(mastercard_input(69, &wiper_l_pk))
		//{
		//	if(wiper_l_pk == 1)
		//	{
		//		sendMessageInt(KEY_COMMAND_ANTIICE_LEFT_WINDSHIELD_WIPER_PARK, 0);
		//	}
		//}
		////// Left Wiper Intermittent switch position
		//if(mastercard_input(70, &wiper_l_int))
		//{
		//	if(wiper_l_int == 1)
		//	{
		//		sendMessageInt(KEY_COMMAND_ANTIICE_LEFT_WINDSHIELD_WIPER_INT, 0);
		//	}
		//}
		////// Left Wiper Low switch position
		//if(mastercard_input(71, &wiper_l_low))
		//{
		//	if(wiper_l_low == 1)
		//	{
		//		sendMessageInt(KEY_COMMAND_ANTIICE_LEFT_WINDSHIELD_WIPER_LOW, 0);
		//	}
		//	else
		//	{
		//		if(wiper_l_int == 0 && wiper_l_int == 0 && wiper_l_pk == 0)
		//		{
		//			sendMessageInt(KEY_COMMAND_ANTIICE_LEFT_WINDSHIELD_WIPER_HIGH, 0);
		//		}
		//	}
		//}
	}
}