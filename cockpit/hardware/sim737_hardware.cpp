#include "sim737_hardware.hpp"

#include "usb/libusb_interface.hpp"

using namespace zcockpit::common;

namespace zcockpit::cockpit::hardware
{
	bool Sim737Hardware::has_run_for_one_second_ = false;

	Sim737Hardware::Sim737Hardware(AircraftModel& ac_model, InterfaceIT& iit): aircraft_model(ac_model), interface_it(iit)
	{
		interface_it.initialize();

		initialize_devices(ac_model);
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
		if (usb_relay) {
			usb_relay->close_down();
		}

		if (overheadGauges) {
			overheadGauges->closeDown();
		}
		if(mipGauges){
			mipGauges->closeDown();
		}
		if (xpndr) {
			xpndr->closeDown();
			Transponder::drop();
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


	constexpr int USB_RELAY_VID = 0;
	constexpr int USB_RELAY_PID = 5;

	void Sim737Hardware::initialize_devices(AircraftModel& ac_model)
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

			usb_relay = std::make_unique<USBRelay>(ac_model, USB_RELAY_VID, USB_RELAY_PID);
			usb_relay->open();

			// FTD 2XX Devices
			ftd2Devices.get_devices();

			// Flight Illusion Gauges and Radios
			mipGauges = std::make_unique<FiController>(ftd2Devices, ac_model, ONE_SECOND / FIVE_HZ);
			mipGauges->initialize(FiController::mipSerialNumber, ftd2Devices.getDevice(FiController::mipSerialNumber));
			mipGauges->open(FiController::mipSerialNumber);
			build_mip_gauges();
			mipGauges->start_timer(mipGauges->FtHandle());

			overheadGauges = std::make_unique<FiController>(ftd2Devices, ac_model, ONE_SECOND / FIVE_HZ);
			overheadGauges->initialize(FiController::overheadSerialNumber, ftd2Devices.getDevice(FiController::overheadSerialNumber));
			overheadGauges->open(FiController::overheadSerialNumber);
			build_overhead_gauges();
			overheadGauges->start_timer(overheadGauges->FtHandle());

			xpndr = std::make_unique<Transponder>(ftd2Devices);
			xpndr->initialize(Transponder::xponderSerialNumber, ftd2Devices.getDevice(Transponder::xponderSerialNumber));
			xpndr->open(Transponder::xponderSerialNumber);


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

		if (power_is_on && !aircraft_model.z738_ac_power_is_on())
		{
			// turn off
			if (mipGauges->Available())
			{
				mipGauges->updateLights(FiDevice::DISPLAYS_OFF);
				power_is_on = false;
			}
			if (overheadGauges->Available())
			{
				overheadGauges->updateLights(FiDevice::DISPLAYS_OFF);
			}
		}
		else if (!power_is_on && aircraft_model.z738_ac_power_is_on())
		{
			// turn on
			if (mipGauges->Available())
			{
				mipGauges->updateLights(FiDevice::DISPLAYS_ON);
				power_is_on = true;
			}
			if (overheadGauges->Available())
			{
				overheadGauges->updateLights(FiDevice::DISPLAYS_ON);
			}
		}
		if(current_cycle % 2 == 0 && mipGauges->Available())
		{
			mipGauges->updateRadios();
		}


		// MIP IOCards
		//
		if(current_cycle % FIVE_HZ == 0)
		{
			// USB Relay
			if(usb_relay && usb_relay->is_running()) {
				usb_relay->process();
			}

			// MIP IOCard
			if(mip_iocard && mip_iocard->is_okay)
			{	int status;
				while ((status = mip_iocard->receive_mastercard()) > 0) {
					mip_iocard->process_encoders();
				}
				if(status >= 0) {
					mip_iocard->process_mip();
				}

				// send outputs
				mip_iocard->send_mastercard();
			}
		}

		else if(current_cycle % FIVE_HZ == 1)
		{
			if(forward_overhead_iocard && forward_overhead_iocard->is_okay)
			{
				int status;
				while((status = forward_overhead_iocard->receive_mastercard()) > 0)
				{
					forward_overhead_iocard->update_encoders();
				}
				if(status >= 0) {
					// update inputs
					forward_overhead_iocard->process_overhead();
					// send outputs
					forward_overhead_iocard->update_displays();
					// send encoders
					forward_overhead_iocard->process_encoders();
				}
				forward_overhead_iocard->send_mastercard();
			}
		}

		else if(current_cycle % FIVE_HZ == 2)
		{
			if(rear_overhead_iocard && rear_overhead_iocard->is_okay)
			{
				int status;
				while((status = rear_overhead_iocard->receive_mastercard()) > 0)
				{
				}
				if(status >= 0) {
					// update inputs
					rear_overhead_iocard->process_rear_over_head();

				}
				rear_overhead_iocard->send_mastercard();
			}
		}


		else if(current_cycle % FIVE_HZ == 3)
		{
			//
			// Flight Illusions
			//   
			if(overheadGauges->Available())
			{
				overheadGauges->updateGauges();
			}
			if (mipGauges->Available())
			{
				mipGauges->updateGauges();
			}

			//
			// Transponder
			//
			if(xpndr->Available() && aircraft_model.z738_is_available())
			{
				if( aircraft_model.z738_ac_power_is_on()){
					xpndr->check_xpndr_digits();
				}

				xpndr->updatePowerOn();
				xpndr->updateFailed();
				xpndr->updateRply();
				xpndr->requestData();
				xpndr->readXpndr();

				if(init_xpndr)
				{
					init_xpndr = false;
					xpndr->sync_switches();
				}
			}


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

		// USB Relay
		auto current_usb_relay_status = Health::FAILED_STATUS;
		if(!usb_relay->is_running()) {
			if(usb_relay->open()) {
				current_usb_relay_status = Health::HEALTHY_STATUS;
			}
		}
		else {
			if(usb_relay->check_errors()) {
				current_usb_relay_status = Health::HEALTHY_STATUS;

			}
		}
		if(current_usb_relay_status != usb_relay_status) {
			usb_relay_status = current_usb_relay_status;
		}


		//
		// look for new FTD2XX connections
		//
		// Transponder
		auto current_transponder_status = Health::FAILED_STATUS;
		if(!xpndr->Available())
		{
			xpndr->initialize(Transponder::xponderSerialNumber, ftd2Devices.getDevice(Transponder::xponderSerialNumber));
			xpndr->open(Transponder::xponderSerialNumber);
			if(xpndr->Available())
			{
				std::string dev_id = std::to_string(xpndr->devInfo.ID);
				current_transponder_status = Health::HEALTHY_STATUS;
			}
		}
		else
		{
			std::string dev_id = std::to_string(xpndr->devInfo.ID);
			current_transponder_status = Health::HEALTHY_STATUS;
		}
		if(transponder_status != current_transponder_status)
		{
			transponder_status = current_transponder_status;
		}

		//
		// Flight Illusions
		//
		auto current_flight_illusions_overhead_status = Health::FAILED_STATUS;
		if(!overheadGauges->Available())
		{
			// try to reconnect
			overheadGauges->initialize(FiController::overheadSerialNumber, ftd2Devices.getDevice(FiController::overheadSerialNumber));
			overheadGauges->open(FiController::overheadSerialNumber);
			if(overheadGauges->Available())
			{
				std::string hex = std::to_string(overheadGauges->devInfo.ID);
				current_flight_illusions_overhead_status = Health::HEALTHY_STATUS;
			}
		}
		// we are connected
		else
		{
			std::string hex = std::to_string(overheadGauges->devInfo.ID);
			current_flight_illusions_overhead_status = Health::HEALTHY_STATUS;
			overheadGauges->validateGauges();
		}
		if(flight_illusions_overhead_status != current_flight_illusions_overhead_status)
		{
			flight_illusions_overhead_status = current_flight_illusions_overhead_status;
		}
		auto current_flight_illusions_mip_status = Health::FAILED_STATUS;
		if (!mipGauges->Available())
		{
			// try to reconnect
			mipGauges->initialize(FiController::mipSerialNumber, ftd2Devices.getDevice(FiController::mipSerialNumber));
			mipGauges->open(FiController::mipSerialNumber);
			if (mipGauges->Available())
			{
				std::string hex = std::to_string(mipGauges->devInfo.ID);
				current_flight_illusions_mip_status = Health::HEALTHY_STATUS;
			}
		}
		// we are connected
		else
		{
			std::string hex = std::to_string(mipGauges->devInfo.ID);
			current_flight_illusions_mip_status = Health::HEALTHY_STATUS;
			mipGauges->validateGauges();
		}
		if (flight_illusions_mip_status != current_flight_illusions_mip_status)
		{
			flight_illusions_mip_status = current_flight_illusions_mip_status;
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

	bool Sim737Hardware::get_flight_illusion_mip_status() const
	{
		return flight_illusions_mip_status == Health::HEALTHY_STATUS;
	}
	bool Sim737Hardware::get_flight_illusion_overhead_status() const
	{
		return flight_illusions_overhead_status == Health::HEALTHY_STATUS;
	}
	bool Sim737Hardware::get_transponder_status() const
	{
		return transponder_status == Health::HEALTHY_STATUS;
	}


	void Sim737Hardware::build_mip_gauges() const
    {
		// Yaw Damper ID = 188
		// 0 - 935 corresponds to rudder deflection -1.0 to 1.0 -- conver to 1000 - 2000
		// scale factor = 935/ 2.0
		// offset for zero = 0
		// K = 0.8
		// id, gaugeType, scaleFactor, min max, offset, K, Needle CMD
		mipGauges->addGauge(188, FiDevice::YAW_DAMPER, 800/ 2000.0, 0, 935, 0, 0.8, FiDevice::NEW_NEEDLE_VALUE);

		// Flaps  ID = 230
		// 0 - 820 corresponds to ...
		// scale factor = 1.0 
		// offset for zero = 0
		// K = 0.8
		// id, gaugeType, scaleFactor, min max, offset, K, Needle CMD
		mipGauges->addGauge(230, FiDevice::FLAP, 1.0, 0, 820, 0, 0.8, FiDevice::NEW_NEEDLE_VALUE);


		// Brake Pressure ID = 231
		// 85 - 835 corresponds to 0 to 4000 psi -- does work on gauge very well ... using 650/3000 which is the correct value for 3000
		// scale factor = does not wrok ...(835 - 85) / 4000
		// offset for zero = 0
		// K = 0.8
		// id, gaugeType, scaleFactor, min max, offset, K, Needle CMD
		mipGauges->addGauge(231, FiDevice::BRAKE_PRESS, (650) / 3000.0, 0, 835, 0, 0.8, FiDevice::NEW_NEEDLE_VALUE, 50);


		// Capt Clock ID = 160
		
		// FO Clock ID = 240

		//
		// RADIOS
		//

		// Capt Comm ID = 120
		mipGauges->addRadio(120, FiDevice::CAPT_COMM, 1.0, 0, 200, FiDevice::STATUS);


		// FO Comm ID = 122
		mipGauges->addRadio(122, FiDevice::FO_COMM, 1.0, 0, 200, FiDevice::STATUS);

		// Capt NAV ID = 121
		mipGauges->addRadio(121, FiDevice::CAPT_NAV, 1.0, 0, 200, FiDevice::STATUS);


		// FO NAV ID = 123
		mipGauges->addRadio(123, FiDevice::FO_NAV, 1.0, 0, 200, FiDevice::STATUS);


		// ADF ID = 124
		mipGauges->addRadio(124, FiDevice::ADF, 1.0, 0, 200, FiDevice::STATUS);

	}


	void Sim737Hardware::build_overhead_gauges() const
	{
		// EGT ID = 180 
		// 0 - 80 corresponds to 0 - 1,100 degrees
		// scale factor = 6400.0/ 1100.0
		// offset for zero = 0
		// K = 0.8
		// id, gaugeType, scaleFactor, min max, offset, K, Needle CMD
		overheadGauges->addGauge(180, FiDevice::APU_EGT, 6400.0 / 1100.0, 0, 800, 0, 0.8, FiDevice::NEW_NEEDLE_VALUE);

		// Diff Press/Cabin Alt = 181
		// Diff Press (large needle) 0 - 925  corresponds to 0-10
		// scale factor = 925.0/ 10.0
		// offset for zero = 0
		// K = 0.8
		// id, gaugeType, scaleFactor, min max, offset, K, Needle CMD
		overheadGauges->addGauge(181, FiDevice::PRESS_DIFF, 925.0 / 10.0, 0, 925, 0, 0.8, FiDevice::NEW_SECOND_VALUE);

		// Same Gauge 181
		// Cabin Alt (small needle )0 - 885 corresponds to 0 to 50,000
		// scale factor = 4800/50000.0
		// offset for zero = 0
		// K = 0.8
		// id, gaugeType, scaleFactor, min max, offset, K, Needle CMD
		overheadGauges->addGauge(181, FiDevice::CABIN_ALT, 4800.0 / 50000.0, 0, 885, 0, 0.8, FiDevice::NEW_NEEDLE_VALUE);


		// Cabin Climb = 182
		// neg - mid - pos
		//  0  - 472 - 945   corresponds to -4,000 to 4,000
		// scale factor = 945/8000.0
		// offset for zero = 472
		// K = 0.8
		// id, gaugeType, scaleFactor, min max, offset, K, Needle CMD
		overheadGauges->addGauge(182, FiDevice::CABIN_CLIMB, 885.0 / 8000.0, 0, 945, 472, 0.8, FiDevice::NEW_NEEDLE_VALUE);

		// Pack Left/Right = 183
		// 0 - 755  corresponds to 0 - 80 psi
		// scale factor = 755.0/65.0
		// offset for zero = 0
		// K = 0.8
		// id, gaugeType, scaleFactor, min max, offset, K, Needle CMD
		overheadGauges->addGauge(183, FiDevice::LEFT_DUCT, 855.0 / 80.0, 0, 755, 0, 0.8, FiDevice::NEW_NEEDLE_VALUE);
		overheadGauges->addGauge(183, FiDevice::RIGHT_DUCT, 855.0 / 80.0, 0, 755, 0, 0.8, FiDevice::NEW_SECOND_VALUE);


		// Fuel Temp = 184
		// neg  - mid - pos
		// 115 - 472 - 815  corresponds -50c -- 0 -- 50c
		// scale factor = (815-115)/100.0
		// offset for zero = 472
		// K = 0.8
		// id, gaugeType, scaleFactor, min max, offset, K, Needle CMD
		overheadGauges->addGauge(184, FiDevice::FUEL_TEMP, 700.0 / 100.0, 115, 815, 472, 0.8, FiDevice::NEW_NEEDLE_VALUE);

		// Cabin Temp  = 185  -- Air_Temperature_neddle_status
		// 0 - 765 corresponds to 0 - 100
		// scale factor = 765.0/100.0
		// offset for zero = 0
		// K = 0.8
		// id, gaugeType, scaleFactor, min max, offset, K, Needle CMD
		overheadGauges->addGauge(185, FiDevice::CABIN_TEMP, 1760.0 / 200.0, 0, 765, 0, 0.8, FiDevice::NEW_NEEDLE_VALUE);

		// Oxygen Pressure = 186
		// 0 - 843  corresponds  0 - 2000
		// scale factor = 843.0/2000
		// ifly range 0 - 2,000
		// offset for zero = 0
		// K = 0.8
		// id, gaugeType, scaleFactor, min max, offset, K, Needle CMD
		overheadGauges->addGauge(186, FiDevice::CREW_OXYGEN, 843.0 / 2000, 0, 843, 0, 0.8, FiDevice::NEW_NEEDLE_VALUE);
	}
}
