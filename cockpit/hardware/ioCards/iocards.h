#ifndef IOCARDS_H
#define IOCARDS_H

#include <chrono>

#include <mutex>
#include <thread>
#include <string>

#include "libusb.h"

//#include "usbworker.h"

//#include "../DeleteMeHotkey.h"

namespace zcockpit::cockpit::hardware
{


	typedef struct masterCard_input_state_struct
	{
		int input;
		int value;
		bool has_changed;
	} masterCard_input_state;


	//struct OnOffKeyCommand;
	//using namespace std;

	struct IOCards_bus_and_addr
	{
		int bus;
		int address;
	};

	class IOCards
	{
	public:

		static constexpr int MAX_IOCARDS = 10;
		static IOCards_bus_and_addr iocards_device_list[MAX_IOCARDS];

		const enum IOCard_Device
		{
			MIP,
			FWD_OVERHEAD,
			REAR_OVERHEAD,
			UNKNOWN,
		};

		static constexpr int NUM_INPUTS = 72; /* number of binary input channels per MASTERCARD 0-35 on J3 and 36-71 on J4 */
		static constexpr int NUM_OUTPUTS = 45; /* number of binary output channels per MASTERCARD 11-48 on J2 (work fine), 49-55 on P2 (?) */
		static constexpr int NUM_DISPLAYS = 64; /* number of 7-segment displays per MASTERCARD 0-15 on J1 */
		static constexpr int MASTERCARDS = 4; /* maximum number of master cards per USB expansion card */
		static constexpr int TAXES = 4; /* number of axes available on the USB expansion or IOCard-USBServos card */

		static constexpr int NCARDS = 1; /* number of cards on USB connection (only for Mastercard) */

		IOCards() = delete;
		explicit IOCards(std::string deviceBusAddr, std::string name);

		void openDevice(std::string device_bus_addr);

		~IOCards();

		static IOCards::IOCard_Device identify_iocards_usb(const std::string& bus_address);
		static std::string find_iocard_devices();


		//void closeDown();

		//void mainThread();
		//void startThread(void);
		bool initializeIOCards(unsigned char number_of_axes = 0);
		int initialize_iocardsdata(void);
		//int copyIOCardsData(void);

		int receive_mastercard_synchronous();
		//int receive_mastercard(void);
		//int send_mastercard(void);

		int mastercard_input(int input, int* value, int card = 0);
		//void process_master_card_inputs(const OnOffKeyCommand keycmd[], int numberOfCmds, int card = 0);

		//void process_master_card_inputs(masterCard_input_state* switch_states[], int numberOfCmds, int card = 0);

		//int mastercard_output(int output, int* value, int card = 0);
		//int mastercard_encoder(int input, double* value, double multiplier, double accelerator, int type = 2, int card = 0);
		//int mastercard_display(int pos, int n0, int* value, int hasnegative, int card = 0);
		//int mastercard_send_display(unsigned char value, int pos, int card = 0);

		//struct libusb_context* Ctx() const
		//{
		//	return ctx;
		//}

		//void Ctx(struct libusb_context* val)
		//{
		//	ctx = val;
		//}


	public:

		//std::unique_ptr<UsbWorker> worker;

		//bool IsInitialized() const
		//{
		//	return isInitialized;
		//}

		bool is_open{false};
		bool is_Claimed{false};

		void set_iocard_device(IOCard_Device device) {io_card_device = device;}
		[[nodiscard]] IOCard_Device get_io_card_device() const {return io_card_device;}

		void set_name(std::string name) {name = name;}
		[[nodiscard]] std::string get_name() const {return device_name;}

		//std::mutex iocards_mutex;
	private:


	//	int get_acceleration(int card, int input, double accelerator);

	//	std::thread iocards_thread;

		std::string device_name;
		IOCard_Device io_card_device{UNKNOWN};
		unsigned short bus{0};
		unsigned short addr{0};

		libusb_device_handle* handle{nullptr};
		unsigned char epIn; // input endpoint 
		unsigned char epOut; // output endpoint
		uint16_t inBufferSize; // input endpoint buffer size
		uint16_t outBufferSize; // output endpoint buffer size

		bool isInitialized{false};
	//	bool sentWakeupMsg;

	//	variables holding current and previous Mastercard states
		int inputs[NUM_INPUTS][MASTERCARDS];
		int inputs_old[NUM_INPUTS][MASTERCARDS];

		int axes[TAXES];
		int axes_old[TAXES];
		int axis; /* number of active A/D converter */

	//    QElapsedTimer update;
		std::chrono::high_resolution_clock::time_point time_enc[NUM_INPUTS][MASTERCARDS];

		int slotdata[8][MASTERCARDS]; /* stores slots present in mastercard USB read */

		int outputs[NUM_OUTPUTS][MASTERCARDS];
		int outputs_old[NUM_OUTPUTS][MASTERCARDS];

	protected:
		int displays[NUM_DISPLAYS][MASTERCARDS];
		int displays_old[NUM_DISPLAYS][MASTERCARDS];
	};
}
#endif // IOCARDS_H
