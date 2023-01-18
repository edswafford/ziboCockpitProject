#pragma once

#include <mutex>
#include <unordered_map>
#include <map>
#include <string>

#include <windows.h>
#include "ftd2xx.h"

using namespace std;


class Ftd2xxDevices
{
public:
	~Ftd2xxDevices();
	Ftd2xxDevices(const Ftd2xxDevices&) = delete;
	Ftd2xxDevices& operator=(const Ftd2xxDevices&) = delete;

	static Ftd2xxDevices* instance()
	{
		static mutex mutex;
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

	void drop()
	{
		static mutex mutex;
		std::lock_guard<std::mutex> lock(mutex);
		delete xpndrInstance;
		xpndrInstance = 0;
	}



	FT_DEVICE_LIST_INFO_NODE* getDevice(string serialNumber)
	{
		get_devices();
		if(devices.find(serialNumber) != devices.end())
		{
			return (FT_DEVICE_LIST_INFO_NODE *)devices[serialNumber];
		}
		return nullptr;
	}


	void get_devices();
	void closeDown();

private:
	Ftd2xxDevices()
	{
	}



	static Ftd2xxDevices* xpndrInstance;

	unordered_map<std::string, FT_DEVICE_LIST_INFO_NODE *> devices;
};

