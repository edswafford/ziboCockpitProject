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

	static Ftd2xxDevices* instance();

	static void drop()
	{
		static std::mutex mutex;
		std::lock_guard<std::mutex> lock(mutex);
//		delete xpndrInstance;
		xpndrInstance = nullptr;
	}



	FT_DEVICE_LIST_INFO_NODE* getDevice(const std::string serial_number);


	void get_devices();
	void closeDown();

private:

	static std::unique_ptr<Ftd2xxDevices> xpndrInstance;

	std::unordered_map<std::string, std::unique_ptr<FT_DEVICE_LIST_INFO_NODE>> devices;
};

