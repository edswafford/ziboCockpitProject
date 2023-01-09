#include <array>
#include "fwd_overhead_iocard.hpp"
#include "../common/logger.hpp"
#include "../DeleteMeHotkey.h"

extern logger LOG;

namespace zcockpit::cockpit::hardware
{
	constexpr int GND = 0;
	constexpr int OFF = 1;
	constexpr int CONT = 2;
	constexpr int FLT = 3;
	constexpr int GENERATOR_DISCONNECT_UP = 1;
	constexpr int GENERATOR_DISCONNECT_DOWN = 0;
	constexpr int OUTFLOW_VALVE_OPEN = -1;
	constexpr int OUTFLOW_VALVE_MIDDLE = 0;
	constexpr int OUTFLOW_VALVE_CLOSE = 1;
	constexpr int ALTERNATE_FLAPS_ARM = 1;
	constexpr int ALTERNATE_FLAPS_OFF = 0;
	constexpr int FLIGHT_CONTROL_B_OFF = 0;
	constexpr int FLIGHT_CONTROL_B_ON = 1;
	constexpr int FLIGHT_CONTROL_B_STBYRUD = -1;
	constexpr int FLIGHT_CONTROL_A_OFF = 0;
	constexpr int FLIGHT_CONTROL_A_ON = 1;
	constexpr int FLIGHT_CONTROL_A_STBYRUD = -1;

	constexpr int ALTERNATE_FLAPS_CTRL_DN = 1;
	constexpr int ALTERNATE_FLAPS_CTRL_OFF = 0;
	constexpr int ALTERNATE_FLAPS_CTRL_UP = -1;

	constexpr int INSTRUMENT_DISPLAYS_SOURCE_1 = -1; 
	constexpr int INSTRUMENT_DISPLAYS_SOURCE_2 = 1;
	constexpr int INSTRUMENT_DISPLAYS_SOURCE_AUTO = 0;
	constexpr int FMS_IRS_TFR_R = 1;
	constexpr int FMS_IRS_TFR_NORMAL = 0;
	constexpr int FMS_IRS_TFR_L = -1;
	constexpr int FMS_VHF_NAV_L = -1;
	constexpr int FMS_VHF_NAV_NORMAL = 0;
	constexpr int FMS_VHF_NAV_R = -1;
;
	std::string ForwardOverheadIOCard::iocard_bus_addr;
	bool ForwardOverheadIOCard::running = false; 

