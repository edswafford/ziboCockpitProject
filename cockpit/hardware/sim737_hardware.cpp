#include "sim737_hardware.hpp"

#include "usb/libusb_interface.hpp"


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

	void Sim737Hardware::initialize_iocard_devices()
	{
		if(LibUsbInterface::is_initialized()) {
			auto avaible_iocards = IOCards::find_iocard_devices();
		}
	}
	void Sim737Hardware::init_fwd_overhead_iocard(const std::string& bus_address)
	{

	//	iocards_fwd_overhead_status = FAILED_STATUS;
		LOG() << "IOCards: creating fwd overhead";
		ovrheadIOCards = std::make_unique<OvrheadIOCards>(bus_address);
		if(ovrheadIOCards->is_open)
		{
			// Did we find the fwd overhead device and manage to open usb connection 
			LOG() << "fwd overhead is Open";
		//	iocards_fwd_overhead_status = HEALTHY_STATUS;
		//	LOG() << "fwd overhead status = " << iocards_fwd_overhead_status;


			if(ovrheadIOCards->initForAsync()) {

				// Axes are not used --> set to 0
				constexpr unsigned char number_of_axes = 0;
				if(ovrheadIOCards->initializeIOCards(number_of_axes))
				{
					ovrheadIOCards->initialize_iocardsdata();

					if(ovrheadIOCards->submit_read_transfer()){
						// Applications should not start the event thread until after their first call to libusb_open()
						ovrheadIOCards->start_event_thread();

						LOG() << "fwd overhead is initialized and thread is running";
					
						ovrheadIOCards->receive_mastercard();


		//			LOG() << "fwd Done First Pass status =" << iocards_fwd_overhead_status;
		//			PostMessage(mainHwnd, WM_IOCARDS_FWD_OVERHEAD_HEALTH, iocards_fwd_overhead_status, NULL);
					}
					else {
						LOG() << "IOCards: fwd overhead failed to reading from usb";
					}
				}
				else
					{
						LOG() << "IOCards: fwd overhead failed init";
					}
			}
		}
		else
		{
			LOG() << "IOCards: Failed to open fwd overhead.";
		}

	}



	void Sim737Hardware::fiveHzTasks(int five_hz_count)
	{

		// MIP IOCards
		//
		if(five_hz_count == 0)
		{
			//if(this->mipIOCards->isOpen)
			//{
			//	status = HEALTHY_STATUS;
			//	if(!this->mipIOCards->IsInitialized())
			//	{
			//		status = FAILED_STATUS;
			//		LOG() << "IOCards 2: closing down mip failed it";
			//	}
			//	else
			//	{
			//		if(this->mipIOCards->receive_mastercard() < 0)
			//		{
			//			status = FAILED_STATUS;
			//			LOG() << "IOCards 2: closing down mip receive < 0";
			//			this->mipIOCards->closeDown();
			//		}
			//		// update inputs
			//		this->mipIOCards->processMIP();

			//		// send outputs
			//		if(this->mipIOCards->send_mastercard() < 0)
			//		{
			//			status = FAILED_STATUS;
			//			LOG() << "IOCards 2: closing down mip send < 0";
			//			this->mipIOCards->closeDown();
			//		}

			//		//// copy current to previous
			//		//if(this->mipIOCards->copyIOCardsData() < 0)
			//		//{
			//		//	status = FAILED_STATUS;
			//		//	LOG() << "IOCards 2: closing down mip copy data < 0";
			//		//	this->mipIOCards->closeDown();
			//		//}
			//	}
			//}
			//else
			//{
			//	status = FAILED_STATUS;
			//}
			//if(status != iocards_mip_status)
			//{
			//	iocards_mip_status = status;
			//	PostMessage(mainHwnd, WM_IOCARDS_MIP_HEALTH, iocards_mip_status, NULL);
			//	LOG() << "IOCards 2: mip status = " << status << " :: " << iocards_mip_status;
			//}
		}

		else if(five_hz_count == 1)
		{
			//if(this->ovrheadIOCards->isOpen)
			//{
			//	status = HEALTHY_STATUS;
			//	if(!this->ovrheadIOCards->IsInitialized())
			//	{
			//		status = FAILED_STATUS;
			//		LOG() << "IOCards 2: closing down fwd overhead failed init";
			//	}
			//	else
			//	{
			//		// update inputs
			//		this->ovrheadIOCards->fastProcessOvrHead();

			//		// send outputs
			//		if(this->ovrheadIOCards->send_mastercard() < 0)
			//		{
			//			status = FAILED_STATUS;
			//			LOG() << "IOCards 2: closing down fwd overhead send < 0";
			//			this->ovrheadIOCards->closeDown();
			//		}


			//		//if(this->ovrheadIOCards->copyIOCardsData() < 0)
			//		//{
			//		//	status = FAILED_STATUS;
			//		//	LOG() << "IOCards 2: closing down fwd overhead copy data < 0";
			//		//	this->ovrheadIOCards->closeDown();
			//		//}
			//	}
			//}
			//else
			//{
			//	status = FAILED_STATUS;
			//}

			//if(status != iocards_fwd_overhead_status)
			//{
			//	iocards_fwd_overhead_status = status;
			//	PostMessage(mainHwnd, WM_IOCARDS_FWD_OVERHEAD_HEALTH, iocards_fwd_overhead_status, NULL);
			//	LOG() << "IOCards 2: fwd overhead status = " << status << " :: " << iocards_fwd_overhead_status;
			//}
		}

		else if(five_hz_count == 2)
		{
			//if(this->rearOvrHeadIOCards->isOpen)
			//{
			//	status = HEALTHY_STATUS;
			//	if(!this->rearOvrHeadIOCards->IsInitialized())
			//	{
			//		status = FAILED_STATUS;
			//		LOG() << "IOCards 2: closing down rear overhead failed init";
			//	}
			//	else
			//	{
			//		status = HEALTHY_STATUS;
			//		if(this->rearOvrHeadIOCards->receive_mastercard() < 0)
			//		{
			//			status = FAILED_STATUS;
			//			LOG() << "IOCards 2: closing down rear overhead receive < 0";
			//			this->rearOvrHeadIOCards->closeDown();
			//		}

			//		// update inputs
			//		this->rearOvrHeadIOCards->fastProcessRearOvrHead();

			//		// send_mastercard() not called
			//		// Because there are no mastercard outputs 

			//		//if(this->rearOvrHeadIOCards->copyIOCardsData() < 0)
			//		//{
			//		//	status = FAILED_STATUS;
			//		//	LOG() << "IOCards 2: closing down rear overhead copy data < 0";
			//		//	this->rearOvrHeadIOCards->closeDown();
			//		//}
			//	}
			//}
			//else
			//{
			//	status = FAILED_STATUS;
			//}

			//if(status != iocards_rear_overhead_status)
			//{
			//	iocards_rear_overhead_status = status;
			//	PostMessage(mainHwnd, WM_IOCARDS_REAR_OVERHEAD_HEALTH, iocards_rear_overhead_status, NULL);
			//	LOG() << "IOCards 2: rear overhead status = " << status << " :: " << iocards_rear_overhead_status;
			//}
		}


		else if(five_hz_count == 3)
		{
			//if(xpndr->Available() && ifly737->shareMemSDK->IFLY737NG_STATE && ifly737->electrical_power_state != 0)
			//{
			//	xpndr->check_xpndr_digits();
			//}

			////
			//// Transponder
			////
			//if(xpndr->Available() && ifly737->shareMemSDK->IFLY737NG_STATE)
			//{
			//	xpndr->updatePowerOn();
			//	xpndr->updateFailed();
			//	xpndr->updateRply();
			//	xpndr->requestData();
			//	xpndr->readXpndr();

			//	if(init_xpndr)
			//	{
			//		init_xpndr = false;
			//		xpndr->sync_switches();
			//	}
			//}


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
