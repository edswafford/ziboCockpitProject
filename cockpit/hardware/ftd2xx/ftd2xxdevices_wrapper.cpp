#include "ftd2xxdevices_wrapper.hpp"
#include "../common/logger.hpp"

extern logger LOG;

Ftd2xxDevices * Ftd2xxDevices::xpndrInstance = nullptr;


void Ftd2xxDevices::free_devices() const
{
	if (!devices.empty()) {
		for (const auto& next_device : devices)
		{
			const auto device = static_cast<FT_DEVICE_LIST_INFO_NODE*>(next_device.second);
			free(device);
		}
	}
}

void Ftd2xxDevices::closeDown() const
{
    free_devices();
    drop();
}

Ftd2xxDevices::~Ftd2xxDevices() {
    LOG() << "	closing Ftd2xxDevices";
}
void Ftd2xxDevices::get_devices()
{
	DWORD numDevs;

    // clear list of devices
    free_devices();
    devices.clear();

    // create the device information list 
    FT_STATUS ft_status = FT_CreateDeviceInfoList(&numDevs);

    if (ft_status == FT_OK) { 
        if (numDevs > 0) {
	        // allocate storage for list based on numDevs 
	        auto dev_info = static_cast<FT_DEVICE_LIST_INFO_NODE*>(malloc(sizeof(FT_DEVICE_LIST_INFO_NODE) * numDevs));
            if(dev_info != nullptr) {
                // get the device information list 
                ft_status = FT_GetDeviceInfoList(dev_info, &numDevs);

                if (ft_status == FT_OK) { 
                    for (unsigned i = 0; i < numDevs; i++) {
	                    std::string deviceSerialNumber = std::string(dev_info[i].SerialNumber);

                        // add device
	                    auto device = new FT_DEVICE_LIST_INFO_NODE();
                        device->ID = dev_info[i].ID;
                        device->Flags = dev_info[i].Flags;
                        device->Type = dev_info[i].Type;
                        device->LocId = dev_info[i].LocId;
                        device->ftHandle = dev_info[i].ftHandle;
                        devices.emplace(deviceSerialNumber, device);                        
                    }
                }
                free(dev_info);
            }
        }
    }
}
