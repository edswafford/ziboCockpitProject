#pragma once
#include "shared_types.hpp"
#include "data_ref_name.hpp"

namespace zcockpit::cockpit {






	//
	// COMMAND REF
	//
	enum class CommandRefName : unsigned{
		battery_dn,
		battery_up,
		bright_test_dn,
		bright_test_up,
		spd_ref_left,
		spd_ref_right,
		autobrake_1,
		autobrake_2,
		autobrake_3,
		autobrake_max,
		autobrake_off,
		autobrake_rto,
		gear_up,
		gear_down,
		gear_off,
		fuel_flow_dn,
		fuel_flow_up,

		// Capt
		master_caution1,
		capt_six_pack,
		chrono_capt_et_mode,
		fire_bell_light1,
		// FO
		master_caution2,
		fo_six_pack,
		chrono_fo_et_mode,
		fire_bell_light2,

		MFD_ENG,
		MFD_SYS,
		//Capt
		ap_disconnect_test1_dn,
		ap_disconnect_test1_up,
		ap_light_pilot_pb,
		at_light_pilot_pb,
		fms_light_pilot_pb,
		// FO
		ap_disconnect_test2_dn,
		ap_disconnect_test2_up,
		ap_light_fo_pb,
		at_light_fo_pb,
		fms_light_fo_pb,

		land_lights_ret_left_off,
		land_lights_ret_left_on,
		land_lights_ret_right_off,
		land_lights_ret_right_on,
		land_lights_left_off,
		land_lights_left_on,
		land_lights_right_off,
		land_lights_right_on,
		rwy_light_left_on,
		rwy_light_left_off,
		rwy_light_right_on,
		rwy_light_right_off,

		taxi_light_brightness_on,
		taxi_light_brightness_off,
		APU_start_pos_dn,
		APU_start_pos_up,
		hydro_pumps1,
		hydro_pumps2,
		electric_hydro_pumps1,
		electric_hydro_pumps2,
		eng_start_source_left,
		eng_start_source_right,
		fmc_source_left,
		fmc_source_right,
		logo_light_on,
		logo_light_off,
		beacon_lights_toggle,
		wing_light_on,
		wing_light_off,
		wheel_light_off,
		wheel_light_on,
		eq_cool_supply,
		eq_cool_exhaust,
		emer_exit_lights_up,
		emer_exit_lights_dn,
		seatbelt_sign_up,
		seatbelt_sign_dn,
		no_smoking_up,
		no_smoking_dn,
		attend,
		grd_call,
		crossfeed_valve_on,
		crossfeed_valve_off,
		fuel_pump_lft1,
		fuel_pump_lft2,
		fuel_pump_rgt2,
		fuel_pump_rgt1,
		fuel_pump_ctr1,
		fuel_pump_ctr2,
		yaw_dumper,
		cab_util_toggle,
		standby_bat_left,
		standby_bat_right,
		gpu_up,
		gpu_dn,
		apu_gen1_up,
		apu_gen1_dn,
		apu_gen2_up,
		apu_gen2_dn,
		gen1_up,
		gen1_dn,
		gen2_up,
		gen2_dn,
		window_heat_l_fwd,
		window_heat_l_side,
		window_heat_r_fwd,
		window_heat_r_side,
		window_ovht_test_up,
		window_ovht_test_dn,
		capt_probes_pos,
		fo_probes_pos,
		main_pnl_du_capt_left,
		main_pnl_du_capt_right,
		main_pnl_du_fo_left,
		main_pnl_du_fo_right, 
		lower_du_capt_left,
		lower_du_capt_right,
		lower_du_fo_left,
		lower_du_fo_right,

		gpws_test,
		gpws_flap,
		gpws_terr,
		gpws_gear,
		rudder_trim_left,
		rudder_trim_right,
		aileron_trim_left,
		aileron_trim_right,
		n1_set_source_left,
		n1_set_source_right,

		CommandRefName_unused,

