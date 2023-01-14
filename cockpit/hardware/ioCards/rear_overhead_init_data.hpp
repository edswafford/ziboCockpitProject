#pragma once


namespace zcockpit::cockpit::hardware
{
	enum class RearSwitchPosition : unsigned {
		drive_disconnect2_pos_generator_disconnect_up,
		drive_disconnect2_pos_generator_disconnect_down,
		eng2_heat_pos_off,
		eng2_heat_pos_on,

		Switch_position_unused,
		kMaxValue = Switch_position_unused,
	};
}