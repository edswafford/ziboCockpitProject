#pragma once
#include "shared_types.hpp"
#include "../data_ref_name.hpp"

namespace zcockpit::cockpit::hardware{

	struct ZcockpitSwitch
	{
		ZcockpitSwitch() = default;
		ZcockpitSwitch(DataRefName name, common::SwitchType type,  int value, int center_value=0, int array_offset=0):
			valid(true),
			dataref_name(name),
			switch_type(type),
			spring_loaded_return_value(center_value),
			offset(array_offset),
			int_hw_value(value)
		{}
		ZcockpitSwitch(DataRefName name, common::SwitchType type, float value, int center_value=0, int array_offset=0):
			valid(true),
			dataref_name(name),
			switch_type(type),
			spring_loaded_return_value(center_value),
			offset(array_offset),
			float_hw_value(value)
		{}
		bool valid{false};
		DataRefName dataref_name{DataRefName::DataRefName_unused};
		common::SwitchType switch_type{common::SwitchType::unused};
		int spring_loaded_return_value{0};
		int offset{0};
		union
		{
			float float_hw_value;
			int int_hw_value{0};
		};
	};
}
