#include "transponder.hpp"


#include "../common/logger.hpp"

extern logger LOG;

namespace zcockpit::cockpit::hardware
{

	Transponder* Transponder::xpndrInstance = nullptr;
	const char* Transponder::xponderSerialNumber = "DPAP30AG";

	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_int_distribution<> dis(1, 12);

	static const int COMMAND_SIZE = 4;
	using namespace std;

	Transponder::Transponder(): valid(false), randomInt(0), rplyCnt(3), stbySelected(false), releaseIdent(false),
	                            xpndr_mode(-1), xpndr_alt(-1), xpndr_atc(-1), xpnd_previous_digits{{ 0xFF,0xFF,0xFF,0xFF }}, ifly_xpnd_previous_digits{{0,0,0,0}},
	                            xpndr_needs_sync{{false,false,false,false}}, ifly_xpndr_stable_cnt(0),
	                            ifly_xpnd_expected_digits{{0xFF,0xFF,0xFF,0xFF }}, ifly_xpnd_expected_cnt{{0,0,0,0}}
	{
		Available(false);

		ftd2Devices = Ftd2xxDevices::instance();

	}

	void Transponder::check_xpndr_digits()
	{
		ifly_xpndr_stable_cnt++;

		//if (FsxSimConnect::transponderChanged)
		//{
		//	FsxSimConnect::transponderChanged = false;
		//if(ifly_xpnd_previous_digits[0] != Ifly737::shareMemSDK->Transponder_1000_Status)
		//	{
		//	ifly_xpnd_previous_digits[0] = Ifly737::shareMemSDK->Transponder_1000_Status;
		//		ifly_xpndr_stable_cnt = 0;
		//	}
		//if(ifly_xpnd_previous_digits[1] != Ifly737::shareMemSDK->Transponder_100_Status)
		//	{
		//	ifly_xpnd_previous_digits[1] = Ifly737::shareMemSDK->Transponder_100_Status;
		//		ifly_xpndr_stable_cnt = 0;
		//	}
		//if(ifly_xpnd_previous_digits[2] != Ifly737::shareMemSDK->Transponder_10_Status)
		//	{
		//	ifly_xpnd_previous_digits[2] = Ifly737::shareMemSDK->Transponder_10_Status;
		//		ifly_xpndr_stable_cnt = 0;
		//	}
		//if(ifly_xpnd_previous_digits[3] != Ifly737::shareMemSDK->Transponder_1_Status)
		//	{
		//	ifly_xpnd_previous_digits[3] = Ifly737::shareMemSDK->Transponder_1_Status;
		//		ifly_xpndr_stable_cnt = 0;
		//	}
		//}
		//if (ifly_xpndr_stable_cnt > 4)
		//{
		//	ifly_xpndr_stable_cnt = 0;
		//	auto needs_sync = false;
		//	for (int i = 0; i < 4; i++)
		//	{
		//		xpndr_needs_sync[i] = ifly_xpnd_previous_digits[i] != xpnd_previous_digits[i] ? true : false;
		//		if (xpndr_needs_sync[i])
		//		{
		//			needs_sync = true;
		//		}
		//	}
		//	if (needs_sync)
		//	{
		//		sync_digits();
		//	}
		//}
		//else
		//{
		//	xpndr_needs_sync[0] = xpndr_needs_sync[1] = xpndr_needs_sync[2] = xpndr_needs_sync[3] = false;
		//}
	}

	void Transponder::initialize(const char* deviceSerialNumber, FT_DEVICE_LIST_INFO_NODE* devInfo)
	{
		if(devInfo != nullptr)
		{
			this->devInfo.ID = devInfo->ID;
			this->devInfo.Flags = devInfo->Flags;
			this->devInfo.Type = devInfo->Type;
			this->devInfo.LocId = devInfo->LocId;
			this->devInfo.ftHandle = devInfo->ftHandle;
			serialNumber = deviceSerialNumber;
			valid = true;
			Available(false);
			ftDeviceHandle = nullptr;
			stbySelected = false;
		}
	}


