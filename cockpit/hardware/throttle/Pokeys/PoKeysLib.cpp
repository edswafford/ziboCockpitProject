#include "windows.h"
#include "stdio.h"
#include "string.h"
#include "PoKeysLib.h"
#include "../common/logger.hpp"

#include <winsock.h>

unsigned char RequestID = 0;
WSADATA wsaData;
SOCKET comSocket;


extern logger LOG;

int InitWinsock()
{
	// Initialize Winsock - version 2.2
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		LOG() << "Pokeys: Winsock startup failed!\n";
		return -1;
	}
	return 0;
}

int TerminateWinsock()
{
	WSACleanup();
	return 0;
}

int EnumeratePoKeysDevices(sPoKeysDevice* devicesList)
{
	sockaddr_in remoteEP;

	LOG() << "Pokeys: Enumerating network PoKeys devices...";

	// Set up the RecvAddr structure with the broadcast ip address and correct port number
	remoteEP.sin_family = AF_INET;
	remoteEP.sin_port = htons(20055);
	remoteEP.sin_addr.s_addr = inet_addr("255.255.255.255");

	//Broadcast the message
	int t = 100; // 100 ms timeout
	int UDPbroadcast = 1;
	int BufLen = 0;
	char SendBuf[1];

	// Create socket for discovery packet
	const SOCKET txSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	setsockopt(txSocket, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<char *>(&UDPbroadcast), sizeof UDPbroadcast);
	setsockopt(txSocket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char *>(&t), sizeof(t));
	setsockopt(txSocket, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<char *>(&t), sizeof(t));
	int status = sendto(txSocket, SendBuf, BufLen, 0, reinterpret_cast<SOCKADDR *>(&remoteEP), sizeof(remoteEP));

	// Was there an error?
	if (status == SOCKET_ERROR)
	{
		LOG() << "Pokeys: Failed to open socket.";
		closesocket(txSocket);
		WSACleanup();
		return -1;
	}
	else
	{
		LOG() << "Pokeys: Discovery packet sent!";
	}

	LOG() << "Pokeys: Waiting for responses...";

	unsigned char rcvbuf[500];

	struct sockaddr remoteAddr;
	int remoteLen = sizeof(struct sockaddr);
	int nrOfDetectedBoards = 0;

	do
	{
		// Receive response from devices
		status = recvfrom(txSocket, reinterpret_cast<char *>(rcvbuf), sizeof(rcvbuf), 0, &remoteAddr, &remoteLen);

		// Get IP address and receive message
		if (status > 0)
		{
			// Parse the response
			LOG() << "Pokeys: Received response";

			LOG() << "  User ID: " << rcvbuf[0];
			LOG() << "  Serial: " << static_cast<int>(256 * static_cast<int>(rcvbuf[1]) + rcvbuf[2]);
			LOG() << "  Version: " << std::to_string(rcvbuf[3]) << "." << std::to_string(rcvbuf[4]);
			LOG() << "  IP address: " << std::to_string(rcvbuf[5]) << "." << std::to_string(rcvbuf[6]) << "." << std::to_string(rcvbuf[7]) << "." << std::to_string(rcvbuf[8]);
			LOG() << "  DHCP: " << std::to_string(rcvbuf[9]);
			LOG() << "  Host IP address: " << std::to_string(rcvbuf[10]) << "." << std::to_string(rcvbuf[11]) << "." << std::to_string(rcvbuf[12]) << "." << std::to_string(rcvbuf[13]);

			// Save the device info
			sPoKeysDevice * device = &devicesList[nrOfDetectedBoards];
			device->userID = rcvbuf[0];
			device->serialNr = static_cast<int>(256 * static_cast<int>(rcvbuf[1]) + rcvbuf[2]);
			device->VersionMajor = rcvbuf[3];
			device->VersionMinor = rcvbuf[4];
			memcpy(device->deviceIP, rcvbuf + 5, 4);
			device->DHCPstatus = rcvbuf[9];
			memcpy(device->hostIP, rcvbuf + 10, 4);

			nrOfDetectedBoards++;
			status = 0;
		}
		else
		{
			if (nrOfDetectedBoards == 0)
			{
				LOG() << "Pokeys: No Boards detected";
			}
		}
	} while (status != SOCKET_ERROR && nrOfDetectedBoards < 16)
		;

	if (closesocket(txSocket))
	{
		LOG() << "Pokeys: Close Socket Failed, error code " << WSAGetLastError();
	}
	return nrOfDetectedBoards;
}

