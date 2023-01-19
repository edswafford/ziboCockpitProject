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
	~Ftd2xxDevices();
	Ftd2xxDevices(const Ftd2xxDevices&) = delete;
	Ftd2xxDevices& operator=(const Ftd2xxDevices&) = delete;

	static Ftd2xxDevices* instance()
	{
		static std::mutex mutex;
		if(!xpndrInstance)
		{
			std::lock_guard<std::mutex> lock(mutex);
			if(!xpndrInstance)
			{
				xpndrInstance = new Ftd2xxDevices;
			}
		}
		return xpndrInstance;
	}

	static void drop()
	{
		static std::mutex mutex;
		std::lock_guard<std::mutex> lock(mutex);
		delete xpndrInstance;
		xpndrInstance = nullptr;
	}



	FT_DEVICE_LIST_INFO_NODE* getDevice(const std::string serialNumber)
	{
		get_devices();
		if(devices.contains(serialNumber))
		{
			return (FT_DEVICE_LIST_INFO_NODE *)devices[serialNumber];
		}
		return nullptr;
	}


	void get_devices();
	void closeDown() const;

private:
	Ftd2xxDevices() = default;


	static Ftd2xxDevices* xpndrInstance;
	void free_devices() const;

	std::unordered_map<std::string, FT_DEVICE_LIST_INFO_NODE *> devices;
};