	ForwardOverheadIOCard::ForwardOverheadIOCard(AircraftModel& ac_model, const std::string deviceBusAddr)
		: IOCards(deviceBusAddr, "fwdOverhead"), aircraft_model(ac_model)
	{
		ForwardOverheadIOCard::iocard_bus_addr = deviceBusAddr;

		initialize_switches();
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
		int constexpr AC_VOLTS_1 = 21;
		int constexpr AC_VOLTS_10 = 22;
		int constexpr AC_VOLTS_100 = 23;
		const int ac_volts = aircraft_model.z737InData.ac_volt_value;
		const uint8_t ac_volt_hundreds = ac_volts/100;
		const int ac_volt_remaining = (ac_volts - (ac_volt_hundreds * 100));
		const uint8_t ac_volt_tens = ac_volt_remaining/10;
		const uint8_t ac_volt_ones = (ac_volt_remaining - (ac_volt_tens * 10));
		mastercard_send_display(ac_volt_ones, AC_VOLTS_1);
		mastercard_send_display(ac_volt_tens, AC_VOLTS_10);
		mastercard_send_display(ac_volt_hundreds, AC_VOLTS_100);


		// AC Freq
		int constexpr AC_FREQ_1 = 16;
		int constexpr AC_FREQ_10 = 17;
		int constexpr AC_FREQ_100 = 18;
		const int ac_freq = aircraft_model.z737InData.ac_freq_value;
		const uint8_t ac_freq_hundreds = ac_freq/100;
		const int ac_freq_remaining = (ac_freq - (ac_freq_hundreds * 100));
		const uint8_t ac_freq_tens = ac_freq_remaining/10;
		const uint8_t ac_freq_ones = (ac_freq_remaining - (ac_freq_tens * 10));
		mastercard_send_display(ac_freq_ones, AC_FREQ_1);
		mastercard_send_display(ac_freq_tens, AC_FREQ_10);
		mastercard_send_display(ac_freq_hundreds, AC_FREQ_100);

		// DC AMPS
		int constexpr DC_AMPS_1 = 19;
		int constexpr DC_AMPS_10 = 20;
		const int dc_amps = aircraft_model.z737InData.dc_amp_value;
		const uint8_t dc_amp_tens = dc_amps/10;
		const uint8_t dc_amp_ones = dc_amps - (dc_amp_tens * 10);
		mastercard_send_display(dc_amp_ones, DC_AMPS_1);
		mastercard_send_display(dc_amp_tens, DC_AMPS_10);

		// AC AMPS
		int constexpr AC_AMPS_1 = 24;
		int constexpr AC_AMPS_10 = 25;
		const int ac_amps = aircraft_model.z737InData.ac_amp_value;
		const uint8_t ac_amp_tens = ac_amps/10;
		mastercard_send_display(ac_amp_tens, AC_AMPS_10);

		// DC Volts
		int constexpr DC_VOLTS_1 = 26;
		int constexpr DC_VOLTS_10 = 27;
		const int dc_volts = aircraft_model.z737InData.dc_volt_value;
		const uint8_t dc_volt_tens = dc_volts/10;
		const uint8_t dc_volt_ones = dc_volts - (dc_volt_tens * 10);
		mastercard_send_display(dc_volt_ones, DC_VOLTS_1);
		mastercard_send_display(dc_volt_tens, DC_VOLTS_10);
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
		iocard_fwd_overhead_zcockpit_switches[0]  = ZcockpitSwitch(DataRefName::starter1_pos, common::SwitchType::rotary_multi_commands, GND);
		iocard_fwd_overhead_zcockpit_switches[1]  = ZcockpitSwitch(DataRefName::starter1_pos, common::SwitchType::rotary_multi_commands, OFF);
		iocard_fwd_overhead_zcockpit_switches[2]  = ZcockpitSwitch(DataRefName::starter1_pos, common::SwitchType::rotary_multi_commands, CONT);
		iocard_fwd_overhead_zcockpit_switches[3]  = ZcockpitSwitch(DataRefName::starter1_pos, common::SwitchType::rotary_multi_commands, FLT);

		iocard_fwd_overhead_zcockpit_switches[4]  = ZcockpitSwitch(DataRefName::starter2_pos, common::SwitchType::rotary_multi_commands, GND);
		iocard_fwd_overhead_zcockpit_switches[5]  = ZcockpitSwitch(DataRefName::starter2_pos, common::SwitchType::rotary_multi_commands, OFF);
		iocard_fwd_overhead_zcockpit_switches[6]  = ZcockpitSwitch(DataRefName::starter2_pos, common::SwitchType::rotary_multi_commands, CONT);
		iocard_fwd_overhead_zcockpit_switches[7]  = ZcockpitSwitch(DataRefName::starter2_pos, common::SwitchType::rotary_multi_commands, FLT);

		iocard_fwd_overhead_zcockpit_switches[8]  = ZcockpitSwitch(DataRefName::drive_disconnect1_pos, common::SwitchType::toggle, GENERATOR_DISCONNECT_UP );
		iocard_fwd_overhead_zcockpit_switches[9]  = ZcockpitSwitch(DataRefName::drive_disconnect1_pos, common::SwitchType::toggle, GENERATOR_DISCONNECT_DOWN);

		iocard_fwd_overhead_zcockpit_switches[10]  = ZcockpitSwitch(DataRefName::air_valve_manual, common::SwitchType::spring_loaded, OUTFLOW_VALVE_OPEN);
		iocard_fwd_overhead_zcockpit_switches[11]  = ZcockpitSwitch(DataRefName::air_valve_manual, common::SwitchType::spring_loaded, OUTFLOW_VALVE_MIDDLE);
		iocard_fwd_overhead_zcockpit_switches[12]  = ZcockpitSwitch(DataRefName::air_valve_manual, common::SwitchType::spring_loaded, OUTFLOW_VALVE_CLOSE);

		iocard_fwd_overhead_zcockpit_switches[12]  = ZcockpitSwitch(DataRefName::alt_flaps_pos, common::SwitchType::toggle, ALTERNATE_FLAPS_ARM );
		iocard_fwd_overhead_zcockpit_switches[13]  = ZcockpitSwitch(DataRefName::alt_flaps_pos, common::SwitchType::toggle, ALTERNATE_FLAPS_OFF);

		iocard_fwd_overhead_zcockpit_switches[14]  = ZcockpitSwitch(DataRefName::flt_ctr_B_pos, common::SwitchType::spring_loaded, FLIGHT_CONTROL_B_ON);
		iocard_fwd_overhead_zcockpit_switches[15]  = ZcockpitSwitch(DataRefName::flt_ctr_B_pos, common::SwitchType::spring_loaded, FLIGHT_CONTROL_B_OFF );
		iocard_fwd_overhead_zcockpit_switches[16]  = ZcockpitSwitch(DataRefName::flt_ctr_B_pos, common::SwitchType::spring_loaded, FLIGHT_CONTROL_B_STBYRUD );

		iocard_fwd_overhead_zcockpit_switches[17]  = ZcockpitSwitch(DataRefName::flt_ctr_A_pos, common::SwitchType::toggle, FLIGHT_CONTROL_A_ON);
		iocard_fwd_overhead_zcockpit_switches[18]  = ZcockpitSwitch(DataRefName::flt_ctr_A_pos, common::SwitchType::toggle, FLIGHT_CONTROL_A_OFF );
		iocard_fwd_overhead_zcockpit_switches[19]  = ZcockpitSwitch(DataRefName::flt_ctr_A_pos, common::SwitchType::toggle, FLIGHT_CONTROL_A_STBYRUD );

		iocard_fwd_overhead_zcockpit_switches[20]  = ZcockpitSwitch(DataRefName::alt_flaps_ctrl, common::SwitchType::toggle, ALTERNATE_FLAPS_CTRL_DN);
		iocard_fwd_overhead_zcockpit_switches[21]  = ZcockpitSwitch(DataRefName::alt_flaps_ctrl, common::SwitchType::toggle, ALTERNATE_FLAPS_CTRL_OFF );
		iocard_fwd_overhead_zcockpit_switches[22]  = ZcockpitSwitch(DataRefName::alt_flaps_ctrl, common::SwitchType::toggle, ALTERNATE_FLAPS_CTRL_UP);

		iocard_fwd_overhead_zcockpit_switches[23]  = ZcockpitSwitch(DataRefName::dspl_source, common::SwitchType::toggle, INSTRUMENT_DISPLAYS_SOURCE_1 );
		iocard_fwd_overhead_zcockpit_switches[24]  = ZcockpitSwitch(DataRefName::dspl_source, common::SwitchType::toggle, INSTRUMENT_DISPLAYS_SOURCE_2);
		iocard_fwd_overhead_zcockpit_switches[25]  = ZcockpitSwitch(DataRefName::dspl_source, common::SwitchType::toggle, INSTRUMENT_DISPLAYS_SOURCE_AUTO );
																				 
		iocard_fwd_overhead_zcockpit_switches[26]  = ZcockpitSwitch(DataRefName::irs_source, common::SwitchType::toggle, FMS_IRS_TFR_R);
		iocard_fwd_overhead_zcockpit_switches[27]  = ZcockpitSwitch(DataRefName::irs_source, common::SwitchType::toggle, FMS_IRS_TFR_NORMAL );
		iocard_fwd_overhead_zcockpit_switches[28]  = ZcockpitSwitch(DataRefName::irs_source, common::SwitchType::toggle, FMS_IRS_TFR_L);

		iocard_fwd_overhead_zcockpit_switches[29]  = ZcockpitSwitch(DataRefName::vhf_nav_source, common::SwitchType::toggle, FMS_VHF_NAV_L );
		iocard_fwd_overhead_zcockpit_switches[30]  = ZcockpitSwitch(DataRefName::vhf_nav_source, common::SwitchType::toggle, FMS_VHF_NAV_NORMAL);
		iocard_fwd_overhead_zcockpit_switches[31]  = ZcockpitSwitch(DataRefName::vhf_nav_source, common::SwitchType::toggle, FMS_VHF_NAV_R );

	}