int ConnectToPoKeysDevice(sPoKeysDevice* device)
{
	auto t = 100; // 100 ms timeout

	// Create socket
	comSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (comSocket == INVALID_SOCKET)
	{
		return -1; // Couldn't create the socket
	}

	// Set socket options
	setsockopt(comSocket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char *>(&t), sizeof(t));
	setsockopt(comSocket, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<char *>(&t), sizeof(t));

	// Create target endpoint
	sockaddr_in remoteEP;

	// Put IP address into the string
	char addrBuf[20];
	sprintf_s(addrBuf, "%u.%u.%u.%u", device->deviceIP[0], device->deviceIP[1], device->deviceIP[2], device->deviceIP[3]);

	// Set up the RecvAddr structure with the broadcast ip address and correct port number
	remoteEP.sin_family = AF_INET;
	remoteEP.sin_port = htons(20055);
	remoteEP.sin_addr.s_addr = inet_addr(addrBuf);

	// Connect to target IP
	if (connect(comSocket, reinterpret_cast<SOCKADDR *>(&remoteEP), sizeof(remoteEP)) == SOCKET_ERROR)
	{
		return -1; // Couldn't connect
	}

	LOG() << "Connected to PoKeys device at " << addrBuf;
	return 0;
}

int DisconnectPoKeysDevice()
{
	if (comSocket)
	{
		closesocket(comSocket);
	}
	return 0;
}

unsigned char CalculateChecksum(unsigned char* packet)
{
	int sum = 0;

	for (int n = 0; n < 7; n++)
	{
		sum += packet[n];
	}

	return static_cast<unsigned char>(sum % 0x100);
}

int SendRequest(const unsigned char type, const unsigned char data2, const unsigned char data3, const unsigned char data4, const unsigned char data5, unsigned char* Request, unsigned char* Response)
{
	auto retries1 = 0;
	auto retries2 = 0;

	unsigned char txBuffer[64];
	unsigned char rxBuffer[64];

	while (true)
	{
		// Form the request packet
		RequestID++;

		txBuffer[0] = 0xBB;
		txBuffer[1] = type;
		txBuffer[2] = data2;
		txBuffer[3] = data3;
		txBuffer[4] = data4;
		txBuffer[5] = data5;
		txBuffer[6] = RequestID;
		txBuffer[7] = CalculateChecksum(txBuffer);

		memcpy(txBuffer + 8, Request + 8, 56);

		// Send the data
		if (send(comSocket, reinterpret_cast<char *>(txBuffer), 64, 0) != 64)
		{
			LOG() << "Pokeys: Error sending TCP report  --  Aborting...";
			return -1;
		}

		// Wait for the response
		while (true)
		{
			const int result = recv(comSocket, reinterpret_cast<char *>(rxBuffer), 64, 0);

			// 64 bytes received?
			if (result == 64)
			{
				if (rxBuffer[0] == 0xAA && rxBuffer[6] == RequestID)
				{
					if (rxBuffer[7] == CalculateChecksum(rxBuffer))
					{
						memcpy(Response, rxBuffer, 64);
						return 0;
					}
				}
			}
			else if (result == 0) {
				LOG() << "Connection closed\n";
			}
			else
			{
				const auto wsa_error = WSAGetLastError();
				if (wsa_error == WSAETIMEDOUT)
				{
					LOG() << "Pokeys: Winsock recv failed: WSA Timeout";
				}
			}

			if (++retries1 > 2)
			{
				LOG() << "Pokeys: Waiting for response retries greater than " << 2;
				retries1 = 0;
				break;
			}
		}

		if (retries2++ > 2)
		{
			LOG() << "Pokeys: Sending Request retries greater than 2";
			break;
		}
	}

	return -1;
}

