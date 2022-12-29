#pragma once
#include <mutex>
#include <thread>

#include "libusb.h"

namespace zcockpit::cockpit::hardware
{
	class LibUsbInterface
	{
	public:
		static void exit();
		static bool initialize();
		static bool is_initialized();
		static libusb_context* ctx;

		static void do_usb_work();
		static void start_event_thread();
		static bool event_thread_run;
		static bool event_thread_has_stopped;
		static std::condition_variable condition;
		static std::mutex event_thread_mutex;
		static std::mutex event_thread_done_mutex;
		static std::thread event_thread;
	private:
		static bool libusb_is_initialized;

	};
}
