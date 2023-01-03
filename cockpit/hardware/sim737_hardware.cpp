#include "sim737_hardware.hpp"

#include "usb/libusb_interface.hpp"

using namespace zcockpit::common;

namespace zcockpit::cockpit::hardware
{
	bool Sim737Hardware::has_run_for_one_second_ = false;

	Sim737Hardware::Sim737Hardware(AircraftModel& ac_model, InterfaceIT& iit): interface_it(iit)
	{
		interface_it.initialize();

		initialize_iocard_devices(ac_model);
	}

	Sim737Hardware::~Sim737Hardware()
	{
		interface_it.closeInterfaceITController();
		interface_it.drop();

		{
			std::lock_guard<std::mutex> guard(event_thread_mutex);
			if (event_thread_run) {
				event_thread_run = false;
			}
		}

		if (mip_iocard) {
			mip_iocard->drop();
			mip_iocard = nullptr;
		}
		if (forward_overhead_iocard) {
			forward_overhead_iocard->drop();
			forward_overhead_iocard = nullptr;
		}
		if (rear_overhead_iocard) {
			rear_overhead_iocard->drop();
			rear_overhead_iocard = nullptr;
		}

		std::unique_lock<std::mutex> lk(event_thread_done_mutex);
			condition.wait(lk, [this]
				{
					return (this->event_thread_has_stopped);
				});
		if(event_thread.joinable()) {
			event_thread.join();
		}

	}
		// Runs in libusb thread
	void Sim737Hardware::do_usb_work()
	{
		event_thread_has_stopped = false;
		while (true) {
			{
				std::lock_guard<std::mutex> guard(event_thread_mutex);
				if (!event_thread_run)
				{
					LOG() << "libusb event thread stopping";
					event_thread_has_stopped = false;
					break;
				}
			}

			struct timeval tv = { 1, 0 };
			const auto ret = libusb_handle_events_timeout(LibUsbInterface::ctx, &tv);
			if (ret < 0) {
				event_thread_run = false;
				break;
		    }
		}
		std::unique_lock<std::mutex> lk( event_thread_done_mutex);
		event_thread_has_stopped = true;
		lk.unlock();
		condition.notify_one();
	}
	void Sim737Hardware::start_event_thread()
	{
		event_thread_run = true;
		event_thread = std::thread(&Sim737Hardware::do_usb_work, this);
	}



	void Sim737Hardware::initialize_iocard_devices(AircraftModel& ac_model)
	{
		if(LibUsbInterface::is_initialized()) {
			auto available_iocards = IOCards::find_iocard_devices();
			if(available_iocards.contains(IOCards::IOCard_Device::MIP)) {
				const auto bus_addr = available_iocards[IOCards::IOCard_Device::MIP];
				mip_iocard = MipIOCard::create_iocard(ac_model, bus_addr);
			}
			if(available_iocards.contains(IOCards::IOCard_Device::FWD_OVERHEAD)) {
				const auto bus_addr = available_iocards[IOCards::IOCard_Device::FWD_OVERHEAD];
				forward_overhead_iocard = ForwardOverheadIOCard::create_iocard(ac_model,bus_addr);
			}
			if(available_iocards.contains(IOCards::IOCard_Device::REAR_OVERHEAD)) {
				const auto bus_addr = available_iocards[IOCards::IOCard_Device::REAR_OVERHEAD];
				rear_overhead_iocard = RearOverheadIOCard::create_iocard(ac_model, bus_addr);
			}

			// Applications should not start the event thread until after their first call to libusb_open()
			start_event_thread();
			LOG() << "libusb Event thread is running";
		}
	}



	void Sim737Hardware::do_updates(int current_cycle)
	{
		if(current_cycle >= ONE_SECOND) {
			Sim737Hardware::has_run_for_one_second_ = true;
		}

		//if (powerIsOn && ifly737->electrical_power_state == 0)
		//{
		//	// turn off
		//	if (mipGauges->Available())
		//	{
		//		mipGauges->updateLights(FiDevice::DISPLAYS_OFF);
		//		powerIsOn = false;
		//	}
		//}
		//else if (!powerIsOn && ifly737->electrical_power_state != 0)
		//{
		//	// turn on
		//	if (mipGauges->Available())
		//	{
		//		mipGauges->updateLights(FiDevice::DISPLAYS_ON);
		//		powerIsOn = true;
		//	}
		//}

		//if(current_cycle % 2 == 0 && mipGauges->Available())
		//{
		//	mipGauges->updateRadios();
		//}




		// MIP IOCards
		//
		mip_iocard->processEncoders();  // update every cycle

		if(current_cycle % FIVE_HZ == 0)
		{
			if(mip_iocard && mip_iocard->is_okay)
			{
				mip_iocard->receive_mastercard();

				// update inputs
				mip_iocard->processMIP();

				// send outputs
				mip_iocard->send_mastercard();

				// copy current to previous
				if(mip_iocard->copyIOCardsData() < 0)
				{
					LOG() << "IOCards 2: closing down mip copy data < 0";
					mip_iocard->close_down();
				}
			}
		}

		else if(current_cycle % FIVE_HZ == 1)
		{
			//if(this->forward_overhead_iocard->isOpen)
			//{
			//	status = HEALTHY_STATUS;
			//	if(!this->forward_overhead_iocard->IsInitialized())
			//	{
			//		status = FAILED_STATUS;
			//		LOG() << "IOCards 2: closing down fwd overhead failed init";
			//	}
			//	else
			//	{
			//		// update inputs
			//		this->forward_overhead_iocard->fastProcessOvrHead();

			//		// send outputs
			//		if(this->forward_overhead_iocard->send_mastercard() < 0)
			//		{
			//			status = FAILED_STATUS;
			//			LOG() << "IOCards 2: closing down fwd overhead send < 0";
			//			this->forward_overhead_iocard->closeDown();
			//		}


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

		else if(current_cycle % FIVE_HZ == 2)
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


		else if(current_cycle % FIVE_HZ == 3)
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
