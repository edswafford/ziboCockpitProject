#pragma once

#ifdef WIN32
#include <windows.h>
#endif

#include "interfaceITAPI_Data.h"
#include <string>
#include <vector>
#include "../usb/hidapi.h"
#include <condition_variable>
#include <stack>
#include <array>
#include <atomic>

class Timer_Data
{
public:
	Timer_Data()
	{
	}
	~Timer_Data()
	{
		if(!timer_has_stopped) {
			drop();
		}
	}

	Timer_Data(Timer_Data&& t)
	{
		WriteLock rhs_lk(t.interfaceIT_timer_mutex);
		stop_timer = t.stop_timer;
		timer_has_stopped = t.timer_has_stopped;
		ls_sw_state = t.ls_sw_state;
		ms_sw_state = t.ms_sw_state;
		if (t.ls_sw_state_ptr == nullptr)
		{
			ls_sw_state_ptr = nullptr;
		}
		else
		{
			ls_sw_state_ptr = &ls_sw_state;
		}
		if (t.ms_sw_state_ptr == nullptr)
		{
			ms_sw_state_ptr = nullptr;
		}
		else
		{
			ms_sw_state_ptr = &ms_sw_state;
		}

	}
	Timer_Data(const Timer_Data& t)
	{
		ReadLock  rhs_lk(t.interfaceIT_timer_mutex);
		stop_timer = t.stop_timer;
		timer_has_stopped = t.timer_has_stopped;
		ls_sw_state = t.ls_sw_state;
		ms_sw_state = t.ms_sw_state;
		if (t.ls_sw_state_ptr == nullptr)
		{
			ls_sw_state_ptr = nullptr;
		}
		else
		{
			ls_sw_state_ptr = &ls_sw_state;
		}
		if (t.ms_sw_state_ptr == nullptr)
		{
			ms_sw_state_ptr = nullptr;
		}
		else
		{
			ms_sw_state_ptr = &ms_sw_state;
		}

	}
	Timer_Data& operator=(const Timer_Data& t)
	{
		if (this != &t)
		{
			WriteLock lhs_lk(interfaceIT_timer_mutex, std::defer_lock);
			ReadLock  rhs_lk(t.interfaceIT_timer_mutex, std::defer_lock);
			std::lock(lhs_lk, rhs_lk);
			stop_timer = t.stop_timer;
			timer_has_stopped = t.timer_has_stopped;
			ls_sw_state = t.ls_sw_state;
			ms_sw_state = t.ms_sw_state;
			if(t.ls_sw_state_ptr == nullptr)
			{
				ls_sw_state_ptr = nullptr;
			}
			else
			{
				ls_sw_state_ptr = &ls_sw_state;
			}
			if(t.ms_sw_state_ptr == nullptr)
			{
				ms_sw_state_ptr = nullptr;
			}
			else
			{
				ms_sw_state_ptr = &ms_sw_state;
			}
		}
		return *this;
	}
	Timer_Data& operator=(const Timer_Data&& t) = delete;

	void timer(hid_device* handle);
	void drop();

	using MutexType = std::mutex;
	using ReadLock = std::unique_lock<MutexType>;
	using WriteLock = std::unique_lock<MutexType>;

	std::stack<std::tuple<int, bool>> switch_stack;

	bool stop_timer{true};
	bool timer_has_stopped{true};

	mutable std::mutex interfaceIT_timer_mutex;
	mutable std::mutex timer_done_mutex;
	mutable std::condition_variable condition;

	uint32_t* ls_sw_state_ptr{ nullptr };
	uint32_t ls_sw_state{ 0 };
	uint32_t* ms_sw_state_ptr{ nullptr };
	uint32_t ms_sw_state{ 0 };

	const long long UPDATE_RATE{50LL};

};

struct InterfaceIT_Devices
{
	std::string path;
	std::string controller;
};

class Interfaceit_Bound_Devices
{
public:
	Interfaceit_Bound_Devices() {}

	Interfaceit_Bound_Devices(Interfaceit_Bound_Devices&& d)
	{
		is_open = d.is_open;
		led_enabled = d.led_enabled;
		sw_poll_enabled = d.sw_poll_enabled;
		thread_running = d.thread_running;
		sw_size = d.sw_size;
		led_size = d.led_size;
		led_state = d.led_state;
		led_buffer = d.led_buffer;
		handle = d.handle;
		tm = d.tm;
	}

	Interfaceit_Bound_Devices(const Interfaceit_Bound_Devices& d)
	{
		is_open = d.is_open;
		led_enabled = d.led_enabled;
		sw_poll_enabled = d.sw_poll_enabled;
		thread_running = d.thread_running;
		sw_size = d.sw_size;
		led_size = d.led_size;
		led_state = d.led_state;
		led_buffer = d.led_buffer;
		handle = d.handle;
		tm = d.tm;
	}

	Interfaceit_Bound_Devices& operator=(const Interfaceit_Bound_Devices& d)
	{
		if (this != &d)
		{
			is_open = d.is_open;
			led_enabled = d.led_enabled;
			sw_poll_enabled = d.sw_poll_enabled;
			thread_running = d.thread_running;
			sw_size = d.sw_size;
			led_size = d.led_size;
			led_state = d.led_state;
			led_buffer = d.led_buffer;
			handle = d.handle;
			tm = d.tm;
		}
		return *this;
	}

	bool is_open{ false };
	bool led_enabled{ false };
	bool sw_poll_enabled{ false };
	bool thread_running{ false };
	int sw_size{ 32 };
	int led_size{ 64 };
	std::array<bool, 256> led_state{ false };
	std::array<uint32_t, 8> led_buffer{ 0 };
	hid_device *handle{ nullptr };
	Timer_Data tm;
};


/**
*	Device change notification
*/
typedef BOOL(CALLBACK* INTERFACEIT_DEVICE_CHANGE_NOTIFY_PROC)(int nAction);

class HidInterfaceIT
{
	
public:
	HidInterfaceIT() = default;;
	~HidInterfaceIT();

	HidInterfaceIT(const HidInterfaceIT&) = delete;
	HidInterfaceIT& operator=(const HidInterfaceIT&) = delete;
	HidInterfaceIT(const HidInterfaceIT&&) = delete;
	HidInterfaceIT& operator=(const HidInterfaceIT&&) = delete;

int pOpenControllers();
int pCloseControllers();
int pBind(std::string controller, unsigned long * session);
int pUnBind(unsigned long session);
int pGetDeviceList(char* buffer, unsigned long* dwSize, char* pBoardType= nullptr);
int pLED_Enable(unsigned long session, bool enable = true);
int pLED_Set(unsigned long session, int nLED, bool on = true);

int pSwitch_Enable_Poll(unsigned long session, bool enable);
int pSwitch_Get_Item(unsigned long session, int* switch_num, int* direction);
int pSwitch_Get_State(unsigned long session, int switch_num, int* state);
int pEnable_Device_Change_CallBack(bool enable, INTERFACEIT_DEVICE_CHANGE_NOTIFY_PROC proc);



void drop();

private:

	bool is_dropped{ false };

//	static HidInterfaceIT* hidInterfaceITInstance;

	std::vector<InterfaceIT_Devices> Devices;
	std::vector<Interfaceit_Bound_Devices> Bound_Devices;

	unsigned char buf[64 + 1]{0};

	//
	// Threads
	//
	std::vector<std::thread> threads;
};
