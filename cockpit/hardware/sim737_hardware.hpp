#pragma once
#include "InterfaceIT/interfaceit.hpp"
#include "ioCards/mip_iocard.hpp"
#include "ioCards/fwd_overhead_iocard.hpp"
#include "ioCards/rear_overhead_iocard.hpp"
#include "health.hpp"
#include "logger.hpp"


extern logger LOG;
namespace zcockpit::cockpit::hardware
{
	class Sim737Hardware
	{
	public:

		Sim737Hardware(InterfaceIT& iit);

		~Sim737Hardware();

		void initialize_iocard_devices();

		void fiveHzTasks(int five_hz_count);

		void checkConnections();

		[[nodiscard]] bool interfaceitMipStatus() const;
		[[nodiscard]] bool interfaceitOverheadStatus() const;
		[[nodiscard]] bool get_iocard_mip_status() const;
		[[nodiscard]] bool get_iocard_forward_overhead_status() const;
		[[nodiscard]] bool get_iocard_rear_overhead_status() const;

	private:
		InterfaceIT& interface_it;
		
		std::unique_ptr<ForwardOverheadIOCard> mip_card;
		std::unique_ptr<ForwardOverheadIOCard> forward_overhead_card;
		std::unique_ptr<ForwardOverheadIOCard> rear_overhead_card;




		Health interfaceIT_MIP_status{Health::UNKNOWN_STATUS};
		Health interfaceIT_overhead_status{Health::UNKNOWN_STATUS};
		Health iocard_mip_status{Health::UNKNOWN_STATUS};
		Health iocard_forward_overhead_status{Health::UNKNOWN_STATUS};
		Health iocard_rear_overhead_status{Health::UNKNOWN_STATUS};
	};
}