		kMaxValue = CommandRefName_unused
	};
	static constexpr CommandRefName command_ref_name_list[] = {
		CommandRefName::battery_dn,
		CommandRefName::battery_up,
		CommandRefName::bright_test_dn,
		CommandRefName::bright_test_up,
		CommandRefName::spd_ref_left,
		CommandRefName::spd_ref_right,
		CommandRefName::autobrake_1,
		CommandRefName::autobrake_2,
		CommandRefName::autobrake_3,
		CommandRefName::autobrake_max,
		CommandRefName::autobrake_off,
		CommandRefName::autobrake_rto,
		CommandRefName::gear_up,
		CommandRefName::gear_down,
		CommandRefName::gear_off,
		CommandRefName::fuel_flow_dn,
		CommandRefName::fuel_flow_up,
		// Capt
		CommandRefName::master_caution1,
		CommandRefName::capt_six_pack,
		CommandRefName::chrono_capt_et_mode,
		CommandRefName::fire_bell_light1,
		// FO
		CommandRefName::master_caution2,
		CommandRefName::fo_six_pack,
		CommandRefName::chrono_fo_et_mode,
		CommandRefName::fire_bell_light2,

		CommandRefName::MFD_ENG,
		CommandRefName::MFD_SYS,
		// Capt
		CommandRefName::ap_disconnect_test1_dn,
		CommandRefName::ap_disconnect_test1_up,
		CommandRefName::ap_light_pilot_pb,
		CommandRefName::at_light_pilot_pb,
		CommandRefName::fms_light_pilot_pb,
		// FO
		CommandRefName::ap_disconnect_test2_dn,
		CommandRefName::ap_disconnect_test2_up,
		CommandRefName::ap_light_fo_pb,
		CommandRefName::at_light_fo_pb,
		CommandRefName::fms_light_fo_pb,

		CommandRefName::land_lights_ret_left_off,
		CommandRefName::land_lights_ret_left_on,
		CommandRefName::land_lights_ret_right_off,
		CommandRefName::land_lights_ret_right_on,
		CommandRefName::land_lights_left_off,
		CommandRefName::land_lights_left_on,
		CommandRefName::land_lights_right_off,
		CommandRefName::land_lights_right_on,
		CommandRefName::rwy_light_left_on,
		CommandRefName::rwy_light_left_off,
		CommandRefName::rwy_light_right_on,
		CommandRefName::rwy_light_right_off,

		CommandRefName::taxi_light_brightness_on,
		CommandRefName::taxi_light_brightness_off,
		CommandRefName::APU_start_pos_dn,
		CommandRefName::APU_start_pos_up,
		CommandRefName::hydro_pumps1,
		CommandRefName::hydro_pumps2,
		CommandRefName::electric_hydro_pumps1,
		CommandRefName::electric_hydro_pumps2,
		CommandRefName::eng_start_source_left,
		CommandRefName::eng_start_source_right,
		CommandRefName::fmc_source_left,
		CommandRefName::fmc_source_right,
		CommandRefName::logo_light_on,
		CommandRefName::logo_light_off,
		CommandRefName::beacon_lights_toggle,
		CommandRefName::wing_light_on,
		CommandRefName::wing_light_off,
		CommandRefName::wheel_light_off,
		CommandRefName::wheel_light_on,
		CommandRefName::eq_cool_supply,
		CommandRefName::eq_cool_exhaust,
		CommandRefName::emer_exit_lights_up,
		CommandRefName::emer_exit_lights_dn,

		CommandRefName::seatbelt_sign_up,
		CommandRefName::seatbelt_sign_dn,
		CommandRefName::no_smoking_up,
		CommandRefName::no_smoking_dn,
		CommandRefName::attend,
		CommandRefName::grd_call,
		CommandRefName::crossfeed_valve_on,
		CommandRefName::crossfeed_valve_off,
		CommandRefName::fuel_pump_lft1,
		CommandRefName::fuel_pump_lft2,
		CommandRefName::fuel_pump_rgt2,
		CommandRefName::fuel_pump_rgt1,
		CommandRefName::fuel_pump_ctr1,
		CommandRefName::fuel_pump_ctr2,
		CommandRefName::yaw_dumper,
		CommandRefName::cab_util_toggle,
		CommandRefName::standby_bat_left,
		CommandRefName::standby_bat_right,
		CommandRefName::gpu_up,
		CommandRefName::gpu_dn,
		CommandRefName::apu_gen1_up,
		CommandRefName::apu_gen1_dn,
		CommandRefName::apu_gen2_up,
		CommandRefName::apu_gen2_dn,
		CommandRefName::gen1_up,
		CommandRefName::gen1_dn,
		CommandRefName::gen2_up,
		CommandRefName::gen2_dn,
		CommandRefName::window_heat_l_fwd,
		CommandRefName::window_heat_l_side,
		CommandRefName::window_heat_r_fwd,
		CommandRefName::window_heat_r_side,
		CommandRefName::window_ovht_test_up,
		CommandRefName::window_ovht_test_dn,
		CommandRefName::capt_probes_pos,
		CommandRefName::fo_probes_pos,
		CommandRefName::main_pnl_du_capt_left,
		CommandRefName::main_pnl_du_capt_right,
		CommandRefName::main_pnl_du_fo_left,
		CommandRefName::main_pnl_du_fo_right,
		CommandRefName::lower_du_capt_left,
		CommandRefName::lower_du_capt_right,
		CommandRefName::lower_du_fo_left,
		CommandRefName::lower_du_fo_right,

		CommandRefName::gpws_test,
		CommandRefName::gpws_flap,
		CommandRefName::gpws_terr,
		CommandRefName::gpws_gear,
		CommandRefName::rudder_trim_left,
		CommandRefName::rudder_trim_right,
		CommandRefName::aileron_trim_left,
		CommandRefName::aileron_trim_right,
		CommandRefName::n1_set_source_left,
		CommandRefName::n1_set_source_right,

	};

	struct DataRefParameter
	{
		DataRefParameter(DataRefName name, common::XplaneType type, bool is_bool): short_name(name), xplane_type(type), is_rounded(is_bool){}
		DataRefName short_name;
		common::XplaneType xplane_type;
		bool is_rounded;
	};

	struct CommandRefParameter
	{
		CommandRefParameter(CommandRefName name): short_name(name){}
		CommandRefName short_name;
	};



}
