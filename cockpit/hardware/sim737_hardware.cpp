#include "sim737_hardware.hpp"

#include "usb/libusb_interface.hpp"


namespace zcockpit::cockpit::hardware
{
	Sim737Hardware::Sim737Hardware(InterfaceIT& iit): interface_it(iit)
	{
		interface_it.initialize();

		initialize_iocard_devices();
	}

	Sim737Hardware::~Sim737Hardware()
	{
		interface_it.closeInterfaceITController();
		interface_it.drop();

		if (forward_overhead_card) {
			forward_overhead_card->drop();
			forward_overhead_card = nullptr;
		}
	}

	void Sim737Hardware::initialize_iocard_devices()
	{
		if(LibUsbInterface::is_initialized()) {
			auto available_iocards = IOCards::find_iocard_devices();
			if(available_iocards.contains(IOCards::IOCard_Device::FWD_OVERHEAD)) {
				const auto bus_addr = available_iocards[IOCards::IOCard_Device::FWD_OVERHEAD];
				forward_overhead_card = ForwardOverheadIOCard::create_forward_overhead_iocard(bus_addr);
			}
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
			//if(this->forward_overhead_card->isOpen)
			//{
			//	status = HEALTHY_STATUS;
			//	if(!this->forward_overhead_card->IsInitialized())
			//	{
			//		status = FAILED_STATUS;
			//		LOG() << "IOCards 2: closing down fwd overhead failed init";
			//	}
			//	else
			//	{
			//		// update inputs
			//		this->forward_overhead_card->fastProcessOvrHead();

			//		// send outputs
			//		if(this->forward_overhead_card->send_mastercard() < 0)
			//		{
			//			status = FAILED_STATUS;
			//			LOG() << "IOCards 2: closing down fwd overhead send < 0";
			//			this->forward_overhead_card->closeDown();
			//		}


			//		//if(this->forward_overhead_card->copyIOCardsData() < 0)
			//		//{
			//		//	status = FAILED_STATUS;
			//		//	LOG() << "IOCards 2: closing down fwd overhead copy data < 0";
			//		//	this->forward_overhead_card->closeDown();
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

		// IOCards MIP Overhead
		auto current_iocard_mip_overhead_status =Health::FAILED_STATUS;
		if(MipIOCard::is_running()) {
			current_iocard_mip_overhead_status = Health::HEALTHY_STATUS;
		}
		if(current_iocard_mip_overhead_status != iocard_mip_status) {
			iocard_mip_status = current_iocard_mip_overhead_status;
		}

		// IOCards Forward Overhead
		auto current_iocard_forward_overhead_status =Health::FAILED_STATUS;
		if(ForwardOverheadIOCard::is_running()) {
			current_iocard_forward_overhead_status = Health::HEALTHY_STATUS;
		}
		if(current_iocard_forward_overhead_status != iocard_forward_overhead_status) {
			iocard_forward_overhead_status = current_iocard_forward_overhead_status;
		}

		// IOCards Forward Overhead
		auto current_iocard_rear_overhead_status =Health::FAILED_STATUS;
		if(RearOverheadIOCard::is_running()) {
			current_iocard_rear_overhead_status = Health::HEALTHY_STATUS;
		}
		if(current_iocard_rear_overhead_status != iocard_rear_overhead_status) {
			iocard_rear_overhead_status = current_iocard_rear_overhead_status;
		}

	}

	bool Sim737Hardware::interfaceitMipStatus() const
	{return interfaceIT_MIP_status == Health::HEALTHY_STATUS;}

	bool Sim737Hardware::interfaceitOverheadStatus() const
	{return interfaceIT_overhead_status == Health::HEALTHY_STATUS;}


	bool Sim737Hardware::get_iocard_mip_status() const
	{ return iocard_mip_status == Health::HEALTHY_STATUS;}

	bool Sim737Hardware::get_iocard_forward_overhead_status() const
	{ return iocard_forward_overhead_status == Health::HEALTHY_STATUS;}

	bool Sim737Hardware::get_iocard_rear_overhead_status() const
	{ return iocard_rear_overhead_status == Health::HEALTHY_STATUS;}
}
