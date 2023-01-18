#pragma once
#include "InterfaceIT/interfaceit.hpp"
#include "ioCards/mip_iocard.hpp"
#include "ioCards/fwd_overhead_iocard.hpp"
#include "ioCards/rear_overhead_iocard.hpp"
#include "health.hpp"
#include "logger.hpp"
#include "flight_illusion/ficontroller.hpp"
#include "ftd2xx/ftd2xxdevices_wrapper.hpp"
#include "transponder/transponder.hpp"
#include "usb/usbrelay.hpp"


extern logger LOG;
namespace zcockpit::cockpit::hardware
{
	class Sim737Hardware
	{
	public:

		Sim737Hardware(AircraftModel& ac_model, InterfaceIT& iit);

		~Sim737Hardware();

		void initialize_devices(AircraftModel& ac_model);

		void do_updates(int current_cycle);

		void checkConnections();

		[[nodiscard]] bool interfaceitMipStatus() const;
		[[nodiscard]] bool interfaceitOverheadStatus() const;
		[[nodiscard]] bool get_iocard_mip_status() const;
		[[nodiscard]] bool get_iocard_forward_overhead_status() const;
		[[nodiscard]] bool get_iocard_rear_overhead_status() const;

		[[nodiscard]] bool get_flight_illusion_overhead_status() const;
		[[nodiscard]] bool get_flight_illusion_mip_status() const;
		[[nodiscard]] bool get_transponder_status() const;

		static bool has_run_for_one_second(){return has_run_for_one_second_;}
	private:
		void build_overhead_gauges() const;
		void build_mip_gauges() const;

		AircraftModel& aircraft_model;

		bool power_is_on{ false };
		bool init_xpndr{true};

		static bool has_run_for_one_second_;

		InterfaceIT& interface_it;
		
		std::unique_ptr<MipIOCard> mip_iocard;
		std::unique_ptr<ForwardOverheadIOCard> forward_overhead_iocard;
		std::unique_ptr<RearOverheadIOCard> rear_overhead_iocard;
		std::unique_ptr<USBRelay> usb_relay;

		Ftd2xxDevices* ftd2Devices{nullptr};
		std::unique_ptr<FiController> overheadGauges;
		std::unique_ptr<FiController> mipGauges;
		std::unique_ptr<Transponder> xpndr;

		void do_usb_work();
		void start_event_thread();
		bool event_thread_run{false};
		bool event_thread_has_stopped{true};
		std::condition_variable condition;
		std::mutex event_thread_mutex;
		std::mutex event_thread_done_mutex;
		std::thread event_thread;


		Health interfaceIT_MIP_status{Health::UNKNOWN_STATUS};
		Health interfaceIT_overhead_status{Health::UNKNOWN_STATUS};
		Health iocard_mip_status{Health::UNKNOWN_STATUS};
		Health iocard_forward_overhead_status{Health::UNKNOWN_STATUS};
		Health iocard_rear_overhead_status{Health::UNKNOWN_STATUS};
		Health usb_relay_status{Health::UNKNOWN_STATUS};

		Health transponder_status{Health::UNKNOWN_STATUS};
		Health flight_illusions_overhead_status{Health::UNKNOWN_STATUS};
		Health flight_illusions_mip_status{Health::UNKNOWN_STATUS};
	};
}