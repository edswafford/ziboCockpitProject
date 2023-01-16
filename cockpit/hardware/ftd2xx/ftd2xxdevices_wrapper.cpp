#include "ftd2xxdevices_wrapper.hpp"

Ftd2xxDevices * Ftd2xxDevices::xpndrInstance = nullptr;


void Ftd2xxDevices::closeDown()
{
    for ( const auto& next_device : devices)
    {
        FT_DEVICE_LIST_INFO_NODE * device = (FT_DEVICE_LIST_INFO_NODE *)next_device.second;
        free(device);
    }

    drop();
}


void Ftd2xxDevices::get_devices()
{
    FT_STATUS ftStatus;
    FT_DEVICE_LIST_INFO_NODE *devInfo;
    DWORD numDevs;

    // clear list of devices
    devices.clear();

    // create the device information list 
    ftStatus = FT_CreateDeviceInfoList(&numDevs);

    if (ftStatus == FT_OK) { 
        if (numDevs > 0) {
            // allocate storage for list based on numDevs 
            devInfo = (FT_DEVICE_LIST_INFO_NODE*)malloc(sizeof(FT_DEVICE_LIST_INFO_NODE)*numDevs);
            if(devInfo != nullptr) {
                // get the device information list 
                ftStatus = FT_GetDeviceInfoList(devInfo, &numDevs);

                if (ftStatus == FT_OK) { 
                    for (unsigned i = 0; i < numDevs; i++) { 
                        string deviceSerialNumber = string(devInfo[i].SerialNumber);

                        // add device
                        FT_DEVICE_LIST_INFO_NODE * device = new FT_DEVICE_LIST_INFO_NODE();
                        device->ID = devInfo[i].ID;
                        device->Flags = devInfo[i].Flags;
                        device->Type = devInfo[i].Type;
                        device->LocId = devInfo[i].LocId;
                        device->ftHandle = devInfo[i].ftHandle;
                        devices.emplace(deviceSerialNumber, device);                        
                    }
                }
                free(devInfo);
            }
        }
    }
}