int GetDeviceType(sPoKeysDeviceStatus* deviceStat)
{
	if (deviceStat->DeviceData.SerialNumber == 0xFFFF)
	{
		return 3; // old bootloader - recovery mode
	}
	if (deviceStat->DeviceData.SerialNumber >= 20000)
	{
		if ((deviceStat->DeviceData.FirmwareVersionMajor & (1 << 7)) > 0)
		{
			return 16;  // bootloader mode
		}
		return 11;
	}
	if (deviceStat->DeviceData.SerialNumber >= 11500)
	{
		return 2;
	}
	if (deviceStat->DeviceData.SerialNumber >= 10113)
	{
		return 1;
	}

	return 0;
}

int GetDeviceData(sPoKeysDeviceStatus* deviceStat)
{
	unsigned char tempOut[64];
	unsigned char tempIn[64];

	// Get serial number and versions
	if (SendRequest(0x00, 0, 0, 0, 0, tempOut, tempIn) != 0)
	{
		return -1;
	}
	deviceStat->DeviceData.SerialNumber = static_cast<int>(tempIn[2]) * 256 + static_cast<int>(tempIn[3]);
	deviceStat->DeviceData.FirmwareVersionMajor = tempIn[4];
	deviceStat->DeviceData.FirmwareVersionMinor = tempIn[5];

	// Get User ID
	if (SendRequest(0x03, 0, 0, 0, 0, tempOut, tempIn) != 0)
	{
		return -1;
	}
	deviceStat->DeviceData.UserID = tempIn[2];

	// Build data
	if (SendRequest(0x04, 0, 0, 0, 0, tempOut, tempIn) != 0)
	{
		return -1;
	}
	memcpy(deviceStat->DeviceData.BuildDate + 0, tempIn + 2, 4);
	if (SendRequest(0x04, 1, 0, 0, 0, tempOut, tempIn) != 0)
	{
		return -1;
	}
	memcpy(deviceStat->DeviceData.BuildDate + 4, tempIn + 2, 4);
	if (SendRequest(0x04, 2, 0, 0, 0, tempOut, tempIn) != 0)
	{
		return -1;
	}
	memcpy(deviceStat->DeviceData.BuildDate + 8, tempIn + 2, 3);
	deviceStat->DeviceData.BuildDate[11] = 0;

	switch (GetDeviceType(deviceStat))
	{
	case 0:
		sprintf_s(deviceStat->DeviceData.DeviceTypeName, "%s", "generic PoKeys device");
		break;
	case 1:
		sprintf_s(deviceStat->DeviceData.DeviceTypeName, "%s", "PoKeys55");
		break;
	case 2:
		sprintf_s(deviceStat->DeviceData.DeviceTypeName, "%s", "PoKeys55");
		break;
	case 3:
		sprintf_s(deviceStat->DeviceData.DeviceTypeName, "%s", "PoKeys55 - recovery");
		break;
	case 10:
		sprintf_s(deviceStat->DeviceData.DeviceTypeName, "%s", "PoKeys56U");
		break;
	case 11:
		sprintf_s(deviceStat->DeviceData.DeviceTypeName, "%s", "PoKeys56E");
		break;

	case 15:
		sprintf_s(deviceStat->DeviceData.DeviceTypeName, "%s", "PoKeys56U - recovery");
		break;
	case 16:
		sprintf_s(deviceStat->DeviceData.DeviceTypeName, "%s", "PoKeys56E - recovery");
		break;

	default:
		sprintf_s(deviceStat->DeviceData.DeviceTypeName, "%s", "PoKeys");
		break;
	}

	// Device name
	if (SendRequest(0x06, 0, 0, 0, 0, tempOut, tempIn) != 0)
	{
		return -1;
	}
	memcpy(deviceStat->DeviceData.DeviceName, tempIn + 8, 10);
	deviceStat->DeviceData.DeviceName[10] = 0;

	return 0;
}

int SetPinFunctions(sPoKeysDeviceStatus* deviceStat)
{
	unsigned char tempOut[64];
	unsigned char tempIn[64];

	for (int i = 0; i < 55; i++)
	{
		tempOut[8 + i] = deviceStat->Pins[i].PinFunction;
	}

	// Set pin functions for all pins
	if (SendRequest(0xC0, 1, 0, 0, 0, tempOut, tempIn) != 0)
	{
		LOG() << "Pokeys: Set Pin function error";
		return -1;
	}
	return 0;
}

