#ifndef __ZCockpitApp__
#define __ZCockpitApp__
#include <mutex>
#include <thread>

#include "mainWindow.h"


//
// Simple Ini
//
#include "../third_party/SimpleIni/SimpleIni.h"



namespace zcockpit::cockpit::gui
{
	class ZcockpitApp : public wxApp
	{
	public:
		ZcockpitApp();
		~ZcockpitApp();

		// override base class virtuals
		// ----------------------------

		// this one is called on application startup and is a good place for the app
		// initialization (doing it here and not in the ctor allows to have an error
		// return: if OnInit() returns false, the application terminates)
		virtual bool OnInit() override;
		virtual int OnExit() override;
		void update_counters();




	private:
		CSimpleIniA ini;
		MainWindow* main_window{nullptr};


		std::mutex sim737_timer_mutex;
		std::mutex timer_done_mutex;
		std::condition_variable condition;

		std::thread timer_thread;
		bool stop_timer{true};
		bool timer_has_stopped{true};
		bool zibo_status{false};
		bool throttle_status{false};
		bool interfaceit_mip_status{false};
		bool interfaceit_overhead_status{false};
		bool iocard_mip_status{false};
		bool iocard_forward_overhead_status{false};
		bool iocard_rear_overhead_status{false};
		bool flight_illusion_mip_status{false};
		bool flight_illusion_overhead_status{false};
		bool transponder_status{false};

		void timer();

		// Cycle Counters

		int current_cycle;
		int five_hz_counter;
		int ten_hz_counter;
		int twenty_hz_counter;
		int five_second_counter;

		std::chrono::high_resolution_clock::time_point start;
		std::chrono::duration<double, std::milli> elapsed{0};
	};
}
#endif