#include <string>
#include "libusb_interface.hpp"

#include "libusbi.h"
#include "logger.hpp"

extern logger LOG;
namespace zcockpit::cockpit::hardware
{
	bool LibUsbInterface::libusb_is_initialized = false;
	libusb_context* LibUsbInterface::ctx = nullptr;
	bool LibUsbInterface::event_thread_run = false;
	std::thread LibUsbInterface::event_thread;
	std::mutex LibUsbInterface::event_thread_mutex;
	std::mutex LibUsbInterface::event_thread_done_mutex;
	bool LibUsbInterface::event_thread_has_stopped = true;
	std::condition_variable LibUsbInterface::condition;

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

	// Runs in libusb thread
	void LibUsbInterface::do_usb_work()
	{
		LibUsbInterface::event_thread_has_stopped = false;
		while (true) {
			{
				std::lock_guard<std::mutex> guard(LibUsbInterface::event_thread_mutex);
				if (!LibUsbInterface::event_thread_run)
				{
					LOG() << "libusb event thread stopping";
					LibUsbInterface::event_thread_has_stopped = false;
					break;
				}
			}

			struct timeval tv = { 1, 0 };
			const auto ret = libusb_handle_events_timeout(LibUsbInterface::ctx, &tv);
			if (ret < 0) {
				LibUsbInterface::event_thread_run = false;
				break;
		    }
		}
		std::unique_lock<std::mutex> lk( LibUsbInterface::event_thread_done_mutex);
		LibUsbInterface::event_thread_has_stopped = true;
		lk.unlock();
		condition.notify_one();
	}
	void LibUsbInterface::start_event_thread()
	{
		LibUsbInterface::event_thread_run = true;
		LibUsbInterface::event_thread = std::thread(&LibUsbInterface::do_usb_work);
	}
}