	/*
	  devInfo.Flags:
	    The flag value is a 4-byte bit map containing miscellaneous data as defined Appendix A – Type Definitions. 
	    Bit 0 (least significant bit) of this number indicates if the port is open (1) or closed (0). 
	    Bit 1 indicates if the device is enumerated as a high-speed USB device (2) or a full-speed USB device (0). 
	    The remaining bits (2 - 31) are reserved
	*/
	void Transponder::open(const char* deviceSerialNumber)
	{
		FT_STATUS ftStatus;
		if(valid)
		{
			if(ftDeviceHandle == nullptr)
			{
				openEx();
			}
			else
			{
				// open failed or something went wrong -- handle is not null so try to close the device
				ftStatus = FT_Close(ftDeviceHandle);
				if(ftStatus != FT_OK)
				{
					LOG() << "Closed Failed for Device: (" << serialNumber << ")  -- Error: " << ftStatus;
				}
				reopen(deviceSerialNumber);
			}
		}
		else
		{
			reopen(deviceSerialNumber);
		}
		if(Available())
		{
			setTimeouts(5000, 1000);
			if(Available())
			{
				ftStatus = FT_SetBaudRate(ftDeviceHandle, 9600);
				if(ftStatus != FT_OK)
				{
					LOG() << "ERROR: Setting BaudRate Failed for Device:";
					Available(false);
					return;
				}
				// Set 8 data bits, 1 stop bit and no parity 
				ftStatus = FT_SetDataCharacteristics(ftDeviceHandle, FT_BITS_8, FT_STOP_BITS_1, FT_PARITY_NONE);
				if(ftStatus != FT_OK)
				{
					LOG() << "ERROR: Setting parity Failed for Device:";
					Available(false);
					return;
				}
				ftStatus = FT_SetFlowControl(ftDeviceHandle, FT_FLOW_RTS_CTS, 0, 0);
				if(ftStatus != FT_OK)
				{
					LOG() << "ERROR: Setting Flow Control Failed for Device";
					Available(false);
				}
			}
		}
	}

	void Transponder::closeDown()
	{
		if(Available())
		{
			FT_Close(ftDeviceHandle);
			drop();
		}
	}

	void Transponder::setTimeouts(int readTimeout, int writeTimeout)
	{
		FT_STATUS ftStatus;
		// Set read/write timeout i.e. 5000 = 5sec, 1000 = 1sec 
		ftStatus = FT_SetTimeouts(ftDeviceHandle, readTimeout, writeTimeout);
		if(ftStatus != FT_OK)
		{
			LOG() << "Setting Timeout Failed for Device: (" << serialNumber << ")  -- Error: " << ftStatus;
			Available(false);
		}
	}

	void Transponder::reopen(const char* deviceSerialNumber)
	{
		// look for device
		FT_DEVICE_LIST_INFO_NODE* devInfo = ftd2Devices->getDevice(deviceSerialNumber);
		if(devInfo != nullptr)
		{
			initialize(deviceSerialNumber, devInfo);
			openEx();
		}
	}

	void Transponder::openEx()
	{
		FT_STATUS ftStatus;
		if((devInfo.Flags & 0X1) == 0)
		{
			ftStatus = FT_OpenEx((PVOID)serialNumber, FT_OPEN_BY_SERIAL_NUMBER, &ftDeviceHandle);
			if(ftStatus == FT_OK)
			{
				ftStatus = FT_ResetDevice(ftDeviceHandle);
				if(ftStatus == FT_OK)
				{
					Available(true);
				}
				else
				{
					Available(false);
					// LOG() << "Reset Failed for Device: (" << serialNumber << ")  -- Error: " << ftStatus;
				}
			}
			else
			{
				void* pSerialNumber;
				pSerialNumber = (void*)string(serialNumber).c_str();
				ftStatus = FT_OpenEx(pSerialNumber, FT_OPEN_BY_SERIAL_NUMBER, &ftDeviceHandle);
				ftStatus = FT_ResetDevice(ftDeviceHandle);
				if(ftStatus == FT_OK)
				{
					Available(true);
				}
				else
				{
					Available(false);
					// LOG() << "Reset Failed for Device: (" << serialNumber << ")  -- Error: " << ftStatus;
				}
			}
		}
		else
		{
			Available(false);
			if(ftDeviceHandle != nullptr)
			{
				FT_Close(ftDeviceHandle);
				// LOG() << "Tried to open Device: (" << serialNumber << ") that is already open -- Flags:" << devInfo.Flags; 
			}
		}
	}

