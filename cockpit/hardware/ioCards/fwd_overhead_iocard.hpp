#pragma once

#include <string>
#include "../../aircraft_model.hpp"
#include "iocards.hpp"

namespace zcockpit::cockpit::hardware
{

	const int NUMBER_OF_ENGINE_SWITCH_STATES = 6;
	const int ENG_START_DEBOUNCE_COUNT = 1;
	const int NUMBER_OF_ALTITUDE_DIGITS = 5;

	enum EngineStartStates
	{
		GND,
		OFF,
		CONT,
		FLT,
	};

	class ForwardOverheadIOCard : public IOCards
	{
	public:
		ForwardOverheadIOCard() = delete;
		explicit ForwardOverheadIOCard(AircraftModel& ac_model, std::string deviceBusAddr);

		[[nodiscard]] static std::unique_ptr<ForwardOverheadIOCard> create_iocard(AircraftModel& ac_model, const std::string& bus_address);

		void fastProcessOvrHead();
		void processOvrHead();
		void update_displays();

		void update_electrical_display();

		void update_landing_alt_display();

		void update_flight_alt_display();

		static bool is_display_blank(std::array<unsigned char, NUMBER_OF_ALTITUDE_DIGITS> digits);


		static long convert_digits_to_long(std::array<unsigned char, NUMBER_OF_ALTITUDE_DIGITS> digits);
		void processEncoders();
		int mag_eng1_start;
		int mag_eng2_start;

		static bool is_running(){return running;}
		static std::string get_bus_addr(){return iocard_bus_addr;}
	private:
		AircraftModel& aircraft_model;

		static std::string iocard_bus_addr;
		static bool running; 

		int compMode_altn;
		int compMode_auto;
		int compMode_man;
		int pos_steady;
		int pos_strobe;
		int outflowOpen;
		int outflowClosed;
		int dc_stbyPwr;
		int dc_batBus;
		int dc_bat;
		int dc_auxBat;
		int dc_tr1;
		int dc_tr2;
		int dc_tr3;
		int dc_test;
		int disconnect_1;
		int elec_maint;
		int ac_stbyPwr;
		int ac_gndPwr;
		int ac_gen1;
		int ac_gen2;
		int ac_apuGen;
		int ac_test;
		int ac_inv;
		int start_1_gnd;
		int start_1_cont;
		int start_1_flt;
		int start_2_gnd;
		int start_2_cont;
		int start_2_flt;
		int wing_aitiIce;
		int eng1_antiIce;
		int eng2_antiIce;
		int lRecirc_fan;
		int rRecirc_fan;
		int leftPack_off;
		int left_pack_high;
		int isolationClose;
		int isolationOpen;
		int right_pack_off;
		int right_pack_hi;
		int eng_1_bleed;
		int apu_bleed;
		int eng_2_bleed;
		int ovht_test;
		int trip_reset;
		int fltctrl_a_stby;
		int fltctrl_a_on;
		int fltctrl_b_stby;
		int fltctrl_b_on;
		int flap_Arm;
		int altFlapUp;
		int altFlapDn;
		int spoiler_a;
		int spoiler_b;
		int wiper_l_pk;
		int wiper_l_int;
		int wiper_l_low;
		int wiper_r_pk;
		int wiper_r_int;
		int wiper_r_low;
		int vhfNavBoth1;
		int vhfNavBoth2;
		int irsBothLeft;
		int irsBothRight;
		int displaySrc1;
		int displaySrcAuto;


		masterCard_input_state eng1_start_gnd;
		masterCard_input_state eng1_start_cont;
		masterCard_input_state eng1_start_flt;

		masterCard_input_state eng2_start_gnd;
		masterCard_input_state eng2_start_cont;
		masterCard_input_state eng2_start_flt;

		masterCard_input_state* engine_start_switches[NUMBER_OF_ENGINE_SWITCH_STATES];
		int eng1_start_debounce;
		int eng2_start_debounce;
		EngineStartStates engine1_state;
		EngineStartStates engine2_state;
	};
}