	void ForwardOverheadIOCard::fastProcessOvrHead()
	{
		const unsigned char pwr = aircraft_model.z738_ac_power_is_on()? 1 : 0;


		update_displays();

		//
		// Process Switches
		//
		//LOG() << "Calling Forward";
//		process_master_card_inputs(constants::fwd_ovrHead_to_keycmd, constants::fwd_ovrhead_keycmd_size);

		//LOG() << "Calling Engine Start";
		process_master_card_inputs(engine_start_switches, NUMBER_OF_ENGINE_SWITCH_STATES);
		//LOG() << "Done forward";

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
				LOG() << "ERROR: Invalid engine start 1";
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

				const auto starter_1 = static_cast<int*>(aircraft_model.get_z_cockpit_switch_data(DataRefName::starter1_pos));

				if(starter_1 != nullptr && *starter_1 != engine1_state && old_engine1_state != engine1_state)
				{
					old_engine1_state = engine1_state;
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
							LOG() << "ERROR: invalid starter 1 state " << engine1_state;
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
				LOG() << "ERROR: Invalid engine start 2";
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
				const auto starter_2 = static_cast<int*>(aircraft_model.get_z_cockpit_switch_data(DataRefName::starter2_pos));

				if(starter_2 != nullptr && *starter_2 != engine2_state && old_engine2_state != engine2_state)
				{
					old_engine2_state = engine2_state;
					switch(engine2_state)
					{
						case GND:
							aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[4]);
							break;

						case OFF:
							aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[5]);
							break;

						case CONT:
							aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[6]);
							break;

						case FLT:
							aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[7]);
							break;
						default:
							LOG() << "ERROR: invalid starter 2 state " << engine2_state;
							break;
					}
				}
			}
			else
			{
				eng2_start_debounce -= 1;
			}
		}


		// Elec Gen 1 Disconnect
		constexpr int DEBOUNCE_MAX_COUNT = 3;
		static int gen1Disconnect = 1;
		static int gen1Counter = 0;



		static int lastGenCmd = -1;


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

					if(disconnect_1 == 0)
					{
						if(lastGenCmd != GENERATOR_DISCONNECT_UP)
						{
							lastGenCmd = GENERATOR_DISCONNECT_UP;
							aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[8]);
						}
					}
					else
					{
						if(lastGenCmd != GENERATOR_DISCONNECT_DOWN)
						{
							lastGenCmd = GENERATOR_DISCONNECT_DOWN;
							aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[9]);
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
		// pressurization manual valve
		if(outflowOpen == 1)
		{
			aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[10]);  // OUTFLOW_VALVE_OPEN
		}
		if(mastercard_input(51, &outflowOpen))
		{
			if(outflowOpen == 0)
			{
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[11]);  // OUTFLOW_VALVE_MIDDLE
			}
			LOG() << "outflow = " << outflowOpen;
		}

		// pressurization manual valve
		if(outflowClosed == 1)
		{
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[12]);  // OUTFLOW_VALVE_CLOSE
		}
		if(mastercard_input(52, &outflowClosed))
		{
			if(outflowOpen == 0)
			{
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[11]);  // OUTFLOW_VALVE_MIDDLE
			}
		}


		// Flaps Master ARM switch position
		if(mastercard_input(53, &flap_Arm))
		{
			if(flap_Arm == 1)
			{
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[12]);  // ALTERNATE_FLAPS_ARM
			}
			else
			{
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[13]);  // ALTERNATE_FLAPS_OFF
			}
		}

		// flight control B ON switch position
		if(mastercard_input(54, &fltctrl_b_on))
		{
			if(fltctrl_b_on == 1)
			{
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[14]);  // FLIGHT_CONTROL_B_ON
			}
			else
			{
				if(fltctrl_b_stby != 1)
				{
					aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[15]);  // FLIGHT_CONTROL_B_OFF
				}
			}
		}

		// flight control B STBY switch position
		if(mastercard_input(55, &fltctrl_b_stby))
		{
			if(fltctrl_b_stby == 1)
			{
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[16]);  // FLIGHT_CONTROL_B_STBYRUD
		}
			else
			{
				if(fltctrl_b_on != 1)
				{
					aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[15]);  // FLIGHT_CONTROL_B_OFF
				}
			}
		}



		// flight control A STBY switch position
		if(mastercard_input(56, &fltctrl_a_stby))
		{
			if(fltctrl_a_stby == 1)
			{
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[19]);  // FLIGHT_CONTROL_A_STBYRUD
		}
			else
			{
				if(fltctrl_a_on != 1)
				{
					aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[18]);  // FLIGHT_CONTROL_A_OFF
				}
			}
		}

		// flight control A ON switch position
		if(mastercard_input(57, &fltctrl_a_on))
		{
			if(fltctrl_a_on == 1)
			{
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[17]);  // FLIGHT_CONTROL_A_ON
			}
			else
			{
				if(fltctrl_a_stby != 1)
				{
					aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[18]);  // FLIGHT_CONTROL_A_OFF
				}
			}
		}


		// Alternate Flaps UP switch position
		// Alternate Flaps DOWN switch position
		if(mastercard_input(58, &altFlapDn))
		{
			if(altFlapDn == 1)
			{
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[20]);  // ALTERNATE_FLAPS_CTRL_DN
			}
			else
			{
				if(altFlapUp != 1)
				{
					aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[21]);  // ALTERNATE_FLAPS_CTRL_OFF
				}
			}
		}
		if(mastercard_input(59, &altFlapUp))
		{
			if(altFlapUp == 1)
			{
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[22]);  // ALTERNATE_FLAPS_CTRL_UP
			}
			else
			{
				if(altFlapDn != 1)
				{
					aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[21]);  // ALTERNATE_FLAPS_CTRL_OFF
				}
			}
		}

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



		// Display Source 1 switch position
		if(mastercard_input(63, &displaySrc1))
		{
			if(displaySrc1 == 1)
			{
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[23]);  // INSTRUMENT_DISPLAYS_SOURCE_1
			}
			else
			{
				if(displaySrcAuto != 1)
				{
					aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[24]);  // INSTRUMENT_DISPLAYS_SOURCE_2
				}
			}
		}

		// Display Source Auto switch position
		if(mastercard_input(64, &displaySrcAuto))
		{
			if(displaySrcAuto == 1)
			{
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[25]);  // INSTRUMENT_DISPLAYS_SOURCE_AUTO
			}
			else
			{
				if(displaySrc1 != 1)
				{
					aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[24]);  // INSTRUMENT_DISPLAYS_SOURCE_2
				}
			}
		}
		// IRS Both on Right switch position
		if(mastercard_input(65, &irsBothRight))
		{
			if(irsBothRight == 1)
			{
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[26]);  // FMS_IRS_TFR_R
			}
			else
			{
				if(irsBothLeft != 1)
				{
					aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[27]);  // FMS_IRS_TFR_NORMAL
				}
			}
		}

		// IRS Both on Left switch position
		if(mastercard_input(66, &irsBothLeft))
		{
			if(irsBothLeft == 1)
			{
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[28]);  // FMS_IRS_TFR_L
			}
			else
			{
				if(irsBothRight != 1)
				{
					aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[27]);  // FMS_IRS_TFR_NORMAL
				}
			}
		}


		// VHF NAV Both on 1 switch position
		if(mastercard_input(67, &vhfNavBoth1))
		{
			if(vhfNavBoth1 == 1)
			{

				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[29]);  // FMS_VHF_NAV_L
			}
			else
			{
				if(vhfNavBoth2 != 1)
				{
					aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[30]);  // FMS_VHF_NAV_NORMAL
				}
			}
		}
		// VHF NAV Both on 2 switch position
		if(mastercard_input(68, &vhfNavBoth2))
		{
			if(vhfNavBoth2 == 1)
			{
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[31]);  // FMS_VHF_NAV_R
			}
			else
			{
				if(vhfNavBoth1 != 1)
				{
					aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[30]);  // FMS_VHF_NAV_NORMAL
				}
			}
		}


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