#pragma once

namespace zcockpit::cockpit::hardware
{
	enum class Health
	{
		HEALTHY_STATUS,
		FAILED_STATUS,
		UNKNOWN_STATUS,
	};


	constexpr const char* HealthToString(const Health hs) noexcept
	{
		switch (hs) {
		case Health::HEALTHY_STATUS: return "Status is Healthy";
		case Health::FAILED_STATUS: return "Status is Failed";
		case Health::UNKNOWN_STATUS: return "Status is Unknown";
		}
		return "Health enum to string conversion failed";
	};
}