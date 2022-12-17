#pragma once
#include "libusb.h"

namespace zcockpit::cockpit::hardware
{
	class LibUsbInterface
	{
	public:
		static void exit();
		static bool initialize();
		static bool is_initialized();
	private:
		static bool libusb_is_initialized;
	};
}
