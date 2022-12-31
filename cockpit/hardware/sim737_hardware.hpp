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

		Sim737Hardware(AircraftModel& ac_model, InterfaceIT& iit);

		~Sim737Hardware();

		void initialize_iocard_devices(AircraftModel& ac_model);

		void do_updates(int current_cycle);

		void checkConnections();

		[[nodiscard]] bool interfaceitMipStatus() const;
		[[nodiscard]] bool interfaceitOverheadStatus() const;
		[[nodiscard]] bool get_iocard_mip_status() const;
		[[nodiscard]] bool get_iocard_forward_overhead_status() const;
		[[nodiscard]] bool get_iocard_rear_overhead_status() const;
		static bool has_run_for_one_second(){return has_run_for_one_second_;}
	private:
		static bool has_run_for_one_second_;

		InterfaceIT& interface_it;
		
		std::unique_ptr<MipIOCard> mip_iocard;
		std::unique_ptr<ForwardOverheadIOCard> forward_overhead_iocard;
		std::unique_ptr<RearOverheadIOCard> rear_overhead_iocard;

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
	};
}