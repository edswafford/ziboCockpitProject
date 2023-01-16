#include "fidevice.hpp"
#include "../common/logger.hpp"
extern logger LOG;

namespace zcockpit::cockpit::hardware
{
	constexpr int FiDevice::FI_COMMAND_SIZE = 8;
	constexpr int FiDevice::DISPLAYS_ON = 0xC0;
	constexpr int FiDevice::DISPLAYS_OFF = 0x00;
	constexpr int FiDevice::DISPLAY_ON_BKLIGHT_OFF = 0x80;
	constexpr int FiDevice::DISPLAY_OFF_BKLIGHT_ON = 0x40;

	bool FiDevice::sendValue(FT_HANDLE ftDeviceHandle, int cmd, double inValue)
	{
		if (abs(inValue - filteredValue) > 1.0)
		{
			sendCount = 0;
		}
		if (sendCount < max_send_cnt)
		{
			// First order Filter
			// Fn = S + K (F(n-1) - S)
			//  K = 1- t/T
			//  t = sample interval
			//  T = Time Constant
			//  Fn = new filtered value
			//  F(n-1) = previous filtered value
			//  S = Sample

			filteredValue = (inValue + (filterCoef * (filteredValue - inValue)));
			DEVICEValue = static_cast<int>(filteredValue * scaleFactor) + offset;
			if (DEVICEValue < minDeviceValue)
			{
				DEVICEValue = minDeviceValue;
			}
			else
			{
				if (DEVICEValue > maxDeviceValue)
				{
					DEVICEValue = maxDeviceValue;
				}
			}
			sendCount++;
			return sendCommand(ftDeviceHandle, cmd, DEVICEValue);
		}
		return true;
	}

	FiDevice::FiDevice(int deviceID, DEVICE_CATEGORY category, DEVICE_Type type, double scaleFactor, int minValue,
	                   int maxValue, int offset, double K,
		FI_DEVICE_CMD needleCmd, FI_DEVICE_CMD radioCmd, FI_DEVICE_CMD clockCmd, int max_send_cnt)
	{
		this->id = deviceID;
		this->category = category;
		this->type = type;
		this->scaleFactor = scaleFactor;
		this->minDeviceValue = minValue;
		this->maxDeviceValue = maxValue;
		this->offset = offset;
		this->filterCoef = K;
		this->gaugeCmd = needleCmd;
		this->radioCmd = radioCmd;
		this->clockCmd = clockCmd;
		filteredValue = minValue;
		DEVICEValue = minValue;
		sendCount = 0;
		this->max_send_cnt = max_send_cnt;
	}

	FiDevice::~FiDevice()
	{
		LOG() << "Good by FiDevice";
	}

	bool FiDevice::sendCommand(FT_HANDLE ftDeviceHandle, int cmd, int inValue)
	{
		unsigned char xferCmd[FI_COMMAND_SIZE];
		xferCmd[0] = 0X0;
		xferCmd[5] = 0XFF;
		xferCmd[6] = 0XFF;
		xferCmd[7] = 0XFF;

		xferCmd[1] = id;

		int long1 = inValue;
		if (inValue < 0)
		{
			long1 = abs(inValue);
		}

		const int int2 = (long1 / 256);

		xferCmd[3] = ((long1 & 0XFF) | 0X01);
		xferCmd[4] = ((int2 & 0XFF) | 0X02);

		int int3 = cmd * 16;
		int3 = int3 | (long1 & 0X01);
		int3 = int3 | (int2 & 0X02);
		if (inValue < 0)
		{
			int3 = int3 | 0X0C;
		}
		else
		{
			int3 = int3 | 0X08;
		}
		xferCmd[2] = (int3 & 0XFF);

		DWORD bytesWritten = 0;
		FT_Write(ftDeviceHandle, xferCmd, FI_COMMAND_SIZE, &bytesWritten);
		return (bytesWritten == FI_COMMAND_SIZE);
	}

	bool FiDevice::checkStatus(FT_HANDLE ftDeviceHandle)
	{
		// send get status command if timer is not running
		if (ftDeviceHandle != nullptr && timeout_counter < 0)
		{
			// send Flight Illusion status command
			sendCommand(ftDeviceHandle, STATUS, 0);
		}

		return valid;
	}



	//
	// STATIC FUNCTION
	//
	int FiDevice::read(FT_HANDLE ftDeviceHandle, uint8_t* buffer, DWORD size_of_buffer)
	{
		DWORD bytes_in_queue = 0;
		DWORD bytes_read = 0;

		FT_STATUS ft_status = FT_GetQueueStatus(ftDeviceHandle, &bytes_in_queue);
		if (ft_status == FT_OK)
		{
			// expect 25 bytes
			if (bytes_in_queue >= 25)
			{

				memset(buffer, 0XFF, size_of_buffer);

				if (bytes_in_queue > size_of_buffer)
				{
					//
					// only read what the buffer can hold
					//
					ft_status = FT_Read(ftDeviceHandle, buffer, size_of_buffer, &bytes_read);
					if (ft_status == FT_OK)
					{
						if (bytes_read == size_of_buffer)
						{
							// sigal there are more messages to read
							return bytes_in_queue;
						}
						else
						{
							return bytes_read;
						}
					}
					else
					{
						LOG() << "Flight Illussions read failure " << ft_status;
						return 0;
					}
				}
				else
				{
					//
					// there is room to read everything  in the queue
					//
					ft_status = FT_Read(ftDeviceHandle, buffer, bytes_in_queue, &bytes_read);
					if (ft_status == FT_OK)
					{
						if (buffer[0] == 0 && (buffer[1] >= 120 && buffer[1] <= 124))
						{
							return bytes_read;
						}
					}
					else
					{
						LOG() << "Flight Illussions read failure " << ft_status;
						return 0;
					}
				}
			}
		}
		return bytes_read;
	}
}
