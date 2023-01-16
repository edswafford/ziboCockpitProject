#pragma once


#include "../ftd2xx/ftd2xxdevices_wrapper.hpp"
#include <atomic>
namespace zcockpit::cockpit::hardware
{
	class FiDevice
	{
	public:
	    static const int FI_COMMAND_SIZE;
		static const int DISPLAYS_ON;
		static const int DISPLAYS_OFF;
		static const int DISPLAY_ON_BKLIGHT_OFF;
		static const int DISPLAY_OFF_BKLIGHT_ON;

		enum DEVICE_CATEGORY
		{
			GAUGE,
			RADIO,
			CLOCK,
		};
	    enum DEVICE_Type
	    {
	        APU_EGT,
	        FUEL_TEMP,
	        CABIN_TEMP,
	        CABIN_ALT,
	        CABIN_CLIMB,
	        PRESS_DIFF,
	        LEFT_DUCT,
	        RIGHT_DUCT,
	        CREW_OXYGEN,
			YAW_DAMPER,
			FLAP,
			BRAKE_TEMP,
			CAPT_COMM,
			CAPT_NAV,
			FO_COMM,
			FO_NAV,
			ADF,
	    };

	    enum FI_DEVICE_CMD
	    {
	        NOOP = 0,
	        INITIALIZE,
	        SET_ADDRESS,
	        SET_SPEED,
	        SET_DIRECTION,
	        NEW_NEEDLE_VALUE,
	        NEW_SECOND_VALUE,
	        STATUS,
	        SET_LIGHTS,
	        SET_MODEL_VERSION,
	    };
		enum FI_DISPLAYS
		{
			ACTIVE,
			INACTIVE,
		};
	 
	    void Value(int val)
	    {
	        DEVICEValue = val;
	    }


	    FiDevice::DEVICE_Type Type() const
	    {
	        return type;
	    }

	    FiDevice::FI_DEVICE_CMD Cmd() const
	    {
	        return gaugeCmd;
	    }
		FiDevice::DEVICE_CATEGORY CATEGORY() const
	    {
			return category;
	    }
		int DeviceID() const
	    {
			return id;
	    }

		uint32_t get_display_value(FI_DISPLAYS display)
	    {
			if (display == ACTIVE)
			{
				return  (rxBuffer[6] << 8) | rxBuffer[7];
			}
			else 
			{
				return  (rxBuffer[9] << 8) | rxBuffer[10];
			}
	    }
		uint8_t * get_buffer() { return rxBuffer; }
		
		FiDevice() = delete;
		FiDevice(int deviceID, DEVICE_CATEGORY category, DEVICE_Type type, double scaleFactor, int minValue, int maxValue, int offset, double K,
		         FI_DEVICE_CMD needleCmd, FI_DEVICE_CMD radioCmd = NOOP, FI_DEVICE_CMD clockCmd = NOOP, int max_send_count = 3);


		~FiDevice();

	    bool sendCommand(FT_HANDLE ftDeviceHandle, int cmd, int val);
	    bool sendValue(FT_HANDLE ftDeviceHandle, int cmd, double inValue);

	    bool checkStatus(FT_HANDLE ftDeviceHandle);
		static int read(FT_HANDLE ftDeviceHandle, uint8_t* buffer, DWORD size_of_buffer);
		
		std::atomic<DWORD> bytes_read_by_timer_thread{0};
		std::atomic<int> timeout_counter{-1};
		std::atomic<bool>valid{ false };
		std::atomic<bool>fresh_data{ false };
	private:

		int id;
		DEVICE_CATEGORY category;
	    DEVICE_Type type;
	    FI_DEVICE_CMD gaugeCmd;
		FI_DEVICE_CMD radioCmd;
		FI_DEVICE_CMD clockCmd;
	    double filterCoef; // K used in first order filter
	    double scaleFactor;
	    int minDeviceValue, maxDeviceValue;
	    int offset;
	    double filteredValue;
	    int DEVICEValue;
	    int sendCount;
		int max_send_cnt{ 3 };
		static const int MAX_READ_BUFFER = 32;
		uint8_t rxBuffer[MAX_READ_BUFFER]{};


	};
}