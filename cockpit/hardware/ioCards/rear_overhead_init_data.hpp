#pragma once


namespace zcockpit::cockpit::hardware
{
	enum class RearSwitchPosition : unsigned {
		drive_disconnect2_pos_generator_disconnect_up,
		drive_disconnect2_pos_generator_disconnect_down,
		eng2_heat_pos_off,
		eng2_heat_pos_on,
		irs_l_off,
		irs_l_align,
		irs_l_nav,
		irs_l_att,
		irs_r_off,
		irs_r_align,
		irs_r_nav,
		irs_r_att,
		irs_dspl_l,
		irs_dspl_r,
		irs_dspl_sel_test,
		irs_dspl_sel_tk_gs,
		irs_dspl_sel_pos,
		irs_dspl_sel_wind,
		irs_dspl_sel_heading,
		pass_oxygen_on,
		pass_oxygen_normal,
		mach_airspd_warning_1_pressed,
		mach_airspd_warning_1_released,
		mach_airspd_warning_2_pressed,
		mach_airspd_warning_2_released,
		stall_warning_1_pressed,
		stall_warning_1_released,
		stall_warning_2_pressed,
		stall_warning_2_released,


		Switch_position_unused,
		kMaxValue = Switch_position_unused,


	};
}