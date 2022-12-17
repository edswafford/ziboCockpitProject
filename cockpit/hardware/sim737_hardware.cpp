#include "sim737_hardware.hpp"


namespace zcockpit::cockpit::hardware
{
	Sim737Hardware::Sim737Hardware(InterfaceIT& iit): interface_it(iit)
	{
		interface_it.initialize();
	}

	Sim737Hardware::~Sim737Hardware()
	{
		interface_it.closeInterfaceITController();
		interface_it.drop();
	}

	void Sim737Hardware::fiveHzTasks(int five_hz_count)
	{
		if (five_hz_count == 0)
		{
		}
		if (five_hz_count == 1)
		{
		}
		if (five_hz_count == 2)
		{
		}
		if (five_hz_count == 3)
		{
			//
			// InterfaceIT
			//
			if (interface_it.Avaliable())
			{
				// process switches
				if (interface_it.MipAvailable())
				{
					//mipTimeout = 0;
					interface_it.processMipState();
				}

				if (interface_it.OverHeadAvailable())
				{
					//overheadTimeout = 0;
					interface_it.processOverHeadState();
				}
			}
		}

	}

	void Sim737Hardware::checkConnections()
	{
		//
		// InterfaceIT MIP
		//
		if (!zcockpit::cockpit::hardware::InterfaceIT::Avaliable())
		{
			zcockpit::cockpit::hardware::InterfaceIT::initialize();
		}


		auto current_interfaceIF_MIP_status = Health::FAILED_STATUS;
		if (zcockpit::cockpit::hardware::InterfaceIT::MipAvailable())
		{
			current_interfaceIF_MIP_status = Health::HEALTHY_STATUS;
		}
		if (interfaceIT_MIP_status != current_interfaceIF_MIP_status)
		{
			interfaceIT_MIP_status = current_interfaceIF_MIP_status;
		}

		//
		// InterfaceIT Overhead
		//
		auto current_interfaceIF_overhead_status = Health::FAILED_STATUS;
		if (zcockpit::cockpit::hardware::InterfaceIT::OverHeadAvailable())
		{
			current_interfaceIF_overhead_status = Health::HEALTHY_STATUS;
		}
		if (interfaceIT_overhead_status != current_interfaceIF_overhead_status)
		{
			interfaceIT_overhead_status = current_interfaceIF_overhead_status;
		}
	}

	bool Sim737Hardware::interfaceitMipStatus() const
	{return interfaceIT_MIP_status == Health::HEALTHY_STATUS;}

	bool Sim737Hardware::interfaceitOverheadStatus() const
	{return interfaceIT_overhead_status == Health::HEALTHY_STATUS;}
}
