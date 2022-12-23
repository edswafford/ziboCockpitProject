#ifndef __ZCockpitApp__
#define __ZCockpitApp__
#include <mutex>
#include <thread>

#include "mainWindow.h"


//
// Simple Ini
//
#include "../third_party/SimpleIni/SimpleIni.h"


namespace zcockpit
{
	namespace cockpit
	{
		namespace hardware
		{
			class OvrheadIOCards;
		}
	}
}

namespace zcockpit::cockpit::gui
{
	class ZcockpitApp : public wxApp
	{
	public:
		ZcockpitApp();

		// override base class virtuals
		// ----------------------------

		// this one is called on application startup and is a good place for the app
		// initialization (doing it here and not in the ctor allows to have an error
		// return: if OnInit() returns false, the application terminates)
		virtual bool OnInit() override;
		virtual int OnExit() override;
		void update_counters();


		static constexpr int HZ_40 = 25;
		static constexpr int HZ_20 = 50; // 50ms
		static constexpr int HZ_10 = 100; // 100ms
		static constexpr int HZ_5 = 200; // 200ms
		static constexpr int HZ_1 = 1000; // 1000ms



	private:
		CSimpleIniA ini;
		MainWindow* main_window{nullptr};

		std::unique_ptr<zcockpit::cockpit::hardware::OvrheadIOCards> ovrheadIOCards;

		std::mutex sim737_timer_mutex;
		std::mutex timer_done_mutex;
		std::condition_variable condition;

		std::thread timer_thread;
		bool stop_timer{true};
		bool timer_has_stopped{true};
		bool zibo_status{false};
		bool interfaceit_mip_status{false};
		bool interfaceit_overhead_status{false};
		void timer();
		void initialize_iocard_devices();
		void initFwdOverheadCards(std::string bus_address);

		// Cycle Counters
		int ONE_SECOND;
		int FIVE_SECONDS;
		int FIVE_HZ;
		int TEN_HZ;
		int TWENTY_HZ;
		int UPDATE_RATE;

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