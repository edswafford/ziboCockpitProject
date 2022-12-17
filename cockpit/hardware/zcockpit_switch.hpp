#pragma once
#include "shared_types.hpp"
#include "../data_ref_name.hpp"

namespace zcockpit::cockpit::hardware{

	struct ZcockpitSwitch
	{
		ZcockpitSwitch() = default;
		ZcockpitSwitch(DataRefName name, common::SwitchType type, int center_value=0, int value=0):
			dataref_name(name),
			switch_type(type),
			spring_loaded_return_value(center_value),
			hw_value(value)
		{}

		DataRefName dataref_name{DataRefName::unused};
		common::SwitchType switch_type{common::SwitchType::unused};
		int spring_loaded_return_value{0};
		int hw_value{0};
	};
}