int GetDigitalInputsStatus(sPoKeysDeviceStatus* deviceStat)
{
	unsigned char tempOut[64];
	unsigned char tempIn[64];

	// Get full device report
	if (SendRequest(0xCC, 0, 0, 0, 0, tempOut, tempIn) != 0) return -1;

	for (int i = 0; i < 55; i++)
	{
		deviceStat->Pins[i].DigitalInputValue = static_cast<unsigned char>((tempIn[8 + (i / 8)] & (1 << (i % 8))) > 0 ? 1 : 0);
	}

	return 0;
}

int GetDigitalAnalogInputs(sPoKeysDeviceStatus* deviceStat, const unsigned char pin)
{
	unsigned char tempOut[64];
	unsigned char tempIn[64];

	// Get full device report
	if (SendRequest(0x35, pin, 0, 0, 0, tempOut, tempIn) != 0) return -1;

	auto status = tempIn[3 - 1];
	//auto inputValue = tempIn[4 - 1];
	const auto msb = tempIn[5 - 1];
	const auto lsb = tempIn[6 - 1];

	deviceStat->Pins[pin].AnalogValue = (msb << 8) + lsb;

	return status;
}

int GetAllAnalogInputs(int* analog, const unsigned char pin)
{
	unsigned char tempOut[64];
	unsigned char tempIn[64];

	// Get full device report
	if (SendRequest(0x3A, pin, 6, 0, 0, tempOut, tempIn) != 0) return -1;

	auto status = 0;
	//auto inputValue = tempIn[4 - 1];

	auto msb = tempIn[9 - 1];
	auto lsb = tempIn[10 - 1];
	analog[0] = (msb << 8) + lsb;

	msb = tempIn[11 - 1];
	lsb = tempIn[12 - 1];
	analog[1] = (msb << 8) + lsb;

	msb = tempIn[13 - 1];
	lsb = tempIn[14 - 1];
	analog[2] = (msb << 8) + lsb;

	msb = tempIn[15 - 1];
	lsb = tempIn[16 - 1];
	analog[3] = (msb << 8) + lsb;

	msb = tempIn[17 - 1];
	lsb = tempIn[18 - 1];
	analog[4] = (msb << 8) + lsb;

	msb = tempIn[19 - 1];
	lsb = tempIn[20 - 1];
	analog[5] = (msb << 8) + lsb;
	return status;
}

int SetDigitalOutputsStatus(sPoKeysDeviceStatus* device_stat)
{
	unsigned char temp_out[64] = { 0 };
	unsigned char temp_in[64];

	for (auto i = 0; i < 55; i++)
	{
		if (device_stat->Pins[i].DigitalOutputValue == 0)
		{
			temp_out[8 + i / 8] |= static_cast<unsigned char>(1 << (i % 8));
		}
	}
	// Set full device report
	if (SendRequest(0xCC, 1, 0, 0, 0, temp_out, temp_in) != 0) return -1;

	for (auto i = 0; i < 55; i++)
	{
		device_stat->Pins[i].DigitalInputValue = static_cast<unsigned char>((temp_in[8 + (i / 8)] & (1 << (i % 8))) > 0 ? 1 : 0);
	}

	return 0;
}

int SetDigitalOutputs(sPoKeysDeviceStatus* device_stat)
{
	unsigned char temp_out[64] = { 0 };
	unsigned char temp_in[64];

	for (auto i = 0; i < 55; i++)
	{
		if (device_stat->Pins[i].PinFunction == pinFunctionDigitalOutput)
		{
			temp_out[8 + i / 8] |= static_cast<unsigned char>(device_stat->Pins[i].DigitalOutputValue << (i % 8));
		}
	}
	// Set full device report
	if (SendRequest(0xCC, 1, 0, 0, 0, temp_out, temp_in) != 0) return -1;

	for (auto i = 0; i < 55; i++)
	{
		device_stat->Pins[i].DigitalInputValue = static_cast<unsigned char>((temp_in[8 + (i / 8)] & (1 << (i % 8))) > 0 ? 1 : 0);
	}

	return 0;
}