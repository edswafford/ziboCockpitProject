#ifndef USBWORKER_H
#define USBWORKER_H

#include <mutex>

#include <libusb.h>
#include <condition_variable>
#include "../common/logger.hpp"

extern logger LOG;

namespace zcockpit::cockpit::hardware
{

	class UsbWorker
	{
	public:
		static const int READ_BUFFER_SIZE;
		static const int WRITE_BUFFER_SIZE;

		UsbWorker(libusb_device* usbDev, libusb_device_handle* handle, struct libusb_context* ctx, std::string name);
		~UsbWorker();
		void initDevice();

		int releaseAndCloseUsb();
		void abort();
		void runStop();
		int is_running();

		int write_usb(unsigned char* bytes, int size);
		int read_usb(unsigned char* bytes, int size);

		void update_name(std::string new_name) { name = new_name; }
		void process();
		std::string name;

	private:
		UsbWorker()
		{
		};



		libusb_device* usbDev;
		libusb_device_handle* handle; /* device handle */
		struct libusb_context* ctx;

		unsigned char epIn; /* input endpoint */
		unsigned char epOut; /* output endpoint */
		int iocardStatus; /* device status (-1: not present, 0: present and ready) */

		std::mutex usb_mutex;
		std::mutex readMutex;
		std::mutex writeMutex;
		std::condition_variable condition;

		bool run;
		bool _abort;


		uint16_t inBufferSize; /* input endpoint buffer size */
		uint16_t outBufferSize; /* output endpoint buffer size */
		unsigned char* inBuffer; /* pointer to input endpoint buffer */
		unsigned char* inBuffer_loc;
		unsigned char* outBuffer; /* pointer to output endpoint buffer */
		unsigned char* outBuffer_loc;
		unsigned char* readBuffer; /* pointer to read buffer */
		unsigned char* readBuffer_loc;
		unsigned char* writeBuffer; /* pointer to write buffer */
		unsigned char* writeBuffer_loc;
		int readLeft; /* counter for yet to be read data packets */
		int writeLeft; /* counter for yet to be written data packets */
		int iswriting; /* device is currently processing write operation */
		int thread_exit_code; /* thread exit code */


		void LIBUSB_CALL read_callback_cpp(struct libusb_transfer* transfer);
		void static LIBUSB_CALL read_callback(struct libusb_transfer* transfer);

		void LIBUSB_CALL write_callback_cpp(struct libusb_transfer* transfer);
		void static LIBUSB_CALL write_callback(struct libusb_transfer* transfer);


		struct libusb_transfer* readTransfer; /* libusb read transfer handle */
		struct libusb_transfer* writeTransfer;/* libusb write transfer handle */
	};
}
#endif // USBWORKER_H
