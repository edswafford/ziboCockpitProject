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

	private:
		static bool libusb_is_initialized;

	};
}
