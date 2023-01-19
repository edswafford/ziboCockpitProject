#pragma once

#include <mutex>
#include <unordered_map>
#include <map>
#include <string>

#include <windows.h>
#include "ftd2xx.h"



class Ftd2xxDevices
{
public:
	Ftd2xxDevices() = default;
	~Ftd2xxDevices();
	Ftd2xxDevices(const Ftd2xxDevices&) = delete;
	Ftd2xxDevices& operator=(const Ftd2xxDevices&) = delete;


	FT_DEVICE_LIST_INFO_NODE* getDevice(const std::string serial_number);


	void get_devices();


private:

	std::unordered_map<std::string, std::unique_ptr<FT_DEVICE_LIST_INFO_NODE>> devices;
};

