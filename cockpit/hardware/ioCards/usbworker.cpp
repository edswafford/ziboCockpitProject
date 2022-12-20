#include "usbworker.h"

namespace zcockpit::cockpit::hardware
{
	const int UsbWorker::READ_BUFFER_SIZE = 8096;
	const int UsbWorker::WRITE_BUFFER_SIZE = 1024;

	UsbWorker::UsbWorker(libusb_device* dev, libusb_device_handle* handle, struct libusb_context* ctx, std::string name) : name(name)
	{
		this->usbDev = dev;
		this->handle = handle;
		this->ctx = ctx;

		run = false;
		_abort = false;
		iocardStatus = -1;

		epIn = 0xFF;
		epOut = 0xFF;
		readTransfer = nullptr;
		writeTransfer = nullptr;
		inBufferSize = 0x0;
		outBufferSize = 0x0;
		inBuffer = nullptr;
		inBuffer_loc = nullptr;
		outBuffer = nullptr;
		outBuffer_loc = nullptr;
		readBuffer = nullptr;
		readBuffer_loc = nullptr;
		writeBuffer = nullptr;
		writeBuffer_loc = nullptr;
		readLeft = 0;
		writeLeft = 0;
		iswriting = 0;
		thread_exit_code = 0;

		//initDevice();
	}

	UsbWorker::~UsbWorker()
	{
		int ret = libusb_release_interface(handle, 0);
		if(ret != 0)
		{
			LOG() << "Can't release interface";
		}
		libusb_close(handle); // This wakes up libusb_handle_events()
		if(inBuffer_loc != nullptr) delete inBuffer_loc;
		if(outBuffer_loc != nullptr) delete outBuffer_loc;
		if(readBuffer_loc != nullptr) delete readBuffer_loc;
		if(writeBuffer_loc != nullptr) delete writeBuffer_loc;
	}

	void UsbWorker::runStop()
	{
		// create and lock
		{
			std::lock_guard<std::mutex> guard(usb_mutex);
			run = !run;
		}
		condition.notify_one();
	}

	int UsbWorker::is_running()
	{
		std::unique_lock<std::mutex> lock(usb_mutex, std::defer_lock);
		if (!lock.try_lock())
		{
			// cannot get lock
			return -1;
		}
		if(run)
		{
			return 1;
		}
		return 0;
	}

	void UsbWorker::abort()
	{
		// create and lock
		{
			std::lock_guard<std::mutex> guard(usb_mutex);
			_abort = true;
		}
		condition.notify_one();
	}

	// --- PROCESS ---
	// Start processing data.
	void UsbWorker::process()
	{
		if(usbDev != nullptr && handle != nullptr)
		{
			// create and lock
			{
				bool abort = false;
				{
					std::unique_lock<std::mutex> lk(usb_mutex);
					if (!run && !_abort)
					{
						condition.wait(lk, [this]
							{
								return (this->run || this->_abort);
							});
						abort = _abort;
					}
				}
				if (!abort)
				{
					while (true)
					{
						{
							std::lock_guard<std::mutex> guard(usb_mutex);
							if (_abort)
							{
								break;
							}
						}
						libusb_handle_events(ctx);
					}
				}
			}
		}
		{
			std::lock_guard<std::mutex> guard(usb_mutex);
			run = false;
		}
	}

	void UsbWorker::initDevice()
	{
		struct libusb_config_descriptor* config;
		const struct libusb_interface* iface;
		const struct libusb_interface_descriptor* altsetting;
		int ret;
		int result = 0;

		/* retrieve device information */
		ret = libusb_get_config_descriptor(usbDev, 0, &config);
		if(ret < 0)
		{
			// LIBUSB_INTERFACE: Failed to get config descriptor
			iocardStatus = -1;
			result = -5;
		}
		else
		{
			iocardStatus = 0;
			iface = &config->interface[0];
			altsetting = &iface->altsetting[0];

			/* claim device interface */
			ret = libusb_claim_interface(handle, 0);
			if(ret < 0)
			{
				// LIBUSB_INTERFACE: Could not claim interface 
				iocardStatus = -1;
			}
			else
			{
				UsbWorker* myself = this;
				for(int ep_idx = 0; ep_idx < altsetting->bNumEndpoints; ep_idx++)
				{
					const struct libusb_endpoint_descriptor* ep = &altsetting->endpoint[ep_idx];

					/* cycle through endpoints and set up asynchronous transfers */
					if(ep->bEndpointAddress >= 0x80)
					{
						/* input endpoint, usually 0x81 */
						epIn = ep->bEndpointAddress;
						inBufferSize = ep->wMaxPacketSize;

						readTransfer = libusb_alloc_transfer(0);
						if(!readTransfer)
						{
							result = -6;
						}
						inBuffer_loc = inBuffer = new unsigned char[inBufferSize];
						readBuffer_loc = readBuffer = new unsigned char[READ_BUFFER_SIZE];
						libusb_fill_interrupt_transfer(readTransfer, handle, epIn, inBuffer, inBufferSize, read_callback, this, 0);
						if(libusb_submit_transfer(readTransfer) < 0)
						{
							result = -7;
						}
					}
					else
					{
						/* output endpoint, usually: 0x01 */
						epOut = ep->bEndpointAddress;
						outBufferSize = ep->wMaxPacketSize;

						writeTransfer = libusb_alloc_transfer(0);
						if(!writeTransfer)
						{
							result = -8;
						}
						outBuffer_loc = outBuffer = new unsigned char[outBufferSize];
						writeBuffer_loc = writeBuffer = new unsigned char[WRITE_BUFFER_SIZE];
						libusb_fill_interrupt_transfer(writeTransfer, handle, epOut, outBuffer, outBufferSize, write_callback, this, 0);
					}
				}
			}
			libusb_free_config_descriptor(config);
		}
	}

