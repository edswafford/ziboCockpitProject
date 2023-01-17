#ifndef _POKEYSLIB_H_
#define _POKEYSLIB_H_


struct sPoKeysDevice
{
	unsigned char userID;
	unsigned int serialNr;
	unsigned char VersionMajor;
	unsigned char VersionMinor;
	unsigned char deviceIP[4];
	unsigned char DHCPstatus;
	unsigned char hostIP[4];
};


struct sPoKeysPinData
{
	/// <summary>
	/// Pin function code
	/// </summary>
	unsigned char PinFunction;
	unsigned char CounterOptions;

	unsigned char DigitalInputValue;
	unsigned char DigitalOutputValue;
	int DigitalCounterValue;
	unsigned char DigitalCounterAvailable;

	int AnalogValue;
	int PWMduty;

	unsigned char MappingType;

	unsigned char KeyCodeMacroID;
	unsigned char KeyModifier;

	unsigned char downKeyCodeMacroID;
	unsigned char downKeyModifier;

	unsigned char upKeyCodeMacroID;
	unsigned char upKeyModifier;
};

struct sPoKeysDeviceData
{
	/// <summary>
	/// Serial number
	/// </summary>
	int SerialNumber;
	/// <summary>
	/// Firmware version - major revision
	/// </summary>
	int FirmwareVersionMajor;
	/// <summary>
	/// Firmware version - minor revision
	/// </summary>
	int FirmwareVersionMinor;

	/// <summary>
	/// User ID
	/// </summary>
	unsigned char UserID;
	/// <summary>
	/// Device name
	/// </summary>
	char DeviceName[11];
	/// <summary>
	/// Type of device (string)
	/// </summary>
	char DeviceTypeName[25];

	/// <summary>
	/// Build date
	/// </summary>
	char BuildDate[12];
};

struct sPoKeysEncoder
{
	unsigned char encoderOptions;

	unsigned char channelApin;
	unsigned char channelBpin;

	unsigned char dirAkeyCode;
	unsigned char dirAkeyModifier;

	unsigned char dirBkeyCode;
	unsigned char dirBkeyModifier;

	int encoderValue;
};

struct sMatrixKeyboard
{
	unsigned char matrixKBconfiguration;
	unsigned char matrixKBwidth;
	unsigned char matrixKBheight;

	unsigned char matrixKBcolumnsPins[8];
	unsigned char matrixKBrowsPins[16];
	unsigned char macroMappingOptions[128];
	unsigned char keyMappingKeyCode[128];
	unsigned char keyMappingKeyModifier[128];
	unsigned char keyMappingTriggeredKey[128];
	unsigned char keyMappingKeyCodeUp[128];
	unsigned char keyMappingKeyModifierUp[128];

	unsigned char matrixKBvalues[128];
};

struct sPoKeysDeviceStatus
{
	/// <summary>
	/// Structure that holds basic device data
	/// </summary>
	sPoKeysDeviceData DeviceData;

	/// <summary>
	/// An array of 55 pins, each with its own settings
	/// </summary>
	sPoKeysPinData Pins[55];

	/// <summary>
	/// An array of 26 encoders, each with its own settings
	/// </summary>
	sPoKeysEncoder Encoders[26];

	/// <summary>
	/// Matrix keyboard settings
	/// </summary>
	sMatrixKeyboard matrixKB;
};


int InitWinsock();
int TerminateWinsock();
int EnumeratePoKeysDevices(sPoKeysDevice* devicesList);
int ConnectToPoKeysDevice(sPoKeysDevice* device);
int DisconnectPoKeysDevice();
unsigned char CalculateChecksum(unsigned char* packet);
int SendEthRequestEx(unsigned char type, unsigned char useExtended, unsigned char data3, unsigned char data4, unsigned char data5, unsigned char* Request, unsigned char* Response);


int GetDeviceType(sPoKeysDeviceStatus* deviceStat);
int GetDeviceData(sPoKeysDeviceStatus* deviceStat);
int GetDigitalInputsStatus(sPoKeysDeviceStatus* deviceStat);
int SetDigitalOutputsStatus(sPoKeysDeviceStatus* deviceStat);
int SetDigitalOutputs(sPoKeysDeviceStatus* deviceStat);

int SetPinFunctions(sPoKeysDeviceStatus* deviceStat);
void StepReverseDefault(sPoKeysDeviceStatus* deviceStat);
int GetDigitalAnalogInputs(sPoKeysDeviceStatus* deviceStat, unsigned char pin);
int GetAllAnalogInputs(int* analogs, const unsigned char pin);

#define pinFunctionInactive			0
#define pinFunctionDigitalInput		2
#define pinFunctionDigitalOutput	4
#define pinFunctionAnalogInput		8
#define pinFunctionAnalogOutput		16
#define pinFunctionTriggeredInput	32
#define pinInvert					128


#endif
