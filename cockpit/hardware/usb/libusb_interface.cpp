#include <string>
#include "libusb_interface.hpp"

#include "libusbi.h"
#include "logger.hpp"

extern logger LOG;
namespace zcockpit::cockpit::hardware
{
	bool LibUsbInterface::libusb_is_initialized = false;
	libusb_context* LibUsbInterface::ctx = nullptr;

	void libusb_logger(libusb_context* ctx, enum libusb_log_level level, const char* str){
		LOG() << "LIBUSB:: " << str;
	}
	void LibUsbInterface::exit()
	{
		if(libusb_is_initialized)
		{
			libusb_exit(ctx);
		}
	}

	bool LibUsbInterface::initialize()
	{
		if(!LibUsbInterface::libusb_is_initialized)
		{
			int ret = libusb_init(&(LibUsbInterface::ctx));
			if(ret < 0)
			{
				LOG() << "Libusb Initialization Error: " << ret;
				LibUsbInterface::libusb_is_initialized = false;

			}
			else {
				ret = libusb_set_option(LibUsbInterface::ctx, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_WARNING);
				if (ret < 0) {
					LOG() << "Cannot set libusb Option DEBUG";
				}
				else {
					libusb_set_log_cb(LibUsbInterface::ctx, libusb_logger, LIBUSB_LOG_CB_CONTEXT);
				}
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
