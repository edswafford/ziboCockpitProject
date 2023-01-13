#pragma once


namespace zcockpit::cockpit::hardware
{
	enum class SwitchPosition : unsigned {
		starter1_pos_gnd,
		starter1_pos_off,
		starter1_pos_cont,
		starter1_pos_flt,
		starter2_pos_gnd,
		starter2_pos_off,
		starter2_pos_cont,
		starter2_pos_flt,
		drive_disconnect1_pos_generator_disconnect_up,
		drive_disconnect1_pos_generator_disconnect_down,
		air_valve_manual_outflow_valve_open,
		air_valve_manual_outflow_valve_middle,
		air_valve_manual_outflow_valve_close,
		alt_flaps_pos_alternate_flaps_arm,
		alt_flaps_pos_alternate_flaps_off,
		flt_ctr_b_pos_flight_control_b_on,
		flt_ctr_b_pos_flight_control_b_off,
		flt_ctr_b_pos_flight_control_b_stbyrud,
		flt_ctr_a_pos_flight_control_a_on,
		flt_ctr_a_pos_flight_control_a_off,
		flt_ctr_a_pos_flight_control_a_stbyrud,
		alt_flaps_ctrl_alternate_flaps_ctrl_dn,
		alt_flaps_ctrl_alternate_flaps_ctrl_off,
		alt_flaps_ctrl_alternate_flaps_ctrl_up,
		dspl_source_instrument_displays_source_1,
		dspl_source_instrument_displays_source_2,
		dspl_source_instrument_displays_source_auto,
		irs_source_fms_irs_tfr_r,
		irs_source_fms_irs_tfr_normal,
		irs_source_fms_irs_tfr_l,
		vhf_nav_source_fms_vhf_nav_l,
		vhf_nav_source_fms_vhf_nav_normal,
		vhf_nav_source_fms_vhf_nav_r,
		max_allowable_altitude,
		landing_alt,

		Switch_position_unused,
		kMaxValue = Switch_position_unused,
	};
}
