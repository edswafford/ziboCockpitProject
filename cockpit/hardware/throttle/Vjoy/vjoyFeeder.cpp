#include <windows.h>
#include "VjoyFeeder.h"
#include "../common/logger.hpp"

extern logger LOG;

namespace zcockpit::cockpit::hardware
{
	VjoyFeeder::~VjoyFeeder()
	{
		if (state == Vjoy_State::Valid)
		{
			RelinquishVJD(dev_id);
		}
	}

	Vjoy_State VjoyFeeder::init_vjoy(const unsigned id)
	{
		dev_id = id;

		// Get the driver attributes (Vendor ID, Product ID, Version Number)
		if (!vJoyEnabled())
		{
			LOG() << "Function vJoyEnabled Failed - make sure that vJoy is installed and enabled.";
			state = Vjoy_State::Failed_To_Enable;
		}
		else
		{
			const auto product_string = static_cast<char *>(GetvJoyProductString());
			const auto serial_number = static_cast<char *>(GetvJoySerialNumberString());

			LOG() << "Vendor: " << product_string << " Product: " << serial_number << " Version Number: ";


			// Get the status of the vJoy device before trying to acquire it
			const VjdStat status = GetVJDStatus(dev_id);

			switch (status)
			{
			case VJD_STAT_OWN:
				LOG() << "vJoy device " << dev_id << " is already owned by this feeder";
				state = Vjoy_State::Valid;
				break;

			case VJD_STAT_FREE:
				LOG() << "vJoy device " << dev_id << " is free";
				state = Vjoy_State::Valid;
				break;

			case VJD_STAT_BUSY:
				LOG() << "vJoy device " << dev_id << " is already owned by another feeder.  Cannot continue";
				state = Vjoy_State::Already_Owned_By_Another_Feeder;
				break;

			case VJD_STAT_MISS:
				LOG() << "vJoy device " << " is not installed or disabled.  Cannot continue.";
				state = Vjoy_State::Disabled;
				break;
			default:
				LOG() << "vJoy device " << dev_id << " general error.  Cannot continue.";
				state = Vjoy_State::Unknown_Status;
				break;
			}
		}
		if(state != Vjoy_State::Valid)
		{
			RelinquishVJD(dev_id);
		}
		return state;
	}

	bool VjoyFeeder::update(const Axis & axis)
	{
		// Set destenition vJoy device
		vjoy_data.bDevice = dev_id;

		// Set position data of 3 first axes
		vjoy_data.wAxisX = axis.axisX;
		vjoy_data.wAxisY = axis.axisY;
		vjoy_data.wAxisZ = axis.axisZ;
		vjoy_data.wAxisXRot = axis.axisR;
		
		vjoy_data.wAxisYRot = axis.axisS;
		vjoy_data.wAxisZRot = axis.axisT;
		vjoy_data.wSlider = axis.axisU;
		vjoy_data.wDial = axis.axisV;

		return UpdateVJD(dev_id, static_cast<PVOID>(&vjoy_data)) != 0;
	}

	bool VjoyFeeder::update(const Axis& axis, const LONG lButtons)
	{
		// Set destenition vJoy device
		vjoy_data.bDevice = dev_id;

		// Set position data of 3 first axes
		vjoy_data.wAxisX = axis.axisX;
		vjoy_data.wAxisY = axis.axisY;
		vjoy_data.wAxisZ = axis.axisZ;
		vjoy_data.wAxisXRot = axis.axisR;
		vjoy_data.wAxisYRot = axis.axisS;
		vjoy_data.wAxisZRot = axis.axisT;
		vjoy_data.wSlider = axis.axisU;
		vjoy_data.wDial = axis.axisV;
		vjoy_data.lButtons = lButtons;

		return UpdateVJD(dev_id, static_cast<PVOID>(&vjoy_data)) != 0;
	}

	bool VjoyFeeder::aquire() const
	{
		if (state == Vjoy_State::Valid)
		{
			// Acquire the vJoy device
			if (AcquireVJD(dev_id))
			{
				LOG() << "Acquired device number " << dev_id;
				return true;
			}
			LOG() << "Failed to acquire vJoy device number " << dev_id;
		}
		else
		{
			LOG() << "Cannot Aquire device " << dev_id << "  Because the Device State is not Valid.";
		}
		return false;
	}
}
