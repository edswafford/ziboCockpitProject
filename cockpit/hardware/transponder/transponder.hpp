#pragma once

#include <array>
#include <windows.h>
#include <mutex>
#include <string>
#include <random>

#include "../ftd2xx/ftd2xxdevices_wrapper.hpp"



//using namespace std;
namespace zcockpit::cockpit::hardware
{
	class Transponder
	{
	public:
		static const char* xponderSerialNumber;
		Transponder();
		~Transponder();
		Transponder(const Transponder&) = delete;
		Transponder& operator=(const Transponder&) = delete;

		static const bool VALID = true;
		FT_DEVICE_LIST_INFO_NODE devInfo;
		int xpndr_mode;
		int xpndr_atc;
		int xpndr_alt;

		bool Available() const
		{
			return xpndrAvailable;
		}

		void Available(bool val)
		{
			xpndrAvailable = val;
		}

		FT_HANDLE FtHandle() const
		{
			return ftDeviceHandle;
		}

		void sync_digits();
		void sync_switches();
		void readXpndr();
		void updatePowerOn();
		void requestData();
		void updateFailed();
		void updateRply();

		void initialize(const char* deviceSerialNumber, FT_DEVICE_LIST_INFO_NODE* devInfo);
		void check_xpndr_digits();
		void open(const char* deviceSerialNumber);
		void closeDown() const;


		//static Transponder* instance()
		//{
		//	static mutex mutex;
		//	if(!xpndrInstance)
		//	{
		//		std::lock_guard<std::mutex> lock(mutex);
		//		if(!xpndrInstance)
		//		{
		//			xpndrInstance = new Transponder;
		//		}
		//	}

		//	return xpndrInstance;
		//}

		static void drop()
		{
			static std::mutex mutex;
			std::lock_guard<std::mutex> lock(mutex);
//			delete xpndrInstance;
//			xpndrInstance = 0;
		}


	private:
		void openEx();
		void reopen(const char* deviceSerialNumber);
		void setTimeouts(int readTimeout, int writeTimeout);

//		static Transponder* xpndrInstance;

		Ftd2xxDevices* ftd2Devices;
		FT_HANDLE ftDeviceHandle;

		const char* serialNumber;
		bool xpndrAvailable;
		bool valid;

		int randomInt;

		int rplyCnt;
		bool stbySelected;
		bool releaseIdent;
		std::array<unsigned char, 4> xpnd_previous_digits;
		std::array<unsigned char, 4> ifly_xpnd_previous_digits;
		std::array<unsigned char, 4> ifly_xpnd_expected_digits;
		std::array<unsigned char, 4> ifly_xpnd_expected_cnt;

		int ifly_xpndr_stable_cnt;

		std::array<bool, 4> xpndr_needs_sync;

	public:
		static const int XPNDR_ATC_CODE = 1;

		static const int XPNDR_SET_FAIL = 2;

		static const int XPNDR_CLEAR_FAIL = 3;

		static const int XPNDR_BUTTONS = 4;

		static const int XPNDR_READY = 5;

		static const int XPNDR_POWER_ON = 6;

		static const int XPNDR_POWER_OFF = 7;

		static const int XPNDR_REPLY_ON = 8;

		static const int XPNDR_REPLY_OFF = 9;

		static const int ACK_XPNDR_ATC_CODE = 101;

		static const int ACK_XPNDR_SET_FAIL = 102;

		static const int ACK_XPNDR_CLEAR_FAIL = 103;

		static const int ACK_XPNDR_BUTTONS = 104;

		static const int ACK_XPNDR_READY = 105;

		static const int ACK_XPNDR_POWER_ON = 106;

		static const int ACK_XPNDR_POWER_OFF = 107;

		static const int ACK_XPNDR_REPLY_ON = 108;

		static const int ACK_XPNDR_REPLY_OFF = 109;
	};
}