	void Transponder::updatePowerOn()
	{
		FT_STATUS ftStatus;
		DWORD BytesWritten;
		char TxBuffer[4]; // Contains data to write to device

		if(Available())
		{
			int code = Transponder::XPNDR_POWER_ON;
			//if(client->electrical_power_state == 0)
			//{
			//	code = Transponder::XPNDR_POWER_OFF;
			//}

			TxBuffer[0] = code;
			TxBuffer[1] = 0;
			TxBuffer[2] = 0;
			TxBuffer[3] = 0;
			ftStatus = FT_Write(ftDeviceHandle, TxBuffer, COMMAND_SIZE, &BytesWritten);
			if(ftStatus != FT_OK)
			{
				Available(false);
			}
		}
	}

	void Transponder::updateRply()
	{
		FT_STATUS ftStatus;
		DWORD BytesWritten;
		char TxBuffer[4]; // Contains data to write to device


		if(Available())
		{
			if(randomInt <= 0)
			{
				BYTE code = Transponder::XPNDR_REPLY_OFF;
				if(stbySelected)
				{
					code = Transponder::XPNDR_REPLY_OFF;
				}
				else
				{
					code = Transponder::XPNDR_REPLY_ON;
				}
				rplyCnt--;
				if(rplyCnt <= 0)
				{
					code = Transponder::XPNDR_REPLY_OFF;

					randomInt = dis(gen);
					rplyCnt = 3;
				}


				TxBuffer[0] = code;
				TxBuffer[1] = 0;
				TxBuffer[2] = 0;
				TxBuffer[3] = 0;
				ftStatus = FT_Write(ftDeviceHandle, TxBuffer, COMMAND_SIZE, &BytesWritten);
				if(ftStatus != FT_OK)
				{
					Available(false);
				}
			}
			else
			{
				randomInt--;
			}
		}
	}

	void Transponder::updateFailed()
	{
		FT_STATUS ftStatus;
		DWORD BytesWritten;
		char TxBuffer[4]; // Contains data to write to device

		if(Available())
		{
			//int code = Transponder::XPNDR_CLEAR_FAIL;
			//if(Ifly737::shareMemSDK->Transponder_Fail_Light_Status == 1)
			//{
			//	code = Transponder::XPNDR_SET_FAIL;
			//}
			//TxBuffer[0] = code;
			//TxBuffer[1] = 0;
			//TxBuffer[2] = 0;
			//TxBuffer[3] = 0;
			//ftStatus = FT_Write(ftDeviceHandle, TxBuffer, COMMAND_SIZE, &BytesWritten);
			//if(ftStatus != FT_OK)
			//{
			//	Available(false);
			//}
		}
	}

	void Transponder::requestData()
	{
		FT_STATUS ftStatus;
		DWORD BytesWritten;
		char TxBuffer[4]; // Contains data to write to device

		if(Available())
		{
			// get ATC code from transponder
			TxBuffer[0] = Transponder::XPNDR_ATC_CODE;
			TxBuffer[1] = 0;
			TxBuffer[2] = 0;
			TxBuffer[3] = 0;
			ftStatus = FT_Write(ftDeviceHandle, TxBuffer, COMMAND_SIZE, &BytesWritten);
			if(ftStatus != FT_OK)
			{
				Available(false);
				return;
			}

			// get switch positions
			TxBuffer[0] = Transponder::XPNDR_BUTTONS;
			ftStatus = FT_Write(ftDeviceHandle, TxBuffer, COMMAND_SIZE, &BytesWritten);
			if(ftStatus != FT_OK)
			{
				Available(false);
			}
		}
	}


//	static const KEY_COMMAND XPND_INCREMENT[] = {KEY_COMMAND_FMS_XPNDR_1000_INC, KEY_COMMAND_FMS_XPNDR_100_INC, KEY_COMMAND_FMS_XPNDR_10_INC, KEY_COMMAND_FMS_XPNDR_1_INC};
//	static const KEY_COMMAND XPND_DECREMENT[] = {KEY_COMMAND_FMS_XPNDR_1000_DEC, KEY_COMMAND_FMS_XPNDR_100_DEC, KEY_COMMAND_FMS_XPNDR_10_DEC, KEY_COMMAND_FMS_XPNDR_1_DEC};