	void LIBUSB_CALL UsbWorker::read_callback(struct libusb_transfer* transfer)
	{
		// explicitly cast to a pointer to TClassA
		UsbWorker* mySelf = reinterpret_cast<UsbWorker*>(transfer->user_data);
		mySelf->read_callback_cpp(transfer);
	}

	void LIBUSB_CALL UsbWorker::read_callback_cpp(struct libusb_transfer* transfer)
	{
		// get user data 
		int* number = (int*)transfer->user_data;

		if(transfer->status != LIBUSB_TRANSFER_COMPLETED)
		{
			/* read callback was not successful: exit! */
			readTransfer = nullptr;
			thread_exit_code = 1;
			return;
		}
		else
		{
			/* read callback was successful: fill read buffer to the right of the last position */
			if(inBufferSize > 0)
			{
				{
					std::lock_guard<std::mutex> guard(readMutex);
					if((readLeft + inBufferSize) <= READ_BUFFER_SIZE)
					{
						memcpy(readBuffer, inBuffer, inBufferSize);
						readLeft += inBufferSize;
						readBuffer += inBufferSize;
					}
					else
					{
						LOG() << "IOCards read buffer full";
					}
				}
			}

			if(libusb_submit_transfer(readTransfer) < 0)
			{
				thread_exit_code = 1;
			}
		}
		return;
	}

	void LIBUSB_CALL UsbWorker::write_callback(struct libusb_transfer* transfer)
	{
		// explicitly cast to a pointer to TClassA
		UsbWorker* mySelf = reinterpret_cast<UsbWorker*>(transfer->user_data);
		mySelf->write_callback_cpp(transfer);
	}

	void LIBUSB_CALL UsbWorker::write_callback_cpp(struct libusb_transfer* transfer)
	{
		// get user data
		int* number = (int*)transfer->user_data;

		if(transfer->status != LIBUSB_TRANSFER_COMPLETED)
		{
			/* write callback was not successful: exit! */
			writeTransfer = nullptr;
			thread_exit_code = 1;
			return;
		}
		else
		{
			/* write callback was successful */

			/* resubmit write call if there is new data in write buffer */
			{
				std::lock_guard<std::mutex> guard(writeMutex);
				if(writeLeft > 0)
				{
					{
						std::lock_guard<std::mutex> guard(usb_mutex);
						/* decrease write buffer position and count */
						writeLeft -= outBufferSize;
						writeBuffer -= outBufferSize;
						/* copy rightmost write buffer to output buffer */
						memcpy(outBuffer, writeBuffer, outBufferSize);
					}

					if(libusb_submit_transfer(writeTransfer) < 0)
					{
						thread_exit_code = 1;
					}
				}
				else
				{
					/* reset write flag so that new write submission can be done */
					iswriting = 0;
				}
			}
		}
		return;
	}


	int UsbWorker::write_usb(unsigned char* bytes, int size)
	{
		int result = 0;

		if(iocardStatus != 0)
		{
			// LIBUSB_INTERFACE: device is not ready or not connected. 
			result = -1;
			return result;
		}

		if(outBufferSize != size)
		{
			//  LIBUSB_INTERFACE: device has write buffer size mismatch
			result = -2;
			return result;
		}

		bool can_submit = false;
		{
			std::lock_guard<std::mutex> guard(writeMutex);
			if((writeLeft + outBufferSize) <= WRITE_BUFFER_SIZE)
			{
				/* shift existing write buffer to the right */
				if(writeLeft > 0)
				{
					memmove(writeBuffer - writeLeft + outBufferSize, writeBuffer - writeLeft, writeLeft);
				}
				/* fill start of write buffer with new write data */
				memcpy(writeBuffer - writeLeft, bytes, outBufferSize);
				/* increase write buffer position and counter */
				writeBuffer += outBufferSize;
				writeLeft += outBufferSize;


				/* do not fill the output buffer while the past write call is not over */
				if(!iswriting)
				{
					/* submit asynchronous write call if this is the first write call to the device */
					if(writeLeft > 0)
					{
						writeLeft -= outBufferSize;
						writeBuffer -= outBufferSize;
						memcpy(outBuffer, writeBuffer, outBufferSize);

						/* submit first asynchronous write call */
						iswriting = 1;
						can_submit = true;
					}
					result = outBufferSize;
				}
			}
		}
		if (can_submit) {
			if (libusb_submit_transfer(writeTransfer) < 0) {
				thread_exit_code = 1;
			}
		}
		return result;
	}

	int UsbWorker::read_usb(unsigned char* bytes, int size)
	{
		int result = 0;

		if(iocardStatus != 0)
		{
			// LIBUSB_INTERFACE: device is not ready or not connected.
			result = -1;
			return result;
		}

		if(inBufferSize != size)
		{
			// LIBUSB_INTERFACE: device has read buffer size mismatch
			result = -2;
			return result;
		}
		{
			std::lock_guard<std::mutex> guard(readMutex);
			if(readLeft > 0)
			{
				/* read from start of read buffer */
				memcpy(bytes, readBuffer - readLeft, inBufferSize);
				/* shift remaining read buffer to the left */
				if(readLeft - inBufferSize > 0)
				{
					memmove(readBuffer - readLeft,
					        readBuffer - readLeft + inBufferSize,
					        readLeft - inBufferSize);
				}
				/* decrease read buffer position and counter */
				readLeft -= inBufferSize;
				readBuffer -= inBufferSize;
				result = inBufferSize;
			}
		}
		return result;
	}
}