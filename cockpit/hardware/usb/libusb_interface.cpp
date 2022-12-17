#include <string>
#include "libusb_interface.hpp"
#include "logger.hpp"

extern logger LOG;
namespace zcockpit::cockpit::hardware
{
	bool LibUsbInterface::libusb_is_initialized = false;



	void LibUsbInterface::exit()
	{
		if(libusb_is_initialized)
		{
			libusb_exit(nullptr);
		}
	}

	bool LibUsbInterface::initialize()
	{
		if(!LibUsbInterface::libusb_is_initialized)
		{
			const int ret = libusb_init(nullptr);
			if(ret < 0)
			{
				LOG() << "Libusb Initialization Error: " << ret;
				LibUsbInterface::libusb_is_initialized = false;

			}
			else {
				LibUsbInterface::libusb_is_initialized = true;
			}
		}
		return libusb_is_initialized;
	}

	bool LibUsbInterface::is_initialized()
	{
		return libusb_is_initialized;
	}
}