	void Transponder::sync_digits()
	{
		for(int i = 0; i < 4; i++)
		{
			//if(xpndr_needs_sync[i] && xpnd_previous_digits[i] >= 0 && xpnd_previous_digits[i] <= 9)
			//{
			//	if(ifly_xpnd_expected_digits[i] == -1)
			//	{
			//		ifly_xpnd_expected_digits[i] = xpnd_previous_digits[i];
			//	}
			//	else if(ifly_xpnd_previous_digits[i] != ifly_xpnd_expected_digits[i])
			//	{
			//		ifly_xpnd_expected_cnt[i]++;
			//		if(ifly_xpnd_expected_cnt[i] > 10)
			//		{
			//			// we've timed out -- so re need to resend
			//			ifly_xpnd_expected_cnt[i] = 0;
			//		}
			//		else
			//		{
			//			// previous increments/decrements have not processed -- continue waiting
			//			continue;
			//		}
			//	}
			//	xpndr_needs_sync[i] = false;
			//	if(ifly_xpnd_previous_digits[i] < xpnd_previous_digits[i])
			//	{
			//		// increment
			//		ifly_xpnd_expected_digits[i] = xpnd_previous_digits[i];
			//		int count = (xpnd_previous_digits[i] - ifly_xpnd_previous_digits[i]);
			//		for(int k = 0; k < count; k++)
			//		{
			//			sendMessageInt(XPND_INCREMENT[i], 0);
			//		}
			//	}
			//	else if(ifly_xpnd_previous_digits[i] > xpnd_previous_digits[i])
			//	{
			//		// decrement
			//		ifly_xpnd_expected_digits[i] = xpnd_previous_digits[i];
			//		int count = (ifly_xpnd_previous_digits[i] - xpnd_previous_digits[i]);
			//		for(int k = 0; k < count; k++)
			//		{
			//			sendMessageInt(XPND_DECREMENT[i], 0);
			//		}
			//	}
			//}
		}
	}

	//std::array<int, 2> xpndr_alt_keycode_lookup{KEY_COMMAND_FMS_XPNDR_ALT_1 ,KEY_COMMAND_FMS_XPNDR_ALT_2};
	//std::array<int, 2> xpndr_atc_keycode_lookup{KEY_COMMAND_FMS_XPNDR_ATC_1 ,KEY_COMMAND_FMS_XPNDR_ATC_2};
	//std::array<int, 6> xpndr_mode_keycode_lookup{KEY_COMMAND_FMS_XPNDR_MODE_POS1 ,KEY_COMMAND_FMS_XPNDR_MODE_POS2, KEY_COMMAND_FMS_XPNDR_MODE_POS3,
	//	KEY_COMMAND_FMS_XPNDR_MODE_POS4, KEY_COMMAND_FMS_XPNDR_MODE_POS5,KEY_COMMAND_FMS_XPNDR_MODE_POS6};

	void Transponder::sync_switches()
	{
		if(Available())
		{
		//	auto ifly_xpndr_alt = Ifly737::shareMemSDK->Transponder_ALT_Switches_Status;
		//	auto ifly_xpndr_atc = Ifly737::shareMemSDK->Transponder_ATC_Switches_Status;
		//	auto ifly_xpndr_mode = Ifly737::shareMemSDK->Transponder_Mode_Switches_Status;

		//	if(xpndr_alt == -1)
		//	{
		//		xpndr_alt = xpndr_alt_keycode_lookup[ifly_xpndr_alt];
		//	}
		//	else if(xpndr_alt != xpndr_alt_keycode_lookup[ifly_xpndr_alt])
		//	{
		//		sendMessageInt(xpndr_alt, 0);
		//	}

		//	if(xpndr_atc == -1)
		//	{
		//		xpndr_atc = xpndr_atc_keycode_lookup[ifly_xpndr_atc];
		//	}
		//	else if(xpndr_atc != xpndr_atc_keycode_lookup[ifly_xpndr_atc])
		//	{
		//		sendMessageInt(xpndr_atc, 0);
		//	}

		//	if(xpndr_mode == -1)
		//	{
		//		xpndr_mode = xpndr_mode_keycode_lookup[ifly_xpndr_mode];
		//	}
		//	else if(xpndr_mode != xpndr_mode_keycode_lookup[ifly_xpndr_mode])
		//	{
		//		sendMessageInt(xpndr_mode, 0);
		//	}
		}
	}

