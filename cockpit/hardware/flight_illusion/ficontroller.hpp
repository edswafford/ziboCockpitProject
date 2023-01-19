#pragma once
#include <vector>
#include <windows.h>

#include "ftd2xx.h"
#include "../ftd2xx/ftd2xxdevices_wrapper.hpp"
#include "fidevice.hpp"
#include "../../aircraft_model.hpp"

//using namespace std;
namespace zcockpit::cockpit::hardware
{
	class FiController
	{
	public:
		static const bool VALID = true;
		static const char* overheadSerialNumber;
		static const char* mipSerialNumber;
		FT_DEVICE_LIST_INFO_NODE devInfo{};


		bool Available() const
		{
			return available;
		}

		void Available(bool val)
		{
			available = val;
		}

		FT_HANDLE FtHandle() const
		{
			return ftDeviceHandle;
		}

		bool updateGauges() const;
		void validateGauges();
		void updateLights(int light_state) const;
		bool updateRadios() const;

		FiController() = delete;
		FiController(AircraftModel& ac_model, int updates_per_second);
		~FiController();

		void addGauge(int ID, FiDevice::DEVICE_Type type, double scaleFactor, int minValue, int maxValue, int offset, double K, FiDevice::FI_DEVICE_CMD needleCmd, int max_send_count = 3);
		void addRadio(int radioID, FiDevice::DEVICE_Type type, double scaleFactor, int minValue, int maxValue,
		              FiDevice::FI_DEVICE_CMD radioCmd);
		void initialize(const char* deviceSerialNumber, const FT_DEVICE_LIST_INFO_NODE* devInfo);
		void open(const char* deviceSerialNumber);
		void closeDown();

		void drop();

		void timer(FT_HANDLE handle);
		void start_timer(FT_HANDLE handle);

		static std::map<int, FiDevice *> devices_map_by_id;

	private:
		void openEx();
		void reopen(const char* deviceSerialNumber);
		void setTimeouts(int readTimeout, int writeTimeout);

		AircraftModel& aircraft_model;


		Ftd2xxDevices* ftd2Devices;

		FT_HANDLE ftDeviceHandle;

		std::vector<std::unique_ptr<FiDevice>> devices;

		std::vector<std::unique_ptr<FiDevice>> radio_devices;
		const char* serialNumber;
		bool available{false};
		bool valid{false};

		int deviceValidationIndex{0};
		int number_of_radios{0};

		// TIMER
		int updates_per_second{ 5 };

		bool stop_timer{ true };
		bool timer_has_stopped{ true };

		mutable std::mutex flightIllusion_timer_mutex;
		mutable std::mutex timer_done_mutex;
		mutable std::condition_variable condition;

		std::thread timer_thread;
		bool timer_is_running{ false };

		//
		// Threads
		//
		std::vector<std::thread> threads;

	};

}