#pragma once
#include <thread>
#include <memory>
#include <condition_variable>
#include <xtree>
#define _WINSOCK2API_
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#include "../../gui/mainWindow.hpp"
#include "windows.h"
#include "Pokeys/PoKeysLib.h"
#include "Pokeys/Stepper.hpp"
#include "Vjoy/VjoyFeeder.h"
#include "Iir.h"
#include "../../aircraft_model.hpp"

namespace zcockpit::cockpit::hardware
{
	struct JoystickDataStore
	{
		bool trim_up;
		bool trim_dn;
		bool ap_disconnect;
		bool ptt;
		int8_t lButtons;
		int32_t adc[7];
	};

	enum Flap_Position : unsigned
	{
		None = 0,
		Flap_0 = 2,
		Flap_1 = 3,
		Flap_2 = 11,
		Flap_5 = 15,
		Flap_10 = 13,
		Flap_15 = 12,
		Flap_25 = 4,
		Flap_30 = 6,
		Flap_40 = 7,
	};

	//
	// Butterworth filter constants
	//
	constexpr int order = 4; // 4th order (=2 biquads)
	constexpr float samplingrate = 40; // 1000; // Hz
	constexpr float cutoff_frequency = 1;// 5; // Hz

	constexpr double MAX_AXIS = 32768.0;
	constexpr double MAX_SPD_BRAKE = 32768.0;
	constexpr double MAX_THROTTLE = 32768.0;
	constexpr double MAX_REVERSER = 32768.0;

	constexpr double REVRSER_FULLSCALE = -0.316;

	constexpr auto SPDBRK_DELTA = 50;
	extern std::mutex pokeys_mutex;

	class ThrottleAndJoystick
	{
	public:

		ThrottleAndJoystick(AircraftModel& ac_model, MainWindow* mw);
		void drop();
		void static updateBeagleBonejoystick(JoystickDataStore& data);

		bool available() {return throttle_healthy;}

		 void set_enable_realtime_display(const bool enable_realtime_dispay)
		{
			realtime_display_enabled = enable_realtime_dispay;
		}

		 void set_enable_calibration(const bool enable_calibration)
		{
			calibration_enabled = enable_calibration;
		}

		 void set_save_calibration(const bool save_calibration)
		{
			calibration_save = save_calibration;
		}

		 void set_cancel_calibration(const bool cancel_calibration)
		{
			calibration_cancel = cancel_calibration;
		}

		// only used to test stepper motors


		 void set_increment_stepper(const bool increment_stepper_)
		{
			increment_stepper = increment_stepper_;
		}

		 void set_decrement_stepper(const bool decrement_stepper_)
		{
			decrement_stepper = decrement_stepper_;
		}
		
		static sPoKeysDeviceStatus deviceStat_;
		static sPoKeysDeviceStatus deviceStat_flaps;

	private:
		void update_throttle_health();
		void update_realtime_display() const;
		void update_calibration_display() const;
		void initialize_calibration();

		bool init_pokeys();
		void calibrate();
		void save_calibration();
		void process_throttle();
		void timer();
		void set_parking_brake_output(const bool on);
		void process_reverser(const int analog_pin, const int side);
		void filter_analogs(int analog_pin, int index);
		void send_reverser_value(int side, double value) const;
		void process_analogs();
		void process_switches();
		void flaps_xplane_update(const unsigned flaps);

	//	static void update_throttle_HEALTH_text(const char* text, const COLOR color);
		void update_eng1_min_text(const int val) const;
		void update_eng1_text(const int val) const;
		void update_eng1_max_text(const int val) const;
		void update_eng2_min_text(const int val) const;

		void update_eng2_text(const int val) const;
		void update_eng2_max_text(const int val) const;
		void update_spdbrk_min_text(const int val) const;
		void update_spdbrk_text(const int val) const;
		void update_spdbrk_max_text(const int val) const;
		void update_rev1_min_text(const int val) const;
		void update_rev1_text(const int val) const;
		void update_rev1_max_text(const int val) const;
		void update_rev2_min_text(const int val) const;
		void update_rev2_text(const int val) const;
		void update_rev2_max_text(const int val) const;


		std::thread timer_thread;
		std::thread stepper_thread;
		std::thread stepper_thread_for_testing;


		bool stop_timer{true};
		bool timer_has_stopped{true};

		std::mutex throttle_timer_mutex;
		std::mutex timer_done_mutex;
		std::condition_variable condition;

		int current_cycle{ 0 };
		bool sync_switches{ true };
		bool sync_flaps{ true };
		bool pokey_alive{ false };

		// List of detected PoKeys devices
		sPoKeysDevice PoKeysDevices[16];

		Flap_Position previous_flaps{ None };
		
		//
		// Switches
		//
		int eng_1_idle_cutoff{ 0 };
		int eng_2_idle_cutoff{ 0 };
		int toga{ 1 };
		int parking_brake{ 1 };
		int horn_cutout{ 1 };
		int stab_trim_man_cutout{ 0 };
		int stab_trim_auto_cutout{ 0 };

		Step_Controller stepper_ctrl;
		std::unique_ptr<Stepper> left_throttle;
		std::unique_ptr<Stepper> right_throttle;
		std::unique_ptr<Stepper> spoiler;

