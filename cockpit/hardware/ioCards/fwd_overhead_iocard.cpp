#include <array>
#include <algorithm>
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
	constexpr int FMS_VHF_NAV_R = 1;
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

	//  special value codes for display II card (set hasspecial to 1)
	//   10 = Blank the digit			--> 0xf7 (also 0x0a)
	//   11 = Put the "-" sign			--> 0xf8
	//   12 = Put a Special 6			--> 0xf9
	//   13 = Put a "t"					--> 0xfa
	//   14 = Put a "d"					--> 0xfb
	//   15 = Put a "_" (Underscore)	--> 0xfc (does not work; rather, has something to do with dimming...)
	//
	void ForwardOverheadIOCard::update_electrical_display()
	{
		// AC Volts
		const int ac_volts = static_cast<int>(aircraft_model.z737InData.ac_volt_value);
		if(ac_volts != previous_ac_volts){
			int constexpr AC_VOLTS_1 = 21;
			int constexpr AC_VOLTS_10 = 22;
			int constexpr AC_VOLTS_100 = 23;

			previous_ac_volts = ac_volts;
			if(ac_volts == 0) {
				mastercard_send_display(0, AC_VOLTS_1);
				mastercard_send_display(0xA, AC_VOLTS_10);		// Blank
				mastercard_send_display(0xA, AC_VOLTS_100);		// Blank
			}
			else {
				const uint8_t ac_volt_hundreds = ac_volts/100;
				const int ac_volt_remaining = (ac_volts - (ac_volt_hundreds * 100));
				const uint8_t ac_volt_tens = ac_volt_remaining/10;
				const uint8_t ac_volt_ones = (ac_volt_remaining - (ac_volt_tens * 10));
				mastercard_send_display(ac_volt_ones, AC_VOLTS_1);
				mastercard_send_display(ac_volt_tens, AC_VOLTS_10);
				mastercard_send_display(ac_volt_hundreds, AC_VOLTS_100);
			}	
		}
		// AC Freq
		const int ac_freq = static_cast<int>(aircraft_model.z737InData.ac_freq_value);
		if(ac_freq != previous_ac_freq){
			int constexpr AC_FREQ_1 = 16;
			int constexpr AC_FREQ_10 = 17;
			int constexpr AC_FREQ_100 = 18;

			previous_ac_freq = ac_freq;
			if(ac_freq == 0){
				mastercard_send_display(0, AC_FREQ_1);
				mastercard_send_display(0xA, AC_FREQ_10);
				mastercard_send_display(0xA, AC_FREQ_100);}
			else {
				const uint8_t ac_freq_hundreds = ac_freq/100;
				const int ac_freq_remaining = (ac_freq - (ac_freq_hundreds * 100));
				const uint8_t ac_freq_tens = ac_freq_remaining/10;
				const uint8_t ac_freq_ones = (ac_freq_remaining - (ac_freq_tens * 10));
				mastercard_send_display(ac_freq_ones, AC_FREQ_1);
				mastercard_send_display(ac_freq_tens, AC_FREQ_10);
				mastercard_send_display(ac_freq_hundreds, AC_FREQ_100);
			}
		}
		// DC AMPS
		int dc_amps = static_cast<int>(aircraft_model.z737InData.dc_amp_value);
		if(dc_amps != previous_dc_amps){
			int constexpr DC_AMPS_1 = 19;
			int constexpr DC_AMPS_10 = 20;

			previous_dc_amps = dc_amps;
			if(dc_amps == 0) {
				mastercard_send_display(0, DC_AMPS_1);
				mastercard_send_display(0xA, DC_AMPS_10);				
			}
			else {
				bool is_negative = false;
				if (dc_amps < 0) {
					dc_amps = -dc_amps;
					is_negative = true;
				}
				uint8_t dc_amp_tens = dc_amps / 10;
				uint8_t dc_amp_ones = dc_amps - (dc_amp_tens * 10);
				if(is_negative) {
					if (dc_amps > 10) {
						dc_amp_ones = 9;
					}
					dc_amp_tens = 0xF8;
				}
				mastercard_send_display(dc_amp_ones, DC_AMPS_1);
				mastercard_send_display(dc_amp_tens, DC_AMPS_10);
			}
		}

		// AC AMPS
		int ac_amps = static_cast<int>(aircraft_model.z737InData.ac_amp_value);
		if(ac_amps != previous_ac_amps){
			int constexpr AC_AMPS_1 = 24;
			int constexpr AC_AMPS_10 = 25;

			previous_ac_amps = ac_amps;
			if(ac_amps == 0) {
				mastercard_send_display(0, AC_AMPS_1);
				mastercard_send_display(0xA, AC_AMPS_10);
			}
			else {
				bool is_negative = false;
				if (ac_amps < 0) {
					ac_amps = -ac_amps;
					is_negative = true;
				}
				uint8_t ac_amp_tens = ac_amps / 10;
				uint8_t ac_amp_ones = ac_amps - (ac_amp_tens * 10);
				if (is_negative) {
					if (ac_amps > 10) {
						ac_amp_ones = 9;
					}
					ac_amp_tens = 0xF8;  // minus sign
				}
				mastercard_send_display(ac_amp_ones, AC_AMPS_1);
				mastercard_send_display(ac_amp_tens, AC_AMPS_10);
			}
		}

		// DC Volts
		int dc_volts = static_cast<int>(aircraft_model.z737InData.dc_volt_value);
		if(dc_volts != previous_dc_volts){
			int constexpr DC_VOLTS_1 = 26;
			int constexpr DC_VOLTS_10 = 27;

			previous_dc_volts = dc_volts;
			if(dc_volts == 0) {
				mastercard_send_display(0, DC_VOLTS_1);
				mastercard_send_display(0xA, DC_VOLTS_10);
			}
			else {
				bool is_negative = false;
				if (dc_volts < 0) {
					dc_volts = -dc_volts;
					is_negative = true;
				}
				uint8_t dc_volt_tens = dc_volts/10;
				uint8_t dc_volt_ones = dc_volts - (dc_volt_tens * 10);
				if(is_negative){
					if(dc_volts > 10) {
						dc_volt_ones = 9;
					}
					dc_volt_tens = 0xF8;  // minus sign
				}
				mastercard_send_display(dc_volt_ones, DC_VOLTS_1);
				mastercard_send_display(dc_volt_tens, DC_VOLTS_10);
			}
		}
	}

	// Landing Altitude
	void ForwardOverheadIOCard::update_landing_alt_display(bool use_xplane_value)
	{
		if(use_xplane_value){
			if(aircraft_model.get_freshmess(DataRefName::landing_alt)) {
				landing_altitude = static_cast<long>(aircraft_model.xplane_switch_data.landing_alt);
				aircraft_model.clear_freshness(DataRefName::landing_alt);
			}
		}
		if(landing_altitude != previous_landing_altitude){	
			int constexpr LAND_ALT_1 = 5;
			int constexpr LAND_ALT_10 = 6;
			int constexpr LAND_ALT_100 = 7;
			int constexpr LAND_ALT_1000 = 8;
			int constexpr LAND_ALT_10_000 = 9;

			// Ones position is Always zero
			mastercard_send_display(0, LAND_ALT_1);
			
			previous_landing_altitude = landing_altitude;
			long land_alt = landing_altitude;
			if(land_alt == 0) {
				mastercard_send_display(0xA, LAND_ALT_10);
				mastercard_send_display(0xA, LAND_ALT_100);
				mastercard_send_display(0xA, LAND_ALT_1000);
				mastercard_send_display(0xA, LAND_ALT_10_000);
			}
			else {
				bool is_negative = false;
				if(land_alt < 0) {
					land_alt = -land_alt;
					is_negative = true;
				}
				const uint8_t land_alt_ten_thousands= land_alt / 10000;
				int temp = land_alt - (land_alt_ten_thousands * 10000);
				const uint8_t land_alt_one_thousands = temp / 1000;
				temp = temp - (land_alt_one_thousands *1000);
				const uint8_t land_alt_hundreds = temp / 100;
				temp = temp - (land_alt_hundreds * 100);
				const uint8_t land_alt_tens = temp/10;
				if(is_negative) {
					if(land_alt >= 1000) {
						mastercard_send_display(0, LAND_ALT_10);
						mastercard_send_display(0, LAND_ALT_100);
						mastercard_send_display(1, LAND_ALT_1000);
						mastercard_send_display(0xF8, LAND_ALT_10_000);							
					}
					else if(land_alt >= 100) {
						mastercard_send_display(land_alt_tens, LAND_ALT_10);
						mastercard_send_display(land_alt_hundreds, LAND_ALT_100);
						mastercard_send_display(0xA, LAND_ALT_1000);
						mastercard_send_display(0xF8, LAND_ALT_10_000);							
					}
					else if(land_alt >= 10) {
						mastercard_send_display(land_alt_tens, LAND_ALT_10);
						mastercard_send_display(0xA, LAND_ALT_100);
						mastercard_send_display(0xA, LAND_ALT_1000);
						mastercard_send_display(0xF8, LAND_ALT_10_000);						
					}
					else {
						mastercard_send_display(0xA, LAND_ALT_10);
						mastercard_send_display(0xA, LAND_ALT_100);
						mastercard_send_display(0xA, LAND_ALT_1000);
						mastercard_send_display(0xA, LAND_ALT_10_000);						
					}
				}
				else {
					if(land_alt >= 10000) {
						mastercard_send_display(land_alt_tens, LAND_ALT_10);
						mastercard_send_display(land_alt_hundreds, LAND_ALT_100);
						mastercard_send_display(land_alt_one_thousands, LAND_ALT_1000);
						mastercard_send_display(land_alt_ten_thousands, LAND_ALT_10_000);
					}
					else if(land_alt >= 1000) {
						mastercard_send_display(land_alt_tens, LAND_ALT_10);
						mastercard_send_display(land_alt_hundreds, LAND_ALT_100);
						mastercard_send_display(land_alt_one_thousands, LAND_ALT_1000);
						mastercard_send_display(0xA, LAND_ALT_10_000);
					}
					else if(land_alt >= 100) {
						mastercard_send_display(land_alt_tens, LAND_ALT_10);
						mastercard_send_display(land_alt_hundreds, LAND_ALT_100);
						mastercard_send_display(0xA, LAND_ALT_1000);
						mastercard_send_display(0xA, LAND_ALT_10_000);
					}
					else if(land_alt >= 10) {
						mastercard_send_display(land_alt_tens, LAND_ALT_10);
						mastercard_send_display(0xA, LAND_ALT_100);
						mastercard_send_display(0XA, LAND_ALT_1000);
						mastercard_send_display(0XA, LAND_ALT_10_000);
					} 
				}
			}

		}
	}


	// Flight Altitude
	void ForwardOverheadIOCard::update_flight_alt_display(bool use_xplane_value)
	{
		if(use_xplane_value){
			if(aircraft_model.get_freshmess(DataRefName::max_allowable_altitude)) {
				flight_altitude = static_cast<long>(aircraft_model.xplane_switch_data.max_allowable_altitude);
				aircraft_model.clear_freshness(DataRefName::max_allowable_altitude);
			}

		}
		if(flight_altitude != previous_flight_altitude){
			int constexpr FLT_ALT_1 = 0;
			int constexpr FLT_ALT_10 = 1;
			int constexpr FLT_ALT_100 = 2;
			int constexpr FLT_ALT_1000 = 3;
			int constexpr FLT_ALT_10_000 = 4;

			// Tens and Ones positions are Always zero
			mastercard_send_display(0,   FLT_ALT_1);

			previous_flight_altitude = flight_altitude;
			long flt_alt = flight_altitude;

			if(flt_alt == 0) {
				mastercard_send_display(0xA, FLT_ALT_10);
				mastercard_send_display(0xA, FLT_ALT_100);
				mastercard_send_display(0xA, FLT_ALT_1000);
				mastercard_send_display(0xA, FLT_ALT_10_000);
			}
			else {
				bool is_negative = false;
				if(flt_alt < 0) {
					flt_alt = -flt_alt;
					is_negative = true;
				}


				const uint8_t flight_alt_ten_thousands= flt_alt / 10000;
				int temp = flt_alt - (flight_alt_ten_thousands * 10000);
				const uint8_t  flight_alt_one_thousands = temp / 1000;
				temp = temp - (flight_alt_one_thousands *1000);
				const uint8_t  flight_alt_hundreds = temp / 100;
				if(is_negative) {
					// always Blank
					mastercard_send_display(0xF8, FLT_ALT_10_000);							

					if(flt_alt >= 1000) {
						mastercard_send_display(0, FLT_ALT_10);
						mastercard_send_display(0, FLT_ALT_100);
						mastercard_send_display(1, FLT_ALT_1000);
					}
					else if(flt_alt >= 100) {
						mastercard_send_display(0, FLT_ALT_10);
						mastercard_send_display(flight_alt_hundreds, FLT_ALT_100);
						mastercard_send_display(0xA, FLT_ALT_1000);
					}
					else if(flt_alt < 100) {
						mastercard_send_display(0, FLT_ALT_10);
						mastercard_send_display(0xA, FLT_ALT_100);
						mastercard_send_display(0xA, FLT_ALT_1000);
					}
				}
				else {
					if(flt_alt >= 10000){
						mastercard_send_display(0, FLT_ALT_10);
						mastercard_send_display(flight_alt_hundreds, FLT_ALT_100);
						mastercard_send_display(flight_alt_one_thousands, FLT_ALT_1000);
						mastercard_send_display(flight_alt_ten_thousands, FLT_ALT_10_000);
					}
					else if(flt_alt >= 1000){
						mastercard_send_display(0, FLT_ALT_10);
						mastercard_send_display(flight_alt_hundreds, FLT_ALT_100);
						mastercard_send_display(flight_alt_one_thousands, FLT_ALT_1000);
						mastercard_send_display(0XA, FLT_ALT_10_000);
					}else if(flt_alt >= 100){
						mastercard_send_display(0, FLT_ALT_10);
						mastercard_send_display(flight_alt_hundreds, FLT_ALT_100);
						mastercard_send_display(0XA, FLT_ALT_1000);
						mastercard_send_display(0XA, FLT_ALT_10_000);
					}
				}
			}
		}
	}


	// Encoders
	void ForwardOverheadIOCard::update_encoders()
	{
		double value = 0.0;

		// Flight Altitude
		if(mastercard_encoder(0, &value, 1.0, 0.1) > 0)
		{
			fresh_flight_altitude = true;
			LOG() << "Flt Alt RAW value " << value;
			value = std::clamp(value, -1.0, 1.0);
			LOG() << "Flt Alt Clamp value " << value;
			LOG() << "Before Flt Alt float delta " << value << " flight_altitude " << flight_altitude;
			flight_altitude += static_cast<long>(value*500);
			if(flight_altitude < -1000.0) {
				flight_altitude = -1000.0;
			}
			else if(flight_altitude > 42000.0) {
				flight_altitude = 42000.0;
			}
			LOG() << "After Flt Alt float delta " << value << " flight_altitude " << flight_altitude;
		}
		// Landing Altitude
		value = 0.0;
		if(mastercard_encoder(2, &value, 1.0, 0.1) > 0)
		{
			fresh_landing_altitude = true;
			LOG() << "Land Alt RAW value " << value;
			value = std::clamp(value, -1.0, 1.0);
			LOG() << "Land Alt Clamp value " << value;
			landing_altitude += static_cast<long>( value*50 );
			if(landing_altitude < -1000.0) {
				landing_altitude = -1000.0;
			}
			else if(landing_altitude > 13600.0) {
				landing_altitude = 13600.0;
			}
			LOG() << "Land Alt float delta " << value << " landing_altitude " << landing_altitude;
		}


		if(previous_flight_altitude != flight_altitude)
		{
			update_flight_alt_display(false);
		}
		if(previous_landing_altitude != landing_altitude)
		{
			update_landing_alt_display(false);
		}
	}
	void ForwardOverheadIOCard::process_encoders()
	{
		if (fresh_flight_altitude) {
			fresh_flight_altitude = false;
			auto sw = iocard_fwd_overhead_zcockpit_switches[SwitchPosition::max_allowable_altitude]; // FLT ALT
			sw.float_hw_value = static_cast<float>(flight_altitude);
			LOG() << "process_encoders: push flt alt " << sw.float_hw_value << " should = " << flight_altitude;
			aircraft_model.push_switch_change(sw);
		}
		if (fresh_landing_altitude) {
			fresh_landing_altitude = false;
			auto sw = iocard_fwd_overhead_zcockpit_switches[SwitchPosition::landing_alt]; // LAND ALT
			sw.float_hw_value = static_cast<float>(landing_altitude);
			aircraft_model.push_switch_change(sw);
		}
	}


	void ForwardOverheadIOCard::initialize_switches()
	{

		constexpr int PRESSURIZATION_MODE_AUTO = 0;		// pin 4
		constexpr int PRESSURIZATION_MODE_ALTN = 1;		// pin 5
		constexpr int PRESSURIZATION_MODE_MAN = 2;		// pin 6

		constexpr int FLIGHT_SPOILER_B_OFF = 0;			// pin 7
		constexpr int FLIGHT_SPOILER_B_ON = 1;			// pin 7

		constexpr int AC_DC_MAINT_TEST_ON = 1;			// pin 18
		constexpr int AC_DC_MAINT_TEST_OFF = 0;			// pin 18
		constexpr int ELECTRICAL_DC_METER_STBY = 0;		// pin 9
		constexpr int ELECTRICAL_DC_METER_BAT_BUS = 1;	// pin 10
		constexpr int ELECTRICAL_DC_METER_BAT = 2;		// pin 11 
//		constexpr int ELECTRICAL_DC_METER_AUX_BAT		// pin 12 
		constexpr int ELECTRICAL_DC_METER_TR1 = 3;		// pin 13 
		constexpr int ELECTRICAL_DC_METER_TR2 = 4;		// pin 14 
		constexpr int ELECTRICAL_DC_METER_TR3 = 5;		// pin 15 
		constexpr int ELECTRICAL_DC_METER_TEST = 6;		// pin 16 

		//electrical panel maint push button
		constexpr int ELECTRICAL_MAINT_TEST = 1;		// pin 18 pushbutton command press/release

		constexpr int ELECTRICAL_AC_METER_STDBY = 0;	// pin 19
		constexpr int ELECTRICAL_AC_METER_GND_PWR = 1;	// pin 20
		constexpr int ELECTRICAL_AC_METER_GEN_1 = 2;	// pin 21
		constexpr int ELECTRICAL_AC_METER_GEN_2 = 4;	// pin 22 
		constexpr int ELECTRICAL_AC_METER_APU = 3;		// pin 23 
		constexpr int ELECTRICAL_AC_METER_TEST = 6;		// pin 24 
		constexpr int ELECTRICAL_AC_METER_INV = 5;		// pin 25 


		constexpr int FLTCTRL_FLIGHT_SPOILER_A_OFF = 1;	// pin 26
		constexpr int FLTCTRL_FLIGHT_SPOILER_A_ON  = 0;	// pin 26
		constexpr int ANTIICE_WING_ON = 1;				// pin 33
		constexpr int ANTIICE_WING_OFF = 0;				// pin 33
		constexpr int ANTIICE_ENG_1_OFF = 1;			// pin 34
		constexpr int ANTIICE_ENG_1_ON  = 0;			// pin 34

		constexpr int WINGBODY_OVHT_TEST_ON = 1;		// pin 36
		constexpr int WINGBODY_OVHT_TEST_OFF = 0;		// pin 36

		constexpr int RIGHT_RECIRC_FAN_AUTO = 1;		// pin 37
		constexpr int RIGHT_RECIRC_FAN_OFF = 0;			// pin 37
		constexpr int RIGHT_PACK_HIGH = 2;				// pin 38
		constexpr int RIGHT_PACK_AUTO = 1;				// pin 38
		constexpr int RIGHT_PACK_OFF = 0;				// pin 39
		constexpr int ISOLATION_VALVE_OPEN = 2;			// pin 40
		constexpr int ISOLATION_VALVE_AUTO =1;			// pin 40
		constexpr int ISOLATION_VALVE_CLOSE = 0;		// pin 41
		constexpr int LEFT_PACK_HIGH = 2;				// pin 42
		constexpr int LEFT_PACK_AUTO = 1;				// pin 42
		constexpr int LEFT_PACK_OFF = 0;				// pin 43
		constexpr int LEFT_RECIRC_FAN_AUTO = 1;			// pin 44
		constexpr int LEFT_RECIRC_FAN_OFF = 0;			// pin 44
		constexpr int ENG_2_BLEED_OFF = 1;				// pin 45
		constexpr int ENG_2_BLEED_ON = 0;				// pin 45

		constexpr int TRIP_RESET_ON = 1;				// pin 46
		constexpr int TRIP_RESET_OFF = 0;				// pin 46

		constexpr int APU_BLEED_OFF = 1;				// pin 47
		constexpr int APU_BLEED_ON = 0;					// pin 47
		constexpr int ENG_1_BLEED_OFF = 1;				// pin 48
		constexpr int ENG_1_BLEED_ON = 0;				// pin 48

		constexpr int POSITION_LIGHT_STROBE = 2;		// pin 49
		constexpr int POSITION_LIGHT_OFF = 1;			// pin 49
		constexpr int POSITION_LIGHT_STEADY = 0;		// pin 50

		constexpr int DC_KNOB_STBY_PWR = 0;				// pin 9
		constexpr int DC_KNOB_BAT_BUS = 1;				// pin 10
		constexpr int DC_KNOB_BAT = 2;					// pin 11
														// pin 12  -- AUX BAT  -- not supported
		constexpr int DC_KNOB_TR1 = 3;					// pin 13
		constexpr int DC_KNOB_TR2 = 4;					// pin 14
		constexpr int DC_KNOB_TR3 = 5;					// pin 15
		constexpr int DC_KNOB_TEST = 6;					// pin 16

		constexpr int AC_KNOB_STBY_PWR = 0;				// pin 19
		constexpr int AC_KNOB_GND_PWR = 1;				// pin 20
		constexpr int AC_KNOB_GEN1 = 2;					// pin 21
		constexpr int AC_KNOB_APU = 3;					// pin 22
		constexpr int AC_KNOB_GEN2 = 4;					// pin 23
		constexpr int AC_KNOB_INV = 5;					// pin 24
		constexpr int AC_KNOB_TEST = 6;					// pin 24



		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::starter1_pos_gnd]  = ZcockpitSwitch(DataRefName::starter1_pos, common::SwitchType::rotary_multi_commands, GND);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::starter1_pos_off]  = ZcockpitSwitch(DataRefName::starter1_pos, common::SwitchType::rotary_multi_commands, OFF);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::starter1_pos_cont]  = ZcockpitSwitch(DataRefName::starter1_pos, common::SwitchType::rotary_multi_commands, CONT);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::starter1_pos_flt]  = ZcockpitSwitch(DataRefName::starter1_pos, common::SwitchType::rotary_multi_commands, FLT);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::starter2_pos_gnd]  = ZcockpitSwitch(DataRefName::starter2_pos, common::SwitchType::rotary_multi_commands, GND);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::starter2_pos_off]  = ZcockpitSwitch(DataRefName::starter2_pos, common::SwitchType::rotary_multi_commands, OFF);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::starter2_pos_cont]  = ZcockpitSwitch(DataRefName::starter2_pos, common::SwitchType::rotary_multi_commands, CONT);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::starter2_pos_flt]  = ZcockpitSwitch(DataRefName::starter2_pos, common::SwitchType::rotary_multi_commands, FLT);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::drive_disconnect1_pos_generator_disconnect_up]   = ZcockpitSwitch(DataRefName::drive_disconnect1_pos, common::SwitchType::toggle, GENERATOR_DISCONNECT_UP );
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::drive_disconnect1_pos_generator_disconnect_down]   = ZcockpitSwitch(DataRefName::drive_disconnect1_pos, common::SwitchType::toggle, GENERATOR_DISCONNECT_DOWN);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::air_valve_manual_outflow_valve_open]  = ZcockpitSwitch(DataRefName::air_valve_manual, common::SwitchType::spring_loaded, OUTFLOW_VALVE_OPEN);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::air_valve_manual_outflow_valve_middle]  = ZcockpitSwitch(DataRefName::air_valve_manual, common::SwitchType::spring_loaded, OUTFLOW_VALVE_MIDDLE);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::air_valve_manual_outflow_valve_close]  = ZcockpitSwitch(DataRefName::air_valve_manual, common::SwitchType::spring_loaded, OUTFLOW_VALVE_CLOSE);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::alt_flaps_pos_alternate_flaps_arm]  = ZcockpitSwitch(DataRefName::alt_flaps_pos, common::SwitchType::toggle, ALTERNATE_FLAPS_ARM );
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::alt_flaps_pos_alternate_flaps_off]  = ZcockpitSwitch(DataRefName::alt_flaps_pos, common::SwitchType::toggle, ALTERNATE_FLAPS_OFF);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::flt_ctr_b_pos_flight_control_b_on]  = ZcockpitSwitch(DataRefName::flt_ctr_B_pos, common::SwitchType::toggle, FLIGHT_CONTROL_B_ON);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::flt_ctr_b_pos_flight_control_b_off]  = ZcockpitSwitch(DataRefName::flt_ctr_B_pos, common::SwitchType::toggle, FLIGHT_CONTROL_B_OFF );
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::flt_ctr_b_pos_flight_control_b_stbyrud]  = ZcockpitSwitch(DataRefName::flt_ctr_B_pos, common::SwitchType::toggle, FLIGHT_CONTROL_B_STBYRUD );
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::flt_ctr_a_pos_flight_control_a_on]  = ZcockpitSwitch(DataRefName::flt_ctr_A_pos, common::SwitchType::toggle, FLIGHT_CONTROL_A_ON);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::flt_ctr_a_pos_flight_control_a_off]  = ZcockpitSwitch(DataRefName::flt_ctr_A_pos, common::SwitchType::toggle, FLIGHT_CONTROL_A_OFF );
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::flt_ctr_a_pos_flight_control_a_stbyrud]  = ZcockpitSwitch(DataRefName::flt_ctr_A_pos, common::SwitchType::toggle, FLIGHT_CONTROL_A_STBYRUD );
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::alt_flaps_ctrl_alternate_flaps_ctrl_dn]  = ZcockpitSwitch(DataRefName::alt_flaps_ctrl, common::SwitchType::spring_loaded, ALTERNATE_FLAPS_CTRL_DN);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::alt_flaps_ctrl_alternate_flaps_ctrl_off]  = ZcockpitSwitch(DataRefName::alt_flaps_ctrl, common::SwitchType::toggle, ALTERNATE_FLAPS_CTRL_OFF );
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::alt_flaps_ctrl_alternate_flaps_ctrl_up]  = ZcockpitSwitch(DataRefName::alt_flaps_ctrl, common::SwitchType::toggle, ALTERNATE_FLAPS_CTRL_UP);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::dspl_source_instrument_displays_source_1]  = ZcockpitSwitch(DataRefName::dspl_source, common::SwitchType::toggle, INSTRUMENT_DISPLAYS_SOURCE_1 );
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::dspl_source_instrument_displays_source_2]  = ZcockpitSwitch(DataRefName::dspl_source, common::SwitchType::toggle, INSTRUMENT_DISPLAYS_SOURCE_2);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::dspl_source_instrument_displays_source_auto]  = ZcockpitSwitch(DataRefName::dspl_source, common::SwitchType::toggle, INSTRUMENT_DISPLAYS_SOURCE_AUTO );
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::irs_source_fms_irs_tfr_r]  = ZcockpitSwitch(DataRefName::irs_source, common::SwitchType::toggle, FMS_IRS_TFR_R);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::irs_source_fms_irs_tfr_normal]  = ZcockpitSwitch(DataRefName::irs_source, common::SwitchType::toggle, FMS_IRS_TFR_NORMAL );
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::irs_source_fms_irs_tfr_l]  = ZcockpitSwitch(DataRefName::irs_source, common::SwitchType::toggle, FMS_IRS_TFR_L);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::vhf_nav_source_fms_vhf_nav_l]  = ZcockpitSwitch(DataRefName::vhf_nav_source, common::SwitchType::toggle, FMS_VHF_NAV_L );
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::vhf_nav_source_fms_vhf_nav_normal]  = ZcockpitSwitch(DataRefName::vhf_nav_source, common::SwitchType::toggle, FMS_VHF_NAV_NORMAL);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::vhf_nav_source_fms_vhf_nav_r]  = ZcockpitSwitch(DataRefName::vhf_nav_source, common::SwitchType::toggle, FMS_VHF_NAV_R );
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::max_allowable_altitude]  = ZcockpitSwitch(DataRefName::max_allowable_altitude, common::SwitchType::raw_encoder, 0.0f, 0, 0);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::landing_alt]  = ZcockpitSwitch(DataRefName::landing_alt, common::SwitchType::raw_encoder, 0.0f, 0, 0);

		//Spoiler B  switch position pin 7
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::spoiler_b_pos_off] =ZcockpitSwitch(DataRefName::spoiler_B_pos,      common::SwitchType::toggle, FLIGHT_SPOILER_B_OFF);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::spoiler_b_pos_on] =ZcockpitSwitch(DataRefName::spoiler_B_pos,      common::SwitchType::toggle, FLIGHT_SPOILER_B_ON);

		// DC Meter Position pins 9-16
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::dc_knob_stby_pwr] = ZcockpitSwitch(DataRefName::dc_power, common::SwitchType::rotary_2_commands,DC_KNOB_STBY_PWR);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::dc_knob_bat_bus] = ZcockpitSwitch(DataRefName::dc_power, common::SwitchType::rotary_2_commands,DC_KNOB_BAT_BUS);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::dc_knob_bat] = ZcockpitSwitch(DataRefName::dc_power, common::SwitchType::rotary_2_commands,DC_KNOB_BAT);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::dc_knob_aux_bat] = ZcockpitSwitch(DataRefName::dc_power, common::SwitchType::rotary_2_commands,DC_KNOB_BAT);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::dc_knob_tr1] = ZcockpitSwitch(DataRefName::dc_power, common::SwitchType::rotary_2_commands,DC_KNOB_TR1);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::dc_knob_tr2] = ZcockpitSwitch(DataRefName::dc_power, common::SwitchType::rotary_2_commands,DC_KNOB_TR2);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::dc_knob_tr3] = ZcockpitSwitch(DataRefName::dc_power, common::SwitchType::rotary_2_commands,DC_KNOB_TR3);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::dc_knob_test] = ZcockpitSwitch(DataRefName::dc_power, common::SwitchType::rotary_2_commands,DC_KNOB_TEST);


		// AC DC Maint test push button pin 18
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::acdc_maint_pos_on] =ZcockpitSwitch(DataRefName::acdc_maint_pos, common::SwitchType::spring_loaded, AC_DC_MAINT_TEST_ON);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::acdc_maint_pos_off] = ZcockpitSwitch(DataRefName::acdc_maint_pos, common::SwitchType::spring_loaded, AC_DC_MAINT_TEST_OFF);

		// DC Meter Position pins 9-16
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::ac_knob_stby_pwr] = ZcockpitSwitch(DataRefName::ac_power, common::SwitchType::rotary_2_commands, AC_KNOB_STBY_PWR);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::ac_knob_gnd_pwr] = ZcockpitSwitch(DataRefName::ac_power, common::SwitchType::rotary_2_commands, AC_KNOB_GND_PWR);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::ac_knob_gen1] = ZcockpitSwitch(DataRefName::ac_power, common::SwitchType::rotary_2_commands, AC_KNOB_GEN1);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::ac_knob_apu] = ZcockpitSwitch(DataRefName::ac_power, common::SwitchType::rotary_2_commands, AC_KNOB_APU);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::ac_knob_gen2] = ZcockpitSwitch(DataRefName::ac_power, common::SwitchType::rotary_2_commands, AC_KNOB_GEN2);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::ac_knob_inv] = ZcockpitSwitch(DataRefName::ac_power, common::SwitchType::rotary_2_commands, AC_KNOB_INV);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::ac_knob_test] = ZcockpitSwitch(DataRefName::ac_power, common::SwitchType::rotary_2_commands, AC_KNOB_TEST);





		//Spoiler A  switch position: pin 26
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::spoiler_a_pos_off] =ZcockpitSwitch(DataRefName::spoiler_A_pos,      common::SwitchType::toggle, FLTCTRL_FLIGHT_SPOILER_A_OFF);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::spoiler_a_pos_on] =ZcockpitSwitch(DataRefName::spoiler_A_pos,      common::SwitchType::toggle, FLTCTRL_FLIGHT_SPOILER_A_ON);

		//wing anti ice switch: pin 33
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::wing_heat_pos_on] =ZcockpitSwitch(DataRefName::wing_heat_pos,      common::SwitchType::toggle, ANTIICE_WING_ON);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::wing_heat_pos_off] =ZcockpitSwitch(DataRefName::wing_heat_pos,      common::SwitchType::toggle, ANTIICE_WING_OFF);

		//engine 1 anti ice switch: pin 34
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::eng1_heat_pos_off] =ZcockpitSwitch(DataRefName::eng1_heat_pos,      common::SwitchType::toggle, ANTIICE_ENG_1_OFF);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::eng1_heat_pos_on] =ZcockpitSwitch(DataRefName::eng1_heat_pos,      common::SwitchType::toggle, ANTIICE_ENG_1_ON);

		// wing body over heat test: pin 36
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::duct_ovht_test_pos_on] =ZcockpitSwitch(DataRefName::duct_ovht_test_pos, common::SwitchType::spring_loaded, WINGBODY_OVHT_TEST_ON);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::duct_ovht_test_pos_off] = ZcockpitSwitch(DataRefName::duct_ovht_test_pos, common::SwitchType::spring_loaded, WINGBODY_OVHT_TEST_OFF);

		//right recerc fan: pin 37
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::r_recirc_fan_pos_auto] =ZcockpitSwitch(DataRefName::r_recirc_fan_pos,   common::SwitchType::toggle, RIGHT_RECIRC_FAN_AUTO);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::r_recirc_fan_pos_off] =ZcockpitSwitch(DataRefName::r_recirc_fan_pos,   common::SwitchType::toggle, RIGHT_RECIRC_FAN_OFF);

		//right pack high switch: pin 38
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::r_pack_pos_high] =ZcockpitSwitch(DataRefName::r_pack_pos,         common::SwitchType::toggle, RIGHT_PACK_HIGH);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::r_pack_pos_auto] =ZcockpitSwitch(DataRefName::r_pack_pos,         common::SwitchType::toggle, RIGHT_PACK_AUTO);
		//right pack off switch: pin 39
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::r_pack_pos_off] =ZcockpitSwitch(DataRefName::r_pack_pos,         common::SwitchType::toggle, RIGHT_PACK_OFF);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::r_pack_pos_auto] =ZcockpitSwitch(DataRefName::r_pack_pos,         common::SwitchType::toggle, RIGHT_PACK_AUTO);

		//isolation valve open auto: pin 40
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::isolation_valve_pos_open] =ZcockpitSwitch(DataRefName::isolation_valve_pos,common::SwitchType::toggle, ISOLATION_VALVE_OPEN);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::isolation_valve_pos_auto] =ZcockpitSwitch(DataRefName::isolation_valve_pos,common::SwitchType::toggle, ISOLATION_VALVE_AUTO);
		//isolation valve close switch: pin 41
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::isolation_valve_pos_close] =ZcockpitSwitch(DataRefName::isolation_valve_pos,common::SwitchType::toggle, ISOLATION_VALVE_CLOSE);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::isolation_valve_pos_auto] =ZcockpitSwitch(DataRefName::isolation_valve_pos,common::SwitchType::toggle, ISOLATION_VALVE_AUTO);

		//left pack high: pin 42
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::l_pack_pos_high] =ZcockpitSwitch(DataRefName::l_pack_pos,         common::SwitchType::toggle, LEFT_PACK_HIGH);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::l_pack_pos_auto] =ZcockpitSwitch(DataRefName::l_pack_pos,         common::SwitchType::toggle, LEFT_PACK_AUTO);
		//left pack switch: pin 43
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::l_pack_pos_off] =ZcockpitSwitch(DataRefName::l_pack_pos,         common::SwitchType::toggle, LEFT_PACK_OFF);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::l_pack_pos_auto] =ZcockpitSwitch(DataRefName::l_pack_pos,         common::SwitchType::toggle, LEFT_PACK_AUTO);

		//left recirc fan switch: pin 44
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::l_recirc_fan_pos_auto] =ZcockpitSwitch(DataRefName::l_recirc_fan_pos,   common::SwitchType::toggle, LEFT_RECIRC_FAN_AUTO);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::l_recirc_fan_pos_off] =ZcockpitSwitch(DataRefName::l_recirc_fan_pos,   common::SwitchType::toggle, LEFT_RECIRC_FAN_OFF);

		//engine 2 bleed: pin 45
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::bleed_air_2_pos_off] =ZcockpitSwitch(DataRefName::bleed_air_2_pos,    common::SwitchType::toggle, ENG_2_BLEED_OFF);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::bleed_air_2_pos_on] =ZcockpitSwitch(DataRefName::bleed_air_2_pos,    common::SwitchType::toggle, ENG_2_BLEED_ON);

		// TRIP rest: pin 46
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::bleed_trip_reset_pb_on] =ZcockpitSwitch(DataRefName::bleed_trip_reset_pb,common::SwitchType::spring_loaded, TRIP_RESET_ON);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::bleed_trip_reset_pb_off] = ZcockpitSwitch(DataRefName::bleed_trip_reset_pb, common::SwitchType::spring_loaded, TRIP_RESET_OFF);

		//apu bleed switch: pin 47
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::bleed_air_apu_pos_off] =ZcockpitSwitch(DataRefName::bleed_air_apu_pos,  common::SwitchType::toggle, APU_BLEED_OFF);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::bleed_air_apu_pos_on] =ZcockpitSwitch(DataRefName::bleed_air_apu_pos,  common::SwitchType::toggle, APU_BLEED_ON);

		//engine 1 bleed switch: pin 48
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::bleed_air_1_pos_off] =ZcockpitSwitch(DataRefName::bleed_air_1_pos,    common::SwitchType::toggle, ENG_1_BLEED_OFF);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::bleed_air_1_pos_on] =ZcockpitSwitch(DataRefName::bleed_air_1_pos,    common::SwitchType::toggle, ENG_1_BLEED_ON);

		//Position light steady: pin 49
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::position_light_pos_steady] =ZcockpitSwitch(DataRefName::position_light_pos, common::SwitchType::rotary_multi_commands, POSITION_LIGHT_STEADY);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::position_light_pos_off] =ZcockpitSwitch(DataRefName::position_light_pos, common::SwitchType::rotary_multi_commands, POSITION_LIGHT_OFF);
		//Position light strobe and steady: pin 50
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::position_light_pos_strobe] =ZcockpitSwitch(DataRefName::position_light_pos, common::SwitchType::rotary_multi_commands, POSITION_LIGHT_STROBE);
		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::position_light_pos_off] =ZcockpitSwitch(DataRefName::position_light_pos, common::SwitchType::rotary_multi_commands, POSITION_LIGHT_OFF);



	}

	void ForwardOverheadIOCard::process_overhead()
	{
		const unsigned char pwr = aircraft_model.z738_ac_power_is_on()? 1 : 0;


		//
		// Process Starter Switches
		//
		process_master_card_inputs(engine_start_switches, NUMBER_OF_ENGINE_SWITCH_STATES);


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
							aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::starter1_pos_gnd]);
							break;

						case OFF:
							aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::starter1_pos_off]);
							break;

						case CONT:
							aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::starter1_pos_cont]);
							break;

						case FLT:
							aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::starter1_pos_flt]);
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
							aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::starter2_pos_gnd]);
							break;

						case OFF:
							aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::starter2_pos_off]);
							break;

						case CONT:
							aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::starter2_pos_cont]);
							break;

						case FLT:
							aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::starter2_pos_flt]);
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
						aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::drive_disconnect1_pos_generator_disconnect_up]);
					}
				}
				else
				{
					if(lastGenCmd != GENERATOR_DISCONNECT_DOWN)
					{
						lastGenCmd = GENERATOR_DISCONNECT_DOWN;
						aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::drive_disconnect1_pos_generator_disconnect_down]);
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

		// Spoiler B switch position pin 7
		if(mastercard_input(7, &spoiler_b))
		{
			if(spoiler_b == 1)
			{
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::spoiler_b_pos_on]); 
			}
			else {
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::spoiler_b_pos_off]);
			}
		}

		// DC Meter Position 
		if(mastercard_input(9, &dc_stby_pwr) && dc_stby_pwr)
		{
			aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::dc_knob_stby_pwr]);
		}
		else if(mastercard_input(10, &dc_bat_bus) && dc_bat_bus)
		{
			aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::dc_knob_bat_bus]);
		}
		else if(mastercard_input(11, &dc_bat) && dc_bat)
		{
			aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::dc_knob_bat]);
		}
		else if(mastercard_input(12, &dc_aux_bat) && dc_aux_bat)
		{
			aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::dc_knob_bat]);
		}
		else if(mastercard_input(13, &dc_tr1) && dc_tr1)
		{
			aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::dc_knob_tr1]);
		}
		else if(mastercard_input(14, &dc_tr2) && dc_tr2)
		{
			aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::dc_knob_tr2]);
		}
		else if(mastercard_input(15, &dc_tr3) && dc_tr3)
		{
			aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::dc_knob_tr3]);
		}
		else if(mastercard_input(16, &dc_test) && dc_test)
		{
			aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::dc_knob_test]);
		}


		// AC DC Maint test push button pin 18
		if(mastercard_input(18, &acdc_maint))
		{
			if (acdc_maint) {
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::acdc_maint_pos_on]);
			}
			else {
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::acdc_maint_pos_off]);
			}
		}

		// AC Meter Position 19-25
		if(mastercard_input(19, &ac_stby_pwr) && ac_stby_pwr)
		{
			aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::ac_knob_stby_pwr]);
		}
		else if(mastercard_input(20, &ac_gnd_pwr) && ac_gnd_pwr)
		{
			aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::ac_knob_gnd_pwr]);
		}
		else if(mastercard_input(21, &ac_gen1) && ac_gen1)
		{
			aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::ac_knob_gen1]);
		}
		else if(mastercard_input(22, &ac_gen2) && ac_gen2)
		{
			aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::ac_knob_gen2]);
		}
		else if(mastercard_input(23, &ac_apu) && ac_apu)
		{
			aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::ac_knob_apu]);
		}
		else if(mastercard_input(24, &ac_test) && ac_test)
		{
			aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::ac_knob_test]);
		}
		else if(mastercard_input(25, &ac_inv) && ac_inv)
		{
			aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::ac_knob_inv]);
		}



		// Spoiler A OFF switch position pin 26
		if(mastercard_input(26, &spoiler_a))
		{
			if(spoiler_a == 1)
			{
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::spoiler_a_pos_off]); 
			}
			else {
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::spoiler_a_pos_on]);
			}
		}
		//wing anti ice switch: pin 33
		if(mastercard_input(33, &wing_heat))
		{
			if(wing_heat == 1)
			{
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::wing_heat_pos_on]); 
			}
			else {
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::wing_heat_pos_off]);
			}
		}

		//engine 1 anti ice switch: pin 34
		if(mastercard_input(34, &eng1_heat))
		{
			if(eng1_heat == 1)
			{
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::eng1_heat_pos_on]); 
			}
			else {
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::eng1_heat_pos_off]);
			}
		}

		// wing body over heat test: pin 36
		if(mastercard_input(36, &duct_ovht_test))
		{
			if (duct_ovht_test) {
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::duct_ovht_test_pos_on]);
			}
			else {
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::duct_ovht_test_pos_off]);
			}
		}

		//right recerc fan: pin 37
		if(mastercard_input(37, &r_recirc_fan))
		{
			if(r_recirc_fan == 1)
			{
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::r_recirc_fan_pos_auto]); 
			}
			else {
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::r_recirc_fan_pos_off]);
			}
		}

		//right pack high switch: pin 38
		const auto r_pack_high_changed = mastercard_input(38, &r_pack_high);
		const auto r_pack_off_changed = mastercard_input(39, &r_pack_off);
		if(r_pack_high_changed)
		{
			if(r_pack_high == 1)
			{
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::r_pack_pos_high]); 
			}
			else if(r_pack_off != 1) {
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::r_pack_pos_auto]);
			}
		}
		//right pack off switch: pin 39
		if(r_pack_off_changed)
		{
			if(r_pack_off == 1)
			{
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::r_pack_pos_off]); 
			}
			else if(r_pack_high != 1) {
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::r_pack_pos_auto]);
			}
		}

		//isolation valve open auto: pin 40
		const auto isolation_valve_open_changed = mastercard_input(40, &isolation_valve_open);
		const auto isolation_valve_closed_changed = mastercard_input(41, &isolation_valve_closed);
		if(isolation_valve_open_changed)
		{
			if(isolation_valve_open == 1)
			{
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::isolation_valve_pos_open]); 
			}
			else if(isolation_valve_closed != 1){
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::isolation_valve_pos_auto]);
			}
		}
		//isolation valve close switch: pin 41
		if(isolation_valve_closed_changed)
		{
			if(isolation_valve_closed == 1)
			{
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::isolation_valve_pos_close]); 
			}
			else if(isolation_valve_open != 1) {
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::isolation_valve_pos_auto]);
			}
		}

		//left pack high: pin 42
		const auto l_pack_off_changed = mastercard_input(42, &l_pack_off);
		const auto l_pack_high_changed = mastercard_input(43, &l_pack_high);
		if(l_pack_off_changed)
		{
			if(l_pack_off == 1)
			{
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::l_pack_pos_high]); 
			}
			else if(l_pack_high != 1) {
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::l_pack_pos_auto]);
			}
		}
		//left pack switch: pin 43
		if(l_pack_high_changed)
		{
			if(l_pack_high == 1)
			{
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::l_pack_pos_off]); 
			}
			else if(l_pack_off != 1) {
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::l_pack_pos_auto]);
			}
		}

		//left recirc fan switch: pin 44
		if(mastercard_input(44, &l_recirc_fan))
		{
			if(l_recirc_fan == 1)
			{
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::l_recirc_fan_pos_auto]); 
			}
			else {
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::l_recirc_fan_pos_off]);
			}
		}

		//engine 2 bleed: pin 45
		if(mastercard_input(45, &bleed_air_2))
		{
			if(bleed_air_2 == 1)
			{
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::bleed_air_2_pos_on]); 
			}
			else {
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::bleed_air_2_pos_off]);
			}
		}

		// TRIP rest: pin 46
		if(mastercard_input(46, &bleed_trip_reset))
		{
			if (bleed_trip_reset != 1) {
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::bleed_trip_reset_pb_on]);
			}
			else {
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::bleed_trip_reset_pb_off]);
			}
		}

		//apu bleed switch: pin 47
		if(mastercard_input(47, &bleed_air_apu))
		{
			if(bleed_air_apu == 1)
			{
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::bleed_air_apu_pos_on]); 
			}
			else {
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::bleed_air_apu_pos_off]);
			}
		}

		//engine 1 bleed switch: pin 48
		if(mastercard_input(48, &bleed_air_1))
		{
			if(bleed_air_1 == 1)
			{
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::bleed_air_1_pos_on]); 
			}
			else {
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::bleed_air_1_pos_off]);
			}
		}

		//Position light steady: pin 49
		const auto position_light_steady_changed = mastercard_input(49, &position_light_steady);
		const auto position_light_strobe_changed = mastercard_input(50, &position_light_strobe);
		if(position_light_steady_changed)
		{
			if(position_light_steady == 1)
			{
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::position_light_pos_steady]); 
			}
			else if(position_light_strobe != 1) {
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::position_light_pos_off]);
			}
		}
		//Position light strobe and steady: pin 50
		if(position_light_strobe_changed)
		{
			if(position_light_strobe == 1)
			{
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::position_light_pos_strobe]); 
			}
			else if(position_light_steady != 1) {
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::position_light_pos_off]);
			}
		}

		// pressurization manual valve
		if(mastercard_input(51, &outflowOpen))
		{
			if(outflowOpen == 1)
			{
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::air_valve_manual_outflow_valve_open]);  // OUTFLOW_VALVE_OPEN
			}
			else {
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::air_valve_manual_outflow_valve_middle]);  // OUTFLOW_VALVE_MIDDLE
			}
		}

		// pressurization manual valve
		if(mastercard_input(52, &outflowClosed))
		{
			if(outflowClosed == 1)
			{
					aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::air_valve_manual_outflow_valve_close]);  // OUTFLOW_VALVE_CLOSE
			}
			else {

				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::air_valve_manual_outflow_valve_middle]);  // OUTFLOW_VALVE_MIDDLE
			}
		}

		// Flaps Master ARM switch position
		if(mastercard_input(53, &flap_Arm))
		{
			if(flap_Arm == 1)
			{
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::alt_flaps_pos_alternate_flaps_arm]);  // ALTERNATE_FLAPS_ARM
			}
			else
			{
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::alt_flaps_pos_alternate_flaps_off]);  // ALTERNATE_FLAPS_OFF
			}
		}



		// flight control B ON switch position
		const auto fltctrl_b_on_changed = mastercard_input(54, &fltctrl_b_on);
		const auto fltctrl_b_stby_changed = mastercard_input(55, &fltctrl_b_stby);
		if (fltctrl_b_on_changed)
		{
			if (fltctrl_b_on == 1)
			{
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::flt_ctr_b_pos_flight_control_b_on]);  // FLIGHT_CONTROL_B_ON
			}
			else
			{
				if (fltctrl_b_stby != 1)
				{
					aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::flt_ctr_b_pos_flight_control_b_off]);  // FLIGHT_CONTROL_B_OFF
				}
			}
		}
		// flight control B STBY switch position
		if (fltctrl_b_stby_changed)
		{
			if (fltctrl_b_stby == 1)
			{
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::flt_ctr_b_pos_flight_control_b_stbyrud]);  // FLIGHT_CONTROL_B_STBYRUD
			}
			else
			{
				if (fltctrl_b_on != 1)
				{
					aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::flt_ctr_b_pos_flight_control_b_off]);  // FLIGHT_CONTROL_B_OFF
				}
			}
		}



		// flight control A STBY switch position
		const auto fltctrl_a_stby_changed = mastercard_input(56, &fltctrl_a_stby);
		const auto fltctrl_a_on_changed = mastercard_input(57, &fltctrl_a_on);
		if(fltctrl_a_stby_changed)
		{
			if(fltctrl_a_stby == 1)
			{
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::flt_ctr_a_pos_flight_control_a_stbyrud]);  // FLIGHT_CONTROL_A_STBYRUD
		}
			else
			{
				if(fltctrl_a_on != 1)
				{
					aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::flt_ctr_a_pos_flight_control_a_off]);  // FLIGHT_CONTROL_A_OFF
				}
			}
		}
		// flight control A ON switch position
		if(fltctrl_a_on_changed)
		{
			if(fltctrl_a_on == 1)
			{
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::flt_ctr_a_pos_flight_control_a_on]);  // FLIGHT_CONTROL_A_ON
			}
			else
			{
				if(fltctrl_a_stby != 1)
				{
					aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::flt_ctr_a_pos_flight_control_a_off]);  // FLIGHT_CONTROL_A_OFF
				}
			}
		}


		// Alternate Flaps UP switch position
		// Alternate Flaps DOWN switch position
		const auto altFlapDn_changed = mastercard_input(58, &altFlapDn);
		const auto altFlapUp_changed = mastercard_input(59, &altFlapUp);
		if(altFlapDn_changed)
		{
			// Spring loaded
			if(altFlapDn == 1)
			{
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::alt_flaps_ctrl_alternate_flaps_ctrl_dn]);  // ALTERNATE_FLAPS_CTRL_DN
			}
			else
			{
				if(altFlapDn_old != altFlapDn)
				{
					aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::flt_ctr_a_pos_flight_control_a_off]);  // ALTERNATE_FLAPS_CTRL_OFF
				}
			}
		}
		if(altFlapUp_changed)
		{
			if(altFlapUp == 1)
			{
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::alt_flaps_ctrl_alternate_flaps_ctrl_up]);  // ALTERNATE_FLAPS_CTRL_UP
			}
			else
			{
				if(altFlapUp_old != altFlapUp)
				{
					aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::alt_flaps_ctrl_alternate_flaps_ctrl_off]);  // ALTERNATE_FLAPS_CTRL_OFF
				}
			}
		}
		altFlapDn_old = altFlapDn;
		altFlapUp_old = altFlapUp;

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
		const auto displaySrc1_changed = mastercard_input(63, &displaySrc1);
		const auto displaySrcAuto_changed = mastercard_input(64, &displaySrcAuto);
		if(displaySrc1_changed)
		{
			if(displaySrc1 == 1)
			{
				aircraft_model.push_switch_change(		iocard_fwd_overhead_zcockpit_switches[SwitchPosition::dspl_source_instrument_displays_source_1]);  // INSTRUMENT_DISPLAYS_SOURCE_1
			}
			else
			{
				if(displaySrcAuto != 1)
				{
					aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::dspl_source_instrument_displays_source_2]);  // INSTRUMENT_DISPLAYS_SOURCE_2
				}
			}
		}

		// Display Source Auto switch position
		if(displaySrcAuto_changed)
		{
			if(displaySrcAuto == 1)
			{
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::dspl_source_instrument_displays_source_auto]);  // INSTRUMENT_DISPLAYS_SOURCE_AUTO
			}
			else
			{
				if(displaySrc1 != 1)
				{
					aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::dspl_source_instrument_displays_source_2]);  // INSTRUMENT_DISPLAYS_SOURCE_2
				}
			}
		}


		// IRS Both on Right switch position
		const auto irsBothRight_changed = mastercard_input(65, &irsBothRight);
		const auto irsBothLeft_changed = mastercard_input(66, &irsBothLeft);
		if(irsBothRight_changed)
		{
			if(irsBothRight == 1)
			{
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::irs_source_fms_irs_tfr_r]);  // FMS_IRS_TFR_R
			}
			else
			{
				if(irsBothLeft != 1)
				{
					aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::irs_source_fms_irs_tfr_normal]);  // FMS_IRS_TFR_NORMAL
				}
			}
		}

		// IRS Both on Left switch position
		if(irsBothLeft_changed)
		{
			if(irsBothLeft == 1)
			{
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::irs_source_fms_irs_tfr_l]);  // FMS_IRS_TFR_L
			}
			else
			{
				if(irsBothRight != 1)
				{
					aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::irs_source_fms_irs_tfr_normal]);  // FMS_IRS_TFR_NORMAL
				}
			}
		}


		// VHF NAV Both on 1 switch position
		const auto vhfNavBoth1_changed = mastercard_input(67, &vhfNavBoth1);
		const auto vhfNavBoth2_changed = mastercard_input(68, &vhfNavBoth2);
		if(vhfNavBoth1_changed)
		{
			if(vhfNavBoth1 == 1)
			{

				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::vhf_nav_source_fms_vhf_nav_l]);  // FMS_VHF_NAV_L
			}
			else
			{
				if(vhfNavBoth2 != 1)
				{
					aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::vhf_nav_source_fms_vhf_nav_normal]);  // FMS_VHF_NAV_NORMAL
				}
			}
		}
		// VHF NAV Both on 2 switch position
		if(vhfNavBoth2_changed)
		{
			if(vhfNavBoth2 == 1)
			{
				aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::vhf_nav_source_fms_vhf_nav_r]);  // FMS_VHF_NAV_R
			}
			else
			{
				if(vhfNavBoth1 != 1)
				{
					aircraft_model.push_switch_change(iocard_fwd_overhead_zcockpit_switches[SwitchPosition::vhf_nav_source_fms_vhf_nav_normal]);  // FMS_VHF_NAV_NORMAL
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