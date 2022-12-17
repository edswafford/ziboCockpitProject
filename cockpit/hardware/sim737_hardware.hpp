#pragma once
#include "InterfaceIT/interfaceit.hpp"
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


		void fiveHzTasks(int five_hz_count);


		void checkConnections();

		[[nodiscard]] bool interfaceitMipStatus() const;
		[[nodiscard]] bool interfaceitOverheadStatus() const;

	private:
		InterfaceIT& interface_it;


		Health interfaceIT_MIP_status{Health::UNKNOWN_STATUS};
		Health interfaceIT_overhead_status{Health::UNKNOWN_STATUS};
	};
}