	public:
		static std::atomic<int> disengage_auto_throttle;
		static std::atomic<bool> auto_throttle_is_disengaged;

		static std::atomic<double> left_n1_commanded;
		static std::atomic<double> right_n1_commanded;
		static std::atomic<bool> enable_at_steppers;
		static bool rev_deployed[2];

	private:
		AircraftModel& aircraft_model;
		MainWindow* main_window{nullptr};
		bool throttle_healthy{ false };

		bool auto_throttle_toggle{ false };

		int previous_speed_brake{ -100 };
		int speed_brake{ -0 };
		bool send_speed_brake{ false };
	//	int rev_filtered[2]{ 0, 0 };
	//	int previous_analog_rev[2]{ 0,0 };
		bool rev_at_min[2]{ true, true };

		int eng_min[2]{ 500, 500 };
	//	int eng_range[2]{ 1000, 1000 };
		int spdbrk_min{ 500 };

		int eng_max[2]{ 2500, 2500 };
		int spdbrk_max{ 4000 };

		int rev_min[2]{ 500, 500 };
		int rev_max[2]{ 2500, 2500 };
		int rev_range[2]{ 2000, 2000 };
		int eng_idle[2]{ 500, 500 };

		int cal_eng_min[2]{ 0, 0 };
		int cal_spdbrk_min{ 0 };
		int cal_rev_min[2]{ 0, 0 };

		int cal_eng_max[2]{ 0, 0 };
		int cal_spdbrk_max{ 0 };
		int cal_rev_max[2]{ 0, 0 };

		int rev_clicks[2]{ 0, 0 };
		double previous_rev[2]{ 0, 0 };
		bool rev_first_pass[2]{ true, true };
		bool rev_wait_for_ann[2]{ false, false };
		bool pmdg_thrust_lever_above_idle[2]{ false, false };
		bool previous_rev_deployed[2]{ false,false };
		bool rev_waiting_for_click_response[2]{ false, false };
		std::chrono::high_resolution_clock::time_point start[2];
		int last_pmdg_change_in_milliseconds[2]{0, 0 };
		double expected_rev_nozzle_position[2]{ 0.0, 0.0 };
		double previous_rev_nozzle_position[2]{ 0.0, 0.0 };

		//double eng_scaler[2]{ MAX_AXIS / 2900.0, MAX_AXIS / 2800.0 };
		//double spdbrk_scaler{ MAX_SPD_BRAKE / 3600.0 };
		//double rev_scaler[2]{ MAX_AXIS / 2900.0, MAX_AXIS / 2800.0 };

		bool realtime_display_enabled{false};
		bool calibration_enabled{false};
		bool calibration_init{false};
		bool calibration_save{false};
		bool calibration_cancel{false};
		bool increment_stepper{false};
		bool decrement_stepper{false};
		void init_callbacks();

		VjoyFeeder vjoy_feeder;
		bool vjoy_available{ false };
		Axis vjoy{ 0,0,0,0,0,0,0,0 };
		// L/R rev, L/R Throttle, Spoiler, flaps
		Iir::Butterworth::LowPass<order> butterworth[6];
		int filter_evolution[2]{ 0,0 };
		bool filter_has_settled{ false };

		static constexpr int MAX_EVOLUTIONS = 100;

	public:

		static constexpr int REV_1 = 0;
		static constexpr int REV_2 = 1;
		static constexpr int ENG_1 = 2;
		static constexpr int ENG_2 = 3;
		static constexpr int SPBRK = 4;
		//static constexpr int FLAPS = 5;

		static constexpr int MAX_AXES = 6;

		double adc_threshold[MAX_AXES] = { 6, 6, 6, 6, 6};
		double adc_previous_filtered[MAX_AXES] = { 100, 100, 100, 100, 100 };
		int adc_raw[MAX_AXES] = { 0, 0, 0, 0, 0 };
		int adc_previous_raw[MAX_AXES] = { 0, 0, 0, 0, 0 };
		int spike_index[MAX_AXES] = { 0, 0, 0, 0, 0};
		const int MAX_SPIKES[6] = {4, 4, 2, 2, 2, 2};
		const int SPIKE_THRESHOLD[6] = { 100, 100, 100, 100, 100 };
		int spikes[4][MAX_AXES] = { { 0, 0, 0, 0, 0 } ,{ 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0 } ,{ 0, 0, 0, 0, 0 } };


		static std::atomic<double> adc_filtered[MAX_AXES];

		long adc_normalized[MAX_AXES] = { 0, 0, 0, 0, 32768 };
		double adc_null_zone[MAX_AXES] =     { 1050.0, 2210.0,  345.0, 270.0, 3260.0};
		double adc_min_null_zone[MAX_AXES] = {    0.0,    0.0,    0.0,   0.0, 3260.0};
		double adc_max_null_zone[MAX_AXES] = { 1050.0, 2210.0,  345.0, 270.0, 4095.0};

		int32_t adc_min[MAX_AXES] = { 0, 0, 0, 0, 0};
		double adc_max[MAX_AXES] = { 0, 0, 0, 0, 0};
		double adc_range[MAX_AXES] = { 1, 1, 1, 1, 1};


		static JoystickDataStore beagleBoneData;
		static bool beagleBoneFresh;
	};
}
