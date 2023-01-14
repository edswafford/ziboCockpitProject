#pragma once

#include <string>
#include "../../aircraft_model.hpp"
#include "iocards.hpp"
#include "fwd_overhead_init_data.hpp"

namespace zcockpit::cockpit::hardware
{

	constexpr int NUMBER_OF_ENGINE_SWITCH_STATES = 6;
	constexpr int ENG_START_DEBOUNCE_COUNT = 1;
	constexpr int NUMBER_OF_ALTITUDE_DIGITS = 5;


	class ForwardOverheadIOCard : public IOCards
	{
	public:
		ForwardOverheadIOCard() = delete;
		explicit ForwardOverheadIOCard(AircraftModel& ac_model, std::string deviceBusAddr);

		[[nodiscard]] static std::unique_ptr<ForwardOverheadIOCard> create_iocard(AircraftModel& ac_model, const std::string& bus_address);

		void process_overhead();
		void update_displays();

		void update_electrical_display();

		void update_landing_alt_display(bool use_xplane_value = true);

		void update_flight_alt_display(bool use_xplane_value = true);


		void update_encoders();
		void process_encoders();

		int mag_eng1_start{0};
		int mag_eng2_start{0};

		int previous_ac_volts{-1};
		int previous_ac_freq{-1};
		int previous_dc_amps{-1};
		int previous_ac_amps{-1};
		int previous_dc_volts{-1};
		long previous_landing_altitude{-1};
		long previous_flight_altitude{-1};
		long flight_altitude{0};
		long landing_altitude{0};
		bool fresh_flight_altitude{ false };
		bool fresh_landing_altitude{ false };
		int spoiler_b{0};
		int acdc_maint{0};
		int spoiler_a{0};
		int wing_heat{0};
		int eng1_heat{0};
		int duct_ovht_test{0};
		int r_recirc_fan{0};
		int r_pack_off{0};
		int r_pack_high{0};
		int isolation_valve_open{0};
		int isolation_valve_closed{0};
		int l_pack_off{0};
		int l_pack_high{0};


		static bool is_running(){return running;}
		static std::string get_bus_addr(){return iocard_bus_addr;}
	private:

		void initialize_switches();

		AircraftModel& aircraft_model;

		static std::string iocard_bus_addr;
		static bool running; 


		int outflowOpen{0};
		int outflowClosed{0};
		int flap_Arm{0};
		int fltctrl_a_stby{0};
		int fltctrl_a_on{0};
		int fltctrl_b_stby{0};
		int fltctrl_b_on{0};
		int altFlapUp{0};
		int altFlapDn{0};
		int altFlapUp_old{ -1 };
		int altFlapDn_old{ 0 };
		int displaySrc1{0};
		int displaySrcAuto{0};
		int vhfNavBoth1{0};
		int vhfNavBoth2{0};
		int irsBothLeft{0};
		int irsBothRight{0};
		int disconnect_1{0};

				//int wiper_l_pk{0};
				//int wiper_l_int{0};
				//int wiper_l_low{0};
				//int wiper_r_pk{0};
				//int wiper_r_int{0};
				//int wiper_r_low{0};



		masterCard_input_state eng1_start_gnd {27, 0, false};
		masterCard_input_state eng1_start_cont{28, 0, false};
		masterCard_input_state eng1_start_flt {29, 0, false};

		masterCard_input_state eng2_start_gnd {30, 0, false};
		masterCard_input_state eng2_start_cont{31, 0, false};
		masterCard_input_state eng2_start_flt {32, 0, false};

		masterCard_input_state* engine_start_switches[NUMBER_OF_ENGINE_SWITCH_STATES]
		{
			 &eng1_start_gnd,
			 &eng1_start_cont,
			 &eng1_start_flt,
			 &eng2_start_gnd,
			 &eng2_start_cont,
			 &eng2_start_flt,
		};

		int eng1_start_debounce{ENG_START_DEBOUNCE_COUNT};
		int eng2_start_debounce{ENG_START_DEBOUNCE_COUNT};
		int engine1_state{-1};
		int old_engine1_state{ -1 };
		int engine2_state{-1};
		int old_engine2_state{ -1 };

		common::EnumArray<SwitchPosition, ZcockpitSwitch> iocard_fwd_overhead_zcockpit_switches;

	};
}