	void Transponder::readXpndr()
	{
		FT_STATUS ftStatus;
		DWORD EventDWord;
		DWORD RxBytes;
		DWORD TxBytes;
		DWORD BytesReceived;
		char RxBuffer[256];
		char TxBuffer[4];
		DWORD BytesWritten;
		bool stbySelected;

		ftStatus = FT_GetStatus(ftDeviceHandle, &RxBytes, &TxBytes, &EventDWord);
		if(ftStatus == FT_OK)
		{
			if(RxBytes > 0)
			{
				ftStatus = FT_Read(ftDeviceHandle, RxBuffer, RxBytes, &BytesReceived);
				if(ftStatus == FT_OK)
				{
					BYTE messageType = 0;
					unsigned i = 0;
					while(i < RxBytes)
					{
						messageType = (BYTE) RxBuffer[i];
						if(messageType > 0 && messageType < 100)
						{
							BYTE ack = (BYTE) (messageType + 100);
							TxBuffer[0] = ack;
							TxBuffer[1] = 0;
							TxBuffer[2] = 0;
							TxBuffer[3] = 0;
							if(ftDeviceHandle != nullptr)
							{
								FT_Write(ftDeviceHandle, TxBuffer, COMMAND_SIZE, &BytesWritten);
							}
						}
						if(messageType == Transponder::XPNDR_ATC_CODE)
						{
							i++;
							unsigned char xpndDigits[4];
							xpndDigits[0] = RxBuffer[i] - 0x30;
							xpndDigits[1] = RxBuffer[i + 1] - 0x30;
							xpndDigits[2] = RxBuffer[i + 2] - 0x30;
							xpndDigits[3] = RxBuffer[i + 3] - 0x30;
							for(int j = 0; j < 4; j++)
							{
								char digit = RxBuffer[i + j] - 0x30;
								if(digit < 0 || digit > 9)
								{
									break;
								}
								if(xpnd_previous_digits[j] < 0 || xpnd_previous_digits[j] > 9)
								{
									xpnd_previous_digits[j] = digit;
								}
								else
								{
									if(digit == xpnd_previous_digits[j])
									{
										continue;
									}
									else if(digit > xpnd_previous_digits[j])
									{
										// increment
										int count = digit - xpnd_previous_digits[j];
										ifly_xpnd_expected_digits[j] = xpnd_previous_digits[j];
										xpnd_previous_digits[j] = digit;
										for(int k = 0; k < count; k++)
										{
//											sendMessageInt(XPND_INCREMENT[j], 0);
										}
									}
									else
									{
										// decrement
										int count = xpnd_previous_digits[j] - digit;
										ifly_xpnd_expected_digits[j] = xpnd_previous_digits[j];
										xpnd_previous_digits[j] = digit;
										for(int k = 0; k < count; k++)
										{
//											sendMessageInt(XPND_DECREMENT[j], 0);
										}
									}
								}
							}

							i += 4;
						}

						else if(messageType == Transponder::XPNDR_BUTTONS)
						{
							i++;
							int buttons = RxBuffer[i];
							i++;
							bool altOn = true;
							// update ALT RPTG
							int altButtons = buttons & 0X18;
							switch(altButtons)
							{
								case 0:
									//if(xpndr_alt != KEY_COMMAND_FMS_XPNDR_ALT_1)
									//{
									//	xpndr_alt = KEY_COMMAND_FMS_XPNDR_ALT_1;
									//	sendMessageInt(KEY_COMMAND_FMS_XPNDR_ALT_1, 0);
									//}
									break;
								case 0X10:
									altOn = false;
									break;
								case 8:
//									if(xpndr_alt != KEY_COMMAND_FMS_XPNDR_ALT_2)
									//{
									//	xpndr_alt = KEY_COMMAND_FMS_XPNDR_ALT_2;
									//	sendMessageInt(KEY_COMMAND_FMS_XPNDR_ALT_2, 0);
									//}
									break;
								default:
									break;
							}

							int transponderButtons = buttons & 0X87;
							stbySelected = false;
							//switch(transponderButtons)
							//{
							//	case 0:
							//		// ATC 2
							//		if(xpndr_atc != KEY_COMMAND_FMS_XPNDR_ATC_2)
							//		{
							//			xpndr_atc = KEY_COMMAND_FMS_XPNDR_ATC_2;
							//			sendMessageInt(KEY_COMMAND_FMS_XPNDR_ATC_2, 0);
							//		}
							//		if(altOn)
							//		{
							//			if(xpndr_mode != KEY_COMMAND_FMS_XPNDR_MODE_POS4)
							//			{
							//				xpndr_mode = KEY_COMMAND_FMS_XPNDR_MODE_POS4;
							//				sendMessageInt(KEY_COMMAND_FMS_XPNDR_MODE_POS4, 0);
							//			}
							//		}
							//		else
							//		{
							//			if(xpndr_mode != KEY_COMMAND_FMS_XPNDR_MODE_POS3)
							//			{
							//				xpndr_mode = KEY_COMMAND_FMS_XPNDR_MODE_POS3;
							//				sendMessageInt(KEY_COMMAND_FMS_XPNDR_MODE_POS3, 0);
							//			}
							//		}
							//		break;

							//	case 2:
							//		// ATC 1
							//		if(xpndr_atc != KEY_COMMAND_FMS_XPNDR_ATC_1)
							//		{
							//			xpndr_atc = KEY_COMMAND_FMS_XPNDR_ATC_1;
							//			sendMessageInt(KEY_COMMAND_FMS_XPNDR_ATC_1, 0);
							//		}
							//		if(altOn)
							//		{
							//			if(xpndr_mode != KEY_COMMAND_FMS_XPNDR_MODE_POS4)
							//			{
							//				xpndr_mode = KEY_COMMAND_FMS_XPNDR_MODE_POS4;
							//				sendMessageInt(KEY_COMMAND_FMS_XPNDR_MODE_POS4, 0);
							//			}
							//		}
							//		else
							//		{
							//			if(xpndr_mode != KEY_COMMAND_FMS_XPNDR_MODE_POS3)
							//			{
							//				xpndr_mode = KEY_COMMAND_FMS_XPNDR_MODE_POS3;
							//				sendMessageInt(KEY_COMMAND_FMS_XPNDR_MODE_POS3, 0);
							//			}
							//		}
							//		break;

							//	case 4:
							//		// STBY
							//		stbySelected = true;
							//		if(xpndr_mode != KEY_COMMAND_FMS_XPNDR_MODE_POS2)
							//		{
							//			xpndr_mode = KEY_COMMAND_FMS_XPNDR_MODE_POS2;
							//			sendMessageInt(KEY_COMMAND_FMS_XPNDR_MODE_POS2, 0);
							//		}
							//		break;


							//	case 1:
							//		// TA
							//		if(xpndr_mode != KEY_COMMAND_FMS_XPNDR_MODE_POS5)
							//		{
							//			xpndr_mode = KEY_COMMAND_FMS_XPNDR_MODE_POS5;
							//			sendMessageInt(KEY_COMMAND_FMS_XPNDR_MODE_POS5, 0);
							//		}
							//		break;

							//	case 0X80:
							//		// TA/RA
							//		if(xpndr_mode != KEY_COMMAND_FMS_XPNDR_MODE_POS6)
							//		{
							//			xpndr_mode = KEY_COMMAND_FMS_XPNDR_MODE_POS6;
							//			sendMessageInt(KEY_COMMAND_FMS_XPNDR_MODE_POS6, 0);
							//		}
							//		break;
							//}


							//// update IDENT
							//if((buttons & 0X20) == 0X20)
							//{
							//	sendMessageInt(KEY_COMMAND_FMS_XPNDR_IDENT, 0);
							//}
						}
						i++;
					} //  loop
				}
				else
				{
					Available(false);
				}
			} // Bytes > 0
		}
		else
		{
			Available(false);
		}
	}
}