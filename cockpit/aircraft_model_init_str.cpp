#include "aircraft_model.hpp"

#include "xp_data_ref_repository.hpp"
#include "hardware/zcockpit_switch.hpp"



namespace zcockpit::cockpit {

	using namespace common;




	void AircraftModel::initialize_ref_strings()
	{
		// Annunicators value will degrade from 1.0 toward 0.0 as the battery drains
		// The harware needs integer values (0/1) to turn off/on, so as soon as the battery start discharging all annunicators
		// would go off.  Therefore, the values need to be rounded.
		constexpr bool round_up = true;
		//
		// SWITCHES
		data_ref_strings[DataRefName::battery_pos] = { "laminar/B738/electric/battery_pos", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::bright_test] = { "laminar/B738/toggle_switch/bright_test", XplaneType::type_Float, !round_up}; // [1, 0, -1]  up==test, center==brt, dn==dim
		data_ref_strings[DataRefName::spd_ref] = { "laminar/B738/toggle_switch/spd_ref", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::autobrake_pos] = { "laminar/B738/autobrake/autobrake_pos", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::landing_gear] = { "laminar/B738/controls/gear_handle_down", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::fuel_flow_pos] = {"laminar/B738/toggle_switch/fuel_flow_pos", XplaneType::type_Float, !round_up};


		data_ref_strings[DataRefName::ap_discon_test1] = {"laminar/B738/toggle_switch/ap_discon_test1", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::ap_discon_test2] = {"laminar/B738/toggle_switch/ap_discon_test2", XplaneType::type_Float, !round_up};

		data_ref_strings[DataRefName::guarded_covers] = { "laminar/B738/guarded_covers", XplaneType::type_Float, !round_up};

		data_ref_strings[DataRefName::ap_light_pilot] = {"laminar/B738/push_button/ap_light_pilot", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::at_light_pilot] = {"laminar/B738/push_button/at_light_pilot", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::fms_light_pilot] = {"laminar/B738/push_button/fms_light_pilot", XplaneType::type_Float, !round_up};

		data_ref_strings[DataRefName::ap_light_fo] = {"laminar/B738/push_button/ap_light_fo", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::at_light_fo] = {"laminar/B738/push_button/at_light_fo", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::fms_light_fo] = {"laminar/B738/push_button/fms_light_fo", XplaneType::type_Float, !round_up};


		data_ref_strings[DataRefName::rudder_trim] = {"sim/flightmodel2/controls/rudder_trim", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::aileron_trim] = {"sim/flightmodel2/controls/aileron_trim", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::instrument_brightness] = {"laminar/B738/electric/instrument_brightness", XplaneType::type_FloatArray, !round_up};
		data_ref_strings[DataRefName::spd_ref_adjust] = {"laminar/B738/toggle_switch/spd_ref_adjust", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::n1_set_adjust] = {"laminar/B738/toggle_switch/n1_set_adjust", XplaneType::type_Float, !round_up};


		data_ref_strings[DataRefName::land_lights_ret_left_pos] = { "laminar/B738/switch/land_lights_ret_left_pos", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::land_lights_ret_right_pos] = { "laminar/B738/switch/land_lights_ret_right_pos", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::land_lights_left_pos] = { "laminar/B738/switch/land_lights_left_pos", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::land_lights_right_pos] = { "laminar/B738/switch/land_lights_right_pos", XplaneType::type_Float, !round_up};

		data_ref_strings[DataRefName::rwy_light_left] = { "laminar/B738/toggle_switch/rwy_light_left", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::rwy_light_right] = { "laminar/B738/toggle_switch/rwy_light_right", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::taxi_light_brightness_pos] = { "laminar/B738/toggle_switch/taxi_light_brightness_pos", XplaneType::type_Float, !round_up};  // off=0 on=2
		data_ref_strings[DataRefName::APU_start_pos] = { "laminar/B738/spring_toggle_switch/APU_start_pos", XplaneType::type_Float, !round_up};		// off=0, on==1 start==2
		data_ref_strings[DataRefName::hydro_pumps1_pos] = { "laminar/B738/toggle_switch/hydro_pumps1_pos", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::hydro_pumps2_pos] = { "laminar/B738/toggle_switch/hydro_pumps2_pos", XplaneType::type_Float, !round_up};

		//
		// *** NOTE *** Zibo has a typo the name for pumps 1 and 2 swapped
		data_ref_strings[DataRefName::electric_hydro_pumps1_pos] = { "laminar/B738/toggle_switch/electric_hydro_pumps2_pos", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::electric_hydro_pumps2_pos] = { "laminar/B738/toggle_switch/electric_hydro_pumps1_pos", XplaneType::type_Float, !round_up};

		data_ref_strings[DataRefName::eng_start_source] = { "laminar/B738/toggle_switch/eng_start_source", XplaneType::type_Float, !round_up};		// left -1 both=0 right=1
		data_ref_strings[DataRefName::fmc_source] = { "laminar/B738/toggle_switch/fmc_source", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::logo_light] = { "laminar/B738/toggle_switch/logo_light", XplaneType::type_Float, !round_up};

		data_ref_strings[DataRefName::beacon_on] = { "sim/cockpit2/switches/beacon_on", XplaneType::type_Int, !round_up};						// anti-collision -- no command use dataref on=1 off=0

		data_ref_strings[DataRefName::wing_light] = { "laminar/B738/toggle_switch/wing_light", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::wheel_light] = { "laminar/B738/toggle_switch/wheel_light", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::eq_cool_supply] = { "laminar/B738/toggle_switch/eq_cool_supply", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::eq_cool_exhaust] = { "laminar/B738/toggle_switch/eq_cool_exhaust", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::emer_exit_lights] = { "laminar/B738/toggle_switch/emer_exit_lights", XplaneType::type_Float, !round_up};		// off=0 armed=1 on =2
		data_ref_strings[DataRefName::seatbelt_sign_pos] = { "laminar/B738/toggle_switch/seatbelt_sign_pos", XplaneType::type_Float, !round_up};	// off=0 auto=1 on=2
		data_ref_strings[DataRefName::no_smoking_pos] = { "laminar/B738/toggle_switch/no_smoking_pos", XplaneType::type_Float, !round_up};			// off=0 auto=1 on=2

		data_ref_strings[DataRefName::attend] = { "laminar/B738/push_button/attend_pos", XplaneType::type_Float, !round_up};			
		data_ref_strings[DataRefName::grd_call] = { "laminar/B738/push_button/grd_call_pos", XplaneType::type_Float, !round_up};			

		data_ref_strings[DataRefName::cross_feed_pos] = { "laminar/B738/knobs/cross_feed_pos", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::fuel_tank_pos_lft1] = { "laminar/B738/fuel/fuel_tank_pos_lft1", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::fuel_tank_pos_lft2] = { "laminar/B738/fuel/fuel_tank_pos_lft2", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::fuel_tank_pos_rgt2] = { "laminar/B738/fuel/fuel_tank_pos_rgt2", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::fuel_tank_pos_rgt1] = { "laminar/B738/fuel/fuel_tank_pos_rgt1", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::fuel_tank_pos_ctr1] = { "laminar/B738/fuel/fuel_tank_pos_ctr1", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::fuel_tank_pos_ctr2] = { "laminar/B738/fuel/fuel_tank_pos_ctr2", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::yaw_dumper_pos] = { "laminar/B738/toggle_switch/yaw_dumper_pos", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::cab_util_pos] = { "laminar/B738/toggle_switch/cab_util_pos", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::standby_bat_pos] = { "laminar/B738/electric/standby_bat_pos", XplaneType::type_Float, !round_up};				//bat=-1 off=0 auto=1 
		data_ref_strings[DataRefName::gpu_pos] = { "laminar/B738/electrical/gpu_pos", XplaneType::type_Float, !round_up};							// up=-1 center=0 dn =1
		data_ref_strings[DataRefName::gen1_pos] = { "laminar/B738/electrical/gen1_pos", XplaneType::type_Float, !round_up};							// up=-1 center=0 dn =1
		data_ref_strings[DataRefName::gen2_pos] = { "laminar/B738/electrical/gen2_pos", XplaneType::type_Float, !round_up};							// up=-1 center=0 dn =1
		data_ref_strings[DataRefName::apu_gen1_pos] = { "laminar/B738/electrical/apu_gen1_pos", XplaneType::type_Float, !round_up};					// up=-1 center=0 dn =1
		data_ref_strings[DataRefName::apu_gen2_pos] = { "laminar/B738/electrical/apu_gen2_pos", XplaneType::type_Float, !round_up};					// up=-1 center=0 dn =1
		data_ref_strings[DataRefName::window_heat_l_side_pos] = { "laminar/B738/ice/window_heat_l_side_pos", XplaneType::type_Float, !round_up};	// on=1 off=0
		data_ref_strings[DataRefName::window_heat_l_fwd_pos] = { "laminar/B738/ice/window_heat_l_fwd_pos", XplaneType::type_Float, !round_up};		// on=1 off=0
		data_ref_strings[DataRefName::window_heat_r_fwd_pos] = { "laminar/B738/ice/window_heat_r_fwd_pos", XplaneType::type_Float, !round_up};		// on=1 off=0
		data_ref_strings[DataRefName::window_heat_r_side_pos] = { "laminar/B738/ice/window_heat_r_side_pos", XplaneType::type_Float, !round_up};	// on=1 off=0
		data_ref_strings[DataRefName::window_ovht_test] = { "laminar/B738/toggle_switch/window_ovht_test", XplaneType::type_Float, !round_up};		// dn=1 up=-1
		data_ref_strings[DataRefName::capt_probes_pos] = { "laminar/B738/toggle_switch/capt_probes_pos", XplaneType::type_Float, !round_up};		// on=1 off=0
		data_ref_strings[DataRefName::fo_probes_pos] = { "laminar/B738/toggle_switch/fo_probes_pos", XplaneType::type_Float, !round_up};			// on=1 off=0
		data_ref_strings[DataRefName::main_pnl_du_capt] = {"laminar/B738/toggle_switch/main_pnl_du_capt", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::main_pnl_du_fo] = {"laminar/B738/toggle_switch/main_pnl_du_fo", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::lower_du_capt] = {"laminar/B738/toggle_switch/lower_du_capt", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::lower_du_fo] = {"laminar/B738/toggle_switch/lower_du_fo", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::n1_set_source] = { "laminar/B738/toggle_switch/n1_set_source", XplaneType::type_Float, !round_up};
//NEW
		data_ref_strings[DataRefName::air_valve_ctrl] = {"laminar/B738/toggle_switch/air_valve_ctrl", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::dc_power] = {"laminar/B738/knob/dc_power", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::ac_power] = {"laminar/B738/knob/ac_power", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::l_pack_pos] = {"laminar/B738/air/l_pack_pos", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::r_pack_pos] = {"laminar/B738/air/r_pack_pos", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::isolation_valve_pos] = {"laminar/B738/air/isolation_valve_pos", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::position_light_pos] = {"laminar/B738/toggle_switch/position_light_pos", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::irs_left] = {"laminar/B738/toggle_switch/irs_left", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::irs_right] = {"laminar/B738/toggle_switch/irs_right", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::irs_dspl_sel] = {"laminar/B738/toggle_switch/irs_dspl_sel", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::starter1_pos] = {"laminar/B738/engine/starter1_pos", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::starter2_pos] = {"laminar/B738/engine/starter2_pos", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::air_valve_manual] = {"laminar/B738/toggle_switch/air_valve_manual", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::flt_ctr_B_pos] = {"laminar/B738/switches/flt_ctr_B_pos", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::flt_ctr_A_pos] = {"laminar/B738/switches/flt_ctr_A_pos", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::alt_flaps_ctrl] = {"laminar/B738/toggle_switch/alt_flaps_ctrl", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::dspl_source] = {"laminar/B738/toggle_switch/dspl_source", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::irs_source] = {"laminar/B738/toggle_switch/irs_source", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::vhf_nav_source] = {"laminar/B738/toggle_switch/vhf_nav_source", XplaneType::type_Float, !round_up};



		data_ref_strings[DataRefName::spoiler_A_pos] = {"laminar/B738/switches/spoiler_A_pos", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::spoiler_B_pos] = {"laminar/B738/switches/spoiler_B_pos", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::wing_heat_pos] = {"laminar/B738/ice/wing_heat_pos", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::eng1_heat_pos] = {"laminar/B738/ice/eng1_heat_pos", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::eng2_heat_pos] = {"laminar/B738/ice/eng2_heat_pos", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::l_recirc_fan_pos] = {"laminar/B738/air/l_recirc_fan_pos", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::r_recirc_fan_pos] = {"laminar/B738/air/r_recirc_fan_pos", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::bleed_air_1_pos] = {"laminar/B738/toggle_switch/bleed_air_1_pos", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::bleed_air_2_pos] = {"laminar/B738/toggle_switch/bleed_air_2_pos", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::irs_sys_dspl] = {"laminar/B738/toggle_switch/irs_sys_dspl", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::pax_oxy_pos] = {"laminar/B738/one_way_switch/pax_oxy_pos", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::drive_disconnect1_pos] = {"laminar/B738/one_way_switch/drive_disconnect1_pos", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::drive_disconnect2_pos] = {"laminar/B738/one_way_switch/drive_disconnect2_pos", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::alt_flaps_pos] = {"laminar/B738/switches/alt_flaps_pos", XplaneType::type_Float, !round_up};


		//data_ref_strings[DataRefName::] = {"", XplaneType::type_Float, !round_up};
		//data_ref_strings[DataRefName::] = {"", XplaneType::type_Float, !round_up};
		//data_ref_strings[DataRefName::] = {"", XplaneType::type_Float, !round_up};
		//data_ref_strings[DataRefName::] = {"", XplaneType::type_Float, !round_up};
		//data_ref_strings[DataRefName::] = {"", XplaneType::type_Float, !round_up};








		//
		// Pushbuttons -- NOT Used
		//

		// Capt
		//data_ref_strings[DataRefName::master_caution1_pb] ={"master_caution1_pb", XplaneType::type_Float, !round_up};
		//data_ref_strings[DataRefName::capt_six_pack_pb] = {"capt_six_pack_pb", XplaneType::type_Float, !round_up};
		//data_ref_strings[DataRefName::chrono_capt_et_mode_pb] = {"chrono_capt_et_mode_pb", XplaneType::type_Float, !round_up};
		//data_ref_strings[DataRefName::fire_bell_light1_pb] = {"fire_bell_light1_pb", XplaneType::type_Float, !round_up};
		//// FO
		//data_ref_strings[DataRefName::master_caution2_pb] ={"master_caution2_pb", XplaneType::type_Float, !round_up};
		//data_ref_strings[DataRefName::fo_six_pack_pb] = {"fo_six_pack_pb", XplaneType::type_Float, !round_up};
		//data_ref_strings[DataRefName::chrono_fo_et_mode_pb] = {"chrono_fo_et_mode_pb", XplaneType::type_Float, !round_up};
		//data_ref_strings[DataRefName::fire_bell_light2_pb] = {"fire_bell_light2_pb", XplaneType::type_Float, !round_up};


		//data_ref_strings[DataRefName::MFD_ENG_pb] = {"MFD_ENG_pb", XplaneType::type_Float, !round_up};
		//data_ref_strings[DataRefName::MFD_SYS_pb] = {"MFD_SYS_pb", XplaneType::type_Float, !round_up};

		// INPUTS
		data_ref_strings[DataRefName::ac_volt_value] = {"laminar/B738/ac_volt_value", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::dc_volt_value] = {"laminar/B738/dc_volt_value", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::ac_freq_value] = {"laminar/B738/ac_freq_value", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::dc_amp_value] = {"laminar/B738/dc_amp_value", XplaneType::type_Float, !round_up};
		data_ref_strings[DataRefName::ac_amp_value] = {"laminar/B738/ac_amp_value", XplaneType::type_Float, !round_up};

			//data_ref_strings[DataRefName::] = {"", XplaneType::type_Float, !round_up};
		//data_ref_strings[DataRefName::] = {"", XplaneType::type_Float, !round_up};
		//data_ref_strings[DataRefName::] = {"", XplaneType::type_Float, !round_up};
		//data_ref_strings[DataRefName::] = {"", XplaneType::type_Float, !round_up};


		// Annunciators
		data_ref_strings[DataRefName::acf_tailnum] = { "sim/aircraft/view/acf_tailnum", XplaneType::type_String };
		data_ref_strings[DataRefName::crossfeed] = { "laminar/B738/annunciator/crossfeed", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::engine1_ovht] = { "laminar/B738/annunciator/engine1_ovht", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::engine2_ovht] = { "laminar/B738/annunciator/engine2_ovht", XplaneType::type_Float, round_up};

		data_ref_strings[DataRefName::cabin_alt] = { "laminar/B738/annunciator/cabin_alt", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::hyd_A_rud] = { "laminar/B738/annunciator/hyd_A_rud", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::hyd_B_rud] = { "laminar/B738/annunciator/hyd_B_rud", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::takeoff_config] = { "laminar/B738/annunciator/takeoff_config", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::gps] = { "laminar/B738/annunciator/gps", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::pax_oxy] = { "laminar/B738/annunciator/pax_oxy", XplaneType::type_Float, round_up};

		data_ref_strings[DataRefName::left_gear_safe] = { "laminar/B738/annunciator/left_gear_safe", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::nose_gear_safe] = { "laminar/B738/annunciator/nose_gear_safe", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::right_gear_safe] = { "laminar/B738/annunciator/right_gear_safe", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::left_gear_transit] = { "laminar/B738/annunciator/left_gear_transit", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::nose_gear_transit] = { "laminar/B738/annunciator/nose_gear_transit", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::right_gear_transit] = { "laminar/B738/annunciator/right_gear_transit", XplaneType::type_Float, round_up};

		data_ref_strings[DataRefName::yaw_damp] = { "laminar/B738/annunciator/yaw_damp", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::hyd_stdby_rud] = { "laminar/B738/annunciator/hyd_stdby_rud", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::feel_diff_press] = { "laminar/B738/annunciator/feel_diff_press", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::auto_slat_fail] = { "laminar/B738/annunciator/auto_slat_fail", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::bat_discharge] = { "laminar/B738/annunciator/bat_discharge", XplaneType::type_Float, round_up};

		data_ref_strings[DataRefName::tr_unit] = { "laminar/B738/annunciator/tr_unit", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::elec] = { "laminar/B738/annunciator/elec", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::standby_pwr_off] = { "laminar/B738/annunciator/standby_pwr_off", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::ground_power_avail] = { "laminar/B738/annunciator/ground_power_avail", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::apu_gen_off_bus] = { "laminar/B738/annunciator/apu_gen_off_bus", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::apu_low_oil] = { "laminar/B738/annunciator/apu_low_oil", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::apu_fault] = { "laminar/B738/annunciator/apu_fault", XplaneType::type_Float, round_up};

		data_ref_strings[DataRefName::emer_exit] = { "laminar/B738/annunciator/emer_exit", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::capt_pitot_off] = { "laminar/B738/annunciator/capt_pitot_off", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::fo_pitot_off] = { "laminar/B738/annunciator/fo_pitot_off", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::dual_bleed] = { "laminar/B738/annunciator/dual_bleed", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::ram_door_open1] = { "laminar/B738/annunciator/ram_door_open1", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::ram_door_open2] = { "laminar/B738/annunciator/ram_door_open2", XplaneType::type_Float, round_up};

		data_ref_strings[DataRefName::six_pack_flt_cont] = { "laminar/B738/annunciator/six_pack_flt_cont", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::six_pack_irs] = { "laminar/B738/annunciator/six_pack_irs", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::six_pack_fuel] = { "laminar/B738/annunciator/six_pack_fuel", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::six_pack_elec] = { "laminar/B738/annunciator/six_pack_elec", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::six_pack_apu] = { "laminar/B738/annunciator/six_pack_apu", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::six_pack_fire] = { "laminar/B738/annunciator/six_pack_fire", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::six_pack_ice] = { "laminar/B738/annunciator/six_pack_ice", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::six_pack_hyd] = { "laminar/B738/annunciator/six_pack_hyd", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::six_pack_doors] = { "laminar/B738/annunciator/six_pack_doors", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::six_pack_eng] = { "laminar/B738/annunciator/six_pack_eng", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::six_pack_overhead] = { "laminar/B738/annunciator/six_pack_overhead", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::six_pack_air_cond] = { "laminar/B738/annunciator/six_pack_air_cond", XplaneType::type_Float, round_up};

		data_ref_strings[DataRefName::speedbrake_armed] = { "laminar/B738/annunciator/speedbrake_armed", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::spd_brk_not_arm] = { "laminar/B738/annunciator/spd_brk_not_arm", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::speedbrake_extend] = { "laminar/B738/annunciator/speedbrake_extend", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::stab_out_of_trim] = { "laminar/B738/annunciator/stab_out_of_trim", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::anti_skid_inop] = { "laminar/B738/annunciator/anti_skid_inop", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::auto_brake_disarm] = { "laminar/B738/annunciator/auto_brake_disarm", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::slats_transit] = { "laminar/B738/annunciator/slats_transit", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::slats_extend] = { "laminar/B738/annunciator/slats_extend", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::gpws] = { "laminar/B738/annunciator/gpws", XplaneType::type_Float, round_up};

		data_ref_strings[DataRefName::parking_brake] = { "laminar/B738/annunciator/parking_brake", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::wheel_well_fire] = { "laminar/B738/annunciator/wheel_well_fire", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::fire_fault_inop] = { "laminar/B738/annunciator/fire_fault_inop", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::extinguisher_circuit_annun_apu] = { "laminar/B738/annunciator/extinguisher_circuit_annun_apu", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::apu_bottle_discharge] = { "laminar/B738/annunciator/apu_bottle_discharge", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::cargo_fault_detector] = { "laminar/B738/annunciator/cargo_fault_detector", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::cargo_fire] = { "laminar/B738/annunciator/cargo_fire", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::xpond_fail] = { "laminar/B738/transponder/indicators/xpond_fail", XplaneType::type_Float, round_up};

		data_ref_strings[DataRefName::fwd_entry] = { "laminar/B738/annunciator/fwd_entry", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::aft_entry] = { "laminar/B738/annunciator/aft_entry", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::fwd_service] = { "laminar/B738/annunciator/fwd_service", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::aft_service] = { "laminar/B738/annunciator/aft_service", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::airstair] = { "laminar/B738/annunciator/airstair", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::left_fwd_overwing] = { "laminar/B738/annunciator/left_fwd_overwing", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::right_fwd_overwing] = { "laminar/B738/annunciator/right_fwd_overwing", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::left_aft_overwing] = { "laminar/B738/annunciator/left_aft_overwing", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::right_aft_overwing] = { "laminar/B738/annunciator/right_aft_overwing", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::fwd_cargo] = { "laminar/B738/annunciator/fwd_cargo", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::aft_cargo] = { "laminar/B738/annunciator/aft_cargo", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::equip_door] = { "laminar/B738/annunciator/equip_door", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::door_lock_fail] = { "laminar/B738/annunciator/door_lock_fail", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::door_auto_unlk] = { "laminar/B738/annunciator/door_auto_unlk", XplaneType::type_Float, round_up};

		data_ref_strings[DataRefName::autofail] = { "laminar/B738/annunciator/autofail", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::off_sched_descent] = { "laminar/B738/annunciator/off_sched_descent", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::altn_press] = { "laminar/B738/annunciator/altn_press", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::manual_press] = { "laminar/B738/annunciator/manual_press", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::irs_align_left] = { "laminar/B738/annunciator/irs_align_left", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::irs_align_right] = { "laminar/B738/annunciator/irs_align_right", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::irs_on_dc_left] = { "laminar/B738/annunciator/irs_on_dc_left", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::irs_on_dc_right] = { "laminar/B738/annunciator/irs_on_dc_right", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::irs_align_fail_left] = { "laminar/B738/annunciator/irs_align_fail_left", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::irs_align_fail_right] = { "laminar/B738/annunciator/irs_align_fail_right", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::irs_dc_fail_left] = { "laminar/B738/annunciator/irs_dc_fail_left", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::irs_dc_fail_right] = { "laminar/B738/annunciator/irs_dc_fail_right", XplaneType::type_Float, round_up};

		data_ref_strings[DataRefName::reverser_fail_0] = { "laminar/B738/annunciator/reverser_fail_0", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::reverser_fail_1] = { "laminar/B738/annunciator/reverser_fail_1", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::fadec_fail_0] = { "laminar/B738/annunciator/fadec_fail_0", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::fadec_fail_1] = { "laminar/B738/annunciator/fadec_fail_1", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::eng1_valve_closed] = { "laminar/B738/annunciator/eng1_valve_closed", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::eng2_valve_closed] = { "laminar/B738/annunciator/eng2_valve_closed", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::spar1_valve_closed] = { "laminar/B738/annunciator/spar1_valve_closed", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::spar2_valve_closed] = { "laminar/B738/annunciator/spar2_valve_closed", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::bypass_filter_1] = { "laminar/B738/annunciator/bypass_filter_1", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::bypass_filter_2] = { "laminar/B738/annunciator/bypass_filter_2", XplaneType::type_Float, round_up};

		data_ref_strings[DataRefName::low_fuel_press_l1] = { "laminar/B738/annunciator/low_fuel_press_l1", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::low_fuel_press_r1] = { "laminar/B738/annunciator/low_fuel_press_r1", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::low_fuel_press_l2] = { "laminar/B738/annunciator/low_fuel_press_l2", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::low_fuel_press_r2] = { "laminar/B738/annunciator/low_fuel_press_r2", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::low_fuel_press_c1] = { "laminar/B738/annunciator/low_fuel_press_c1", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::low_fuel_press_c2] = { "laminar/B738/annunciator/low_fuel_press_c2", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::trans_bus_off1] = { "laminar/B738/annunciator/trans_bus_off1", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::trans_bus_off2] = { "laminar/B738/annunciator/trans_bus_off2", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::source_off1] = { "laminar/B738/annunciator/source_off1", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::source_off2] = { "laminar/B738/annunciator/source_off2", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::gen_off_bus1] = { "laminar/B738/annunciator/gen_off_bus1", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::gen_off_bus2] = { "laminar/B738/annunciator/gen_off_bus2", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::drive1] = { "laminar/B738/annunciator/drive1", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::drive2] = { "laminar/B738/annunciator/drive2", XplaneType::type_Float, round_up};

		data_ref_strings[DataRefName::window_heat_ovht_ls] = { "laminar/B738/annunciator/window_heat_ovht_ls", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::window_heat_ovht_lf] = { "laminar/B738/annunciator/window_heat_ovht_lf", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::window_heat_ovht_rf] = { "laminar/B738/annunciator/window_heat_ovht_rf", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::window_heat_ovht_rs] = { "laminar/B738/annunciator/window_heat_ovht_rs", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::window_heat_l_fwd] = { "laminar/B738/annunciator/window_heat_l_fwd", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::window_heat_r_fwd] = { "laminar/B738/annunciator/window_heat_r_fwd", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::window_heat_l_side] = { "laminar/B738/annunciator/window_heat_l_side", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::window_heat_r_side] = { "laminar/B738/annunciator/window_heat_r_side", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::wing_ice_on_L] = { "laminar/B738/annunciator/wing_ice_on_L", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::wing_ice_on_R] = { "laminar/B738/annunciator/wing_ice_on_R", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::cowl_ice_0] = { "laminar/B738/annunciator/cowl_ice_0", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::cowl_ice_1] = { "laminar/B738/annunciator/cowl_ice_1", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::cowl_ice_on_0] = { "laminar/B738/annunciator/cowl_ice_on_0", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::cowl_ice_on_1] = { "laminar/B738/annunciator/cowl_ice_on_1", XplaneType::type_Float, round_up};

		data_ref_strings[DataRefName::hyd_press_a] = { "laminar/B738/annunciator/hyd_press_a", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::hyd_press_b] = { "laminar/B738/annunciator/hyd_press_b", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::hyd_el_press_a] = { "laminar/B738/annunciator/hyd_el_press_a", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::hyd_el_press_b] = { "laminar/B738/annunciator/hyd_el_press_b", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::pack_left] = { "laminar/B738/annunciator/pack_left", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::pack_right] = { "laminar/B738/annunciator/pack_right", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::wing_body_ovht_left] = { "laminar/B738/annunciator/wing_body_ovht_left", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::wing_body_ovht_right] = { "laminar/B738/annunciator/wing_body_ovht_right", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::bleed_trip_1] = { "laminar/B738/annunciator/bleed_trip_1", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::bleed_trip_2] = { "laminar/B738/annunciator/bleed_trip_2", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::engine1_fire] = { "laminar/B738/annunciator/engine1_fire", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::engine2_fire] = { "laminar/B738/annunciator/engine2_fire", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::master_caution_light] = { "laminar/B738/annunciator/master_caution_light", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::below_gs] = { "laminar/B738/annunciator/below_gs", XplaneType::type_Float, round_up};

		data_ref_strings[DataRefName::ap_disconnect1] = { "laminar/B738/annunciator/ap_disconnect1", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::ap_disconnect2] = { "laminar/B738/annunciator/ap_disconnect2", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::at_disconnect1] = { "laminar/B738/annunciator/at_disconnect1", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::at_disconnect2] = { "laminar/B738/annunciator/at_disconnect2", XplaneType::type_Float, round_up};


		data_ref_strings[DataRefName::ap_warn1] = { "laminar/B738/annunciator/ap_warn1", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::ap_warn2] = { "laminar/B738/annunciator/ap_warn2", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::at_fms_warn1] = { "laminar/B738/annunciator/at_fms_warn1", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::at_fms_warn2] = { "laminar/B738/annunciator/at_fms_warn2", XplaneType::type_Float, round_up};

		data_ref_strings[DataRefName::at_fms_disconnect1] = { "laminar/B738/annunciator/at_fms_disconnect1", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::at_fms_disconnect2] = { "laminar/B738/annunciator/at_fms_disconnect2", XplaneType::type_Float, round_up};


		data_ref_strings[DataRefName::fms_exec_light_pilot] = { "laminar/B738/indicators/fms_exec_light_pilot", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::fms_exec_light_copilot] = { "laminar/B738/indicators/fms_exec_light_copilot", XplaneType::type_Float, round_up};

		data_ref_strings[DataRefName::ac_tnsbus1_status] = {"laminar/B738/electric/ac_tnsbus1_status", XplaneType::type_Float, round_up};
		data_ref_strings[DataRefName::ac_tnsbus2_status] = {"laminar/B738/electric/ac_tnsbus2_status", XplaneType::type_Float, round_up};


		//
		// End ZBO datarefs
		//


		//
		// COMMAND Refs
		//
		command_ref_strings[CommandRefName::battery_dn] = { "laminar/B738/switch/battery_dn" };
		command_ref_strings[CommandRefName::battery_up] = { "laminar/B738/switch/battery_up" };
		command_ref_strings[CommandRefName::bright_test_dn] = { "laminar/B738/toggle_switch/bright_test_dn" };
		command_ref_strings[CommandRefName::bright_test_up] = { "laminar/B738/toggle_switch/bright_test_up" };
		command_ref_strings[CommandRefName::spd_ref_left] = { "laminar/B738/toggle_switch/spd_ref_left" };
		command_ref_strings[CommandRefName::spd_ref_right] = { "laminar/B738/toggle_switch/spd_ref_right" };

		command_ref_strings[CommandRefName::autobrake_1]  = { "laminar/B738/knob/autobrake_1"};
		command_ref_strings[CommandRefName::autobrake_2]  = { "laminar/B738/knob/autobrake_2"};
		command_ref_strings[CommandRefName::autobrake_3]  = { "laminar/B738/knob/autobrake_3"};
		command_ref_strings[CommandRefName::autobrake_max] = {"laminar/B738/knob/autobrake_max"};
		command_ref_strings[CommandRefName::autobrake_off] = {"laminar/B738/knob/autobrake_off"};
		command_ref_strings[CommandRefName::autobrake_rto] = {"laminar/B738/knob/autobrake_rto"};
		command_ref_strings[CommandRefName::gear_down] = { "laminar/B738/push_button/gear_down" };
		command_ref_strings[CommandRefName::gear_off] = { "laminar/B738/push_button/gear_off" };
		command_ref_strings[CommandRefName::gear_up] = { "laminar/B738/push_button/gear_up" };

		command_ref_strings[CommandRefName::fuel_flow_dn] = {"laminar/B738/toggle_switch/fuel_flow_dn"};
		command_ref_strings[CommandRefName::fuel_flow_up] = {"laminar/B738/toggle_switch/fuel_flow_up"};

		// Capt
		command_ref_strings[CommandRefName::master_caution1] = { "laminar/B738/push_button/master_caution1"};
		command_ref_strings[CommandRefName::capt_six_pack] = { "laminar/B738/push_button/capt_six_pack"};
		command_ref_strings[CommandRefName::chrono_capt_et_mode] = {"laminar/B738/push_button/chrono_capt_et_mode"};
		command_ref_strings[CommandRefName::fire_bell_light1] = {"laminar/B738/push_button/fire_bell_light1"};
		// FO
		command_ref_strings[CommandRefName::master_caution2] = { "laminar/B738/push_button/master_caution2"};
		command_ref_strings[CommandRefName::fo_six_pack] = { "laminar/B738/push_button/fo_six_pack"};
		command_ref_strings[CommandRefName::chrono_fo_et_mode] = {"laminar/B738/push_button/chrono_fo_et_mode"};
		command_ref_strings[CommandRefName::fire_bell_light2] = {"laminar/B738/push_button/fire_bell_light2"};

		command_ref_strings[CommandRefName::MFD_ENG] = {"laminar/B738/LDU_control/push_button/MFD_ENG"};
		command_ref_strings[CommandRefName::MFD_SYS] = {"laminar/B738/LDU_control/push_button/MFD_SYS"};

		// Capt
		command_ref_strings[CommandRefName::ap_disconnect_test1_dn] = {"laminar/B738/toggle_switch/ap_disconnect_test1_dn"};  // Capt TEST 2
		command_ref_strings[CommandRefName::ap_disconnect_test1_up] = {"laminar/B738/toggle_switch/ap_disconnect_test1_up"};  // Capt TEST 1
		command_ref_strings[CommandRefName::ap_light_pilot_pb] = {"laminar/B738/push_button/ap_light_pilot"};
		command_ref_strings[CommandRefName::at_light_pilot_pb] = {"laminar/B738/push_button/at_light_pilot"};
		command_ref_strings[CommandRefName::fms_light_pilot_pb] = {"laminar/B738/push_button/fms_light_pilot"};
		// FO
		command_ref_strings[CommandRefName::ap_disconnect_test2_dn] = {"laminar/B738/toggle_switch/ap_disconnect_test2_dn"};  // FO TEST 2
		command_ref_strings[CommandRefName::ap_disconnect_test2_up] = {"laminar/B738/toggle_switch/ap_disconnect_test2_up"};  // FO TEST 1
		command_ref_strings[CommandRefName::ap_light_fo_pb] = {"laminar/B738/push_button/ap_light_fo"};
		command_ref_strings[CommandRefName::at_light_fo_pb] = {"laminar/B738/push_button/at_light_fo"};
		command_ref_strings[CommandRefName::fms_light_fo_pb] = {"laminar/B738/push_button/fms_light_fo"};


		command_ref_strings[CommandRefName::land_lights_ret_left_off] = { "laminar/B738/switch/land_lights_ret_left_off" };
		command_ref_strings[CommandRefName::land_lights_ret_left_on] = { "laminar/B738/switch/land_lights_ret_left_on" };
		command_ref_strings[CommandRefName::land_lights_ret_right_off] = { "laminar/B738/switch/land_lights_ret_right_off" };
		command_ref_strings[CommandRefName::land_lights_ret_right_on] = { "laminar/B738/switch/land_lights_ret_right_on" };
		command_ref_strings[CommandRefName::land_lights_left_off] = { "laminar/B738/switch/land_lights_left_off" };
		command_ref_strings[CommandRefName::land_lights_left_on] = { "laminar/B738/switch/land_lights_left_on" };
		command_ref_strings[CommandRefName::land_lights_right_off] = { "laminar/B738/switch/land_lights_right_off" };
		command_ref_strings[CommandRefName::land_lights_right_on] = { "laminar/B738/switch/land_lights_right_on" };
		command_ref_strings[CommandRefName::rwy_light_left_on] = {"laminar/B738/switch/rwy_light_left_on"};
		command_ref_strings[CommandRefName::rwy_light_left_off] = {"laminar/B738/switch/rwy_light_left_off"};
		command_ref_strings[CommandRefName::rwy_light_right_on] = {"laminar/B738/switch/rwy_light_right_on"};
		command_ref_strings[CommandRefName::rwy_light_right_off] = {"laminar/B738/switch/rwy_light_right_off"};


		command_ref_strings[CommandRefName::taxi_light_brightness_on] = {"laminar/B738/toggle_switch/taxi_light_brightness_on"};
		command_ref_strings[CommandRefName::taxi_light_brightness_off] = {"laminar/B738/toggle_switch/taxi_light_brightness_off"};
		command_ref_strings[CommandRefName::APU_start_pos_dn] = {"laminar/B738/spring_toggle_switch/APU_start_pos_dn"};
		command_ref_strings[CommandRefName::APU_start_pos_up] = {"laminar/B738/spring_toggle_switch/APU_start_pos_up"};
		command_ref_strings[CommandRefName::hydro_pumps1] = {"laminar/B738/toggle_switch/hydro_pumps1"};
		command_ref_strings[CommandRefName::hydro_pumps2] = {"laminar/B738/toggle_switch/hydro_pumps2"};
		//
		// *** NOTE *** Zibo has a typo the name for pumps 1 and 2 swapped
		command_ref_strings[CommandRefName::electric_hydro_pumps1] = {"laminar/B738/toggle_switch/electric_hydro_pumps2"};
		command_ref_strings[CommandRefName::electric_hydro_pumps2] = {"laminar/B738/toggle_switch/electric_hydro_pumps1"};
		command_ref_strings[CommandRefName::eng_start_source_left] = {"laminar/B738/toggle_switch/eng_start_source_left"};
		command_ref_strings[CommandRefName::eng_start_source_right] = {"laminar/B738/toggle_switch/eng_start_source_right"};
		command_ref_strings[CommandRefName::fmc_source_left] = {"laminar/B738/toggle_switch/fmc_source_left"};
		command_ref_strings[CommandRefName::fmc_source_right] = {"laminar/B738/toggle_switch/fmc_source_right"};
		command_ref_strings[CommandRefName::logo_light_on] = {"laminar/B738/switch/logo_light_on"};
		command_ref_strings[CommandRefName::logo_light_off] = {"laminar/B738/switch/logo_light_off"};
		command_ref_strings[CommandRefName::beacon_lights_toggle] = {"sim/lights/beacon_lights_toggle"};
		command_ref_strings[CommandRefName::wing_light_on] = {"laminar/B738/switch/wing_light_on"};
		command_ref_strings[CommandRefName::wing_light_off] = {"laminar/B738/switch/wing_light_off"};
		command_ref_strings[CommandRefName::wheel_light_off] = {"laminar/B738/switch/wheel_light_off"};			// ********* command opposite   wheel_light_off == ON
		command_ref_strings[CommandRefName::wheel_light_on] = {"laminar/B738/switch/wheel_light_on"};			// ********* command opposite   wheel_light_on == OFF
		command_ref_strings[CommandRefName::eq_cool_supply] = {"laminar/B738/toggle_switch/eq_cool_supply"};
		command_ref_strings[CommandRefName::eq_cool_exhaust] = {"laminar/B738/toggle_switch/eq_cool_exhaust"};
		command_ref_strings[CommandRefName::emer_exit_lights_up] = {"laminar/B738/toggle_switch/emer_exit_lights_up"};
		command_ref_strings[CommandRefName::emer_exit_lights_dn] = {"laminar/B738/toggle_switch/emer_exit_lights_dn"};
		command_ref_strings[CommandRefName::seatbelt_sign_up] = {"laminar/B738/toggle_switch/seatbelt_sign_up"};
		command_ref_strings[CommandRefName::seatbelt_sign_dn] = {"laminar/B738/toggle_switch/seatbelt_sign_dn"};
		command_ref_strings[CommandRefName::no_smoking_up] = {"laminar/B738/toggle_switch/no_smoking_up"};
		command_ref_strings[CommandRefName::no_smoking_dn] = {"laminar/B738/toggle_switch/no_smoking_dn"};

		command_ref_strings[CommandRefName::attend] = {"laminar/B738/push_button/attend"};
		command_ref_strings[CommandRefName::grd_call] = {"laminar/B738/push_button/grd_call"};

		command_ref_strings[CommandRefName::crossfeed_valve_on] = {"laminar/B738/toggle_switch/crossfeed_valve_on"};
		command_ref_strings[CommandRefName::crossfeed_valve_off] = {"laminar/B738/toggle_switch/crossfeed_valve_off"};
		command_ref_strings[CommandRefName::fuel_pump_lft1] = {"laminar/B738/toggle_switch/fuel_pump_lft1"};
		command_ref_strings[CommandRefName::fuel_pump_lft2] = {"laminar/B738/toggle_switch/fuel_pump_lft2"};
		command_ref_strings[CommandRefName::fuel_pump_rgt2] = {"laminar/B738/toggle_switch/fuel_pump_rgt2"};
		command_ref_strings[CommandRefName::fuel_pump_rgt1] = {"laminar/B738/toggle_switch/fuel_pump_rgt1"};
		command_ref_strings[CommandRefName::fuel_pump_ctr1] = {"laminar/B738/toggle_switch/fuel_pump_ctr1"};
		command_ref_strings[CommandRefName::fuel_pump_ctr2] = {"laminar/B738/toggle_switch/fuel_pump_ctr2"};
		command_ref_strings[CommandRefName::yaw_dumper] = {"laminar/B738/toggle_switch/yaw_dumper"};
		command_ref_strings[CommandRefName::cab_util_toggle] = {"laminar/B738/autopilot/cab_util_toggle"};
		command_ref_strings[CommandRefName::standby_bat_left] = {"laminar/B738/switch/standby_bat_left"};
		command_ref_strings[CommandRefName::standby_bat_right] = {"laminar/B738/switch/standby_bat_right"};
		command_ref_strings[CommandRefName::gpu_up] = {"laminar/B738/toggle_switch/gpu_up"};
		command_ref_strings[CommandRefName::gpu_dn] = {"laminar/B738/toggle_switch/gpu_dn"};
		command_ref_strings[CommandRefName::apu_gen1_up] = {"laminar/B738/toggle_switch/apu_gen1_up"};
		command_ref_strings[CommandRefName::apu_gen1_dn] = {"laminar/B738/toggle_switch/apu_gen1_dn"};
		command_ref_strings[CommandRefName::apu_gen2_up] = {"laminar/B738/toggle_switch/apu_gen2_up"};
		command_ref_strings[CommandRefName::apu_gen2_dn] = {"laminar/B738/toggle_switch/apu_gen2_dn"};
		command_ref_strings[CommandRefName::gen1_up] = {"laminar/B738/toggle_switch/gen1_up"};
		command_ref_strings[CommandRefName::gen1_dn] = {"laminar/B738/toggle_switch/gen1_dn"};
		command_ref_strings[CommandRefName::gen2_up] = {"laminar/B738/toggle_switch/gen2_up"};
		command_ref_strings[CommandRefName::gen2_dn] = {"laminar/B738/toggle_switch/gen2_dn"};
		command_ref_strings[CommandRefName::window_heat_l_fwd] = {"laminar/B738/toggle_switch/window_heat_l_fwd"};
		command_ref_strings[CommandRefName::window_heat_l_side] = {"laminar/B738/toggle_switch/window_heat_l_side"};
		command_ref_strings[CommandRefName::window_heat_r_fwd] = {"laminar/B738/toggle_switch/window_heat_r_fwd"};
		command_ref_strings[CommandRefName::window_heat_r_side] = {"laminar/B738/toggle_switch/window_heat_r_side"};
		command_ref_strings[CommandRefName::window_ovht_test_up] = {"laminar/B738/toggle_switch/window_ovht_test_up"};
		command_ref_strings[CommandRefName::window_ovht_test_dn] = {"laminar/B738/toggle_switch/window_ovht_test_dn"};
		command_ref_strings[CommandRefName::capt_probes_pos] = {"laminar/B738/toggle_switch/capt_probes_pos"};
		command_ref_strings[CommandRefName::fo_probes_pos] = {"laminar/B738/toggle_switch/fo_probes_pos"};
		command_ref_strings[CommandRefName::main_pnl_du_capt_left] = {"laminar/B738/toggle_switch/main_pnl_du_capt_left"};
		command_ref_strings[CommandRefName::main_pnl_du_capt_right] = {"laminar/B738/toggle_switch/main_pnl_du_capt_right"};
//		command_ref_strings[CommandRefName::main_pnl_du_fo_left] = {"laminar/B738/toggle_switch/main_pnl_du_fo_left"};
//		command_ref_strings[CommandRefName::main_pnl_du_fo_right] = {"laminar/B738/toggle_switch/main_pnl_du_fo_right"};
		command_ref_strings[CommandRefName::lower_du_capt_left] = {"laminar/B738/toggle_switch/lower_du_capt_left"};
		command_ref_strings[CommandRefName::lower_du_capt_right] = {"laminar/B738/toggle_switch/lower_du_capt_right"};
//		command_ref_strings[CommandRefName::lower_du_fo_left] = {"laminar/B738/toggle_switch/lower_du_fo_left"};
//		command_ref_strings[CommandRefName::lower_du_fo_right] = {"laminar/B738/toggle_switch/lower_du_fo_right"};

		command_ref_strings[CommandRefName::gpws_test] = {"laminar/B738/push_button/gpws_test"};
		command_ref_strings[CommandRefName::gpws_flap] = {"laminar/B738/toggle_switch/gpws_flap"};
		command_ref_strings[CommandRefName::gpws_terr] = {"laminar/B738/toggle_switch/gpws_terr"};
		command_ref_strings[CommandRefName::gpws_gear] = {"laminar/B738/toggle_switch/gpws_gear"};

		command_ref_strings[CommandRefName::rudder_trim_left] = {"sim/flight_controls/rudder_trim_left"};
		command_ref_strings[CommandRefName::rudder_trim_right] = {"sim/flight_controls/rudder_trim_right"};
		command_ref_strings[CommandRefName::aileron_trim_left] = {"sim/flight_controls/aileron_trim_left"};
		command_ref_strings[CommandRefName::aileron_trim_right] = {"sim/flight_controls/aileron_trim_right"};

		command_ref_strings[CommandRefName::n1_set_source_left] = {"laminar/B738/toggle_switch/n1_set_source_left"};
		command_ref_strings[CommandRefName::n1_set_source_right] = {"laminar/B738/toggle_switch/n1_set_source_right"};


		command_ref_strings[CommandRefName::air_valve_ctrl_left] = { "laminar/B738/toggle_switch/air_valve_ctrl_left" };
		command_ref_strings[CommandRefName::air_valve_ctrl_right] = { "laminar/B738/toggle_switch/air_valve_ctrl_right" };
		command_ref_strings[CommandRefName::spoiler_A] = { "laminar/B738/toggle_switch/spoiler_A" };
		command_ref_strings[CommandRefName::spoiler_B] = { "laminar/B738/toggle_switch/spoiler_B" };
		command_ref_strings[CommandRefName::dc_power_dn] = { "laminar/B738/knob/dc_power_dn" };
		command_ref_strings[CommandRefName::dc_power_up] = { "laminar/B738/knob/dc_power_up" };
		command_ref_strings[CommandRefName::ac_power_dn] = { "laminar/B738/knob/ac_power_dn" };
		command_ref_strings[CommandRefName::ac_power_up] = { "laminar/B738/knob/ac_power_up" };
		command_ref_strings[CommandRefName::wing_heat] = {"laminar/B738/toggle_switch/wing_heat"};
		command_ref_strings[CommandRefName::eng1_heat] = {"laminar/B738/toggle_switch/eng1_heat"};
		command_ref_strings[CommandRefName::eng2_heat] = {"laminar/B738/toggle_switch/eng2_heat"};
		command_ref_strings[CommandRefName::l_recirc_fan] = {"laminar/B738/toggle_switch/l_recirc_fan"};
		command_ref_strings[CommandRefName::r_recirc_fan] = {"laminar/B738/toggle_switch/r_recirc_fan"};
		command_ref_strings[CommandRefName::l_pack_dn] = {"laminar/B738/toggle_switch/l_pack_dn"};
		command_ref_strings[CommandRefName::l_pack_up] = {"laminar/B738/toggle_switch/l_pack_up"};
		command_ref_strings[CommandRefName::r_pack_dn] = {"laminar/B738/toggle_switch/r_pack_dn"};
		command_ref_strings[CommandRefName::r_pack_up] = {"laminar/B738/toggle_switch/r_pack_up"};
		command_ref_strings[CommandRefName::iso_valve_dn] = {"laminar/B738/toggle_switch/iso_valve_dn"};
		command_ref_strings[CommandRefName::iso_valve_up] = {"laminar/B738/toggle_switch/iso_valve_up"};
		command_ref_strings[CommandRefName::bleed_air_1] = {"laminar/B738/toggle_switch/bleed_air_1"};
		command_ref_strings[CommandRefName::bleed_air_2] = {"laminar/B738/toggle_switch/bleed_air_2"};
		command_ref_strings[CommandRefName::bleed_air_apu] = {"laminar/B738/toggle_switch/bleed_air_apu"};
		command_ref_strings[CommandRefName::bleed_trip_reset] = {"laminar/B738/push_button/bleed_trip_reset"};
		command_ref_strings[CommandRefName::position_light_steady] = {"laminar/B738/toggle_switch/position_light_steady"};
		command_ref_strings[CommandRefName::position_light_strobe] = {"laminar/B738/toggle_switch/position_light_strobe"};
		command_ref_strings[CommandRefName::position_light_off] = {"laminar/B738/toggle_switch/position_light_off"};
		command_ref_strings[CommandRefName::irs_L_left] = {"laminar/B738/toggle_switch/irs_L_left"};
		command_ref_strings[CommandRefName::irs_L_right] = {"laminar/B738/toggle_switch/irs_L_right"};
		command_ref_strings[CommandRefName::irs_R_left] = {"laminar/B738/toggle_switch/irs_R_left"};
		command_ref_strings[CommandRefName::irs_R_right] = {"laminar/B738/toggle_switch/irs_R_right"};
		command_ref_strings[CommandRefName::irs_sys_dspl] = {"laminar/B738/toggle_switch/irs_sys_dspl"};
		command_ref_strings[CommandRefName::irs_dspl_sel_left] = {"laminar/B738/toggle_switch/irs_dspl_sel_left"};
		command_ref_strings[CommandRefName::irs_dspl_sel_right] = {"laminar/B738/toggle_switch/irs_dspl_sel_right"};
		command_ref_strings[CommandRefName::pax_oxy_on] = {"laminar/B738/one_way_switch/pax_oxy_on"};
		command_ref_strings[CommandRefName::pax_oxy_norm] = {"laminar/B738/one_way_switch/pax_oxy_norm"};

		command_ref_strings[CommandRefName::eng1_start_off] = {"laminar/B738/rotary/eng1_start_off"};
		command_ref_strings[CommandRefName::eng1_start_grd] = {"laminar/B738/rotary/eng1_start_grd"};
		command_ref_strings[CommandRefName::eng1_start_flt] = {"laminar/B738/rotary/eng1_start_flt"};
		command_ref_strings[CommandRefName::eng1_start_cont] = {"laminar/B738/rotary/eng1_start_cont"};
		command_ref_strings[CommandRefName::eng2_start_off] = {"laminar/B738/rotary/eng2_start_off"};
		command_ref_strings[CommandRefName::eng2_start_grd] = {"laminar/B738/rotary/eng2_start_grd"};
		command_ref_strings[CommandRefName::eng2_start_flt] = {"laminar/B738/rotary/eng2_start_flt"};
		command_ref_strings[CommandRefName::eng2_start_cont] = {"laminar/B738/rotary/eng2_start_cont"};

		command_ref_strings[CommandRefName::drive_disconnect1] = {"laminar/B738/one_way_switch/drive_disconnect1"};
		command_ref_strings[CommandRefName::drive_disconnect1_off] = {"laminar/B738/one_way_switch/drive_disconnect1_off"};
		command_ref_strings[CommandRefName::drive_disconnect2] = {"laminar/B738/one_way_switch/drive_disconnect2"};
		command_ref_strings[CommandRefName::drive_disconnect2_off] = {"laminar/B738/one_way_switch/drive_disconnect2_off"};
		command_ref_strings[CommandRefName::air_valve_manual_left] = {"laminar/B738/toggle_switch/air_valve_manual_left"};
		command_ref_strings[CommandRefName::air_valve_manual_right] = {"laminar/B738/toggle_switch/air_valve_manual_right"};
		command_ref_strings[CommandRefName::alt_flaps] = {"laminar/B738/toggle_switch/alt_flaps"};
		command_ref_strings[CommandRefName::flt_ctr_B_up] = {"laminar/B738/toggle_switch/flt_ctr_B_up"};
		command_ref_strings[CommandRefName::flt_ctr_B_dn] = {"laminar/B738/toggle_switch/flt_ctr_B_dn"};
		command_ref_strings[CommandRefName::flt_ctr_A_up] = {"laminar/B738/toggle_switch/flt_ctr_A_up"};
		command_ref_strings[CommandRefName::flt_ctr_A_dn] = {"laminar/B738/toggle_switch/flt_ctr_A_dn"};
		command_ref_strings[CommandRefName::alt_flaps_ctrl_up] = {"laminar/B738/toggle_switch/alt_flaps_ctrl_up"};
		command_ref_strings[CommandRefName::alt_flaps_ctrl_dn] = {"laminar/B738/toggle_switch/alt_flaps_ctrl_dn"};
		command_ref_strings[CommandRefName::dspl_source_left] = {"laminar/B738/toggle_switch/dspl_source_left"};
		command_ref_strings[CommandRefName::dspl_source_right] = {"laminar/B738/toggle_switch/dspl_source_right"};
		command_ref_strings[CommandRefName::irs_source_left] = {"laminar/B738/toggle_switch/irs_source_left"};
		command_ref_strings[CommandRefName::irs_source_right] = {"laminar/B738/toggle_switch/irs_source_right"};
		command_ref_strings[CommandRefName::vhf_nav_source_lft] = {"laminar/B738/toggle_switch/vhf_nav_source_lft"};
		command_ref_strings[CommandRefName::vhf_nav_source_rgt] = {"laminar/B738/toggle_switch/vhf_nav_source_rgt"};






		//command_ref_strings[CommandRefName::] = { "" };
		//command_ref_strings[CommandRefName::] = { "" };
		//command_ref_strings[CommandRefName::] = { "" };
		//command_ref_strings[CommandRefName::] = { "" };
		//command_ref_strings[CommandRefName::] = {""};


				// guards [11]
		// [0] EEC #1			open = 1.0
		// [1] EEC #2			open = 1.0
		// [2] Battery_pos,		open = 1.0
		// [3] StandBy Power	open = 1.0
		// [4] Eng 1 Disconnect open = 1.0
		// [5] Eng 2 Disconnect open = 1.0
		// [6] Bus Transfer		open = 1.0
		// [7] Pass Oxygen		open = 1.0
		// [8] ELT				open = 1.0
		// [9] Emer Exit Lights	open = 1.0
		// [10] Nose Wheel Steering open = 1.0
		//data_ref_strings[DataRefName::covers] = { "laminar/B738/button_switch/cover_position", XplaneType::type_FloatArray }; 
		//

	}
}