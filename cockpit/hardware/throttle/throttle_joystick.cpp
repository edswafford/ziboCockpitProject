#include <ctime>
#include <thread>
#include "throttle_joystick.hpp"
#include "../common/logger.hpp"
#include "Pokeys/stepper.hpp"
#include "throttle_hardware.hpp"
#include "../../sim_configuration.hpp"
#include "shared_types.hpp"

extern logger LOG;

namespace zcockpit::cockpit::hardware
{
	enum class Autothrottle_Modes : unsigned
	{
		BLANK = 0,		// Autothrottle Flight mode Annunicator is blank.  Servos are Inhibited
		ARM = 1,		// No Autothrottle mode is engaged.  Servos are Inhibited
		N1 = 2,			// Autothrottle maintains thrust at N1 limit
		MCD_SPD = 3,	// Autothrottle maintains speed by MCP
		FMC_SPD = 4,	// Autothrottle maintains speed by FMC
		GA = 5,			// Autothrottle maintains thrust at reduced go-around setting
		THR_HLD = 6,	// Autothrottle servos are Inhibited
		RETARD = 7,		// Autothrottle move levers to aft stop.  Retard mode is followed by ARM
		UNKNOWN = 8,
	};

	// STATIC
	JoystickDataStore ThrottleAndJoystick::beagleBoneData;
	bool ThrottleAndJoystick::beagleBoneFresh = false;


	sPoKeysDeviceStatus ThrottleAndJoystick::deviceStat_;
	sPoKeysDeviceStatus ThrottleAndJoystick::deviceStat_flaps;
	std::mutex pokeys_mutex;

	std::atomic<bool> stepper_test_running = false;
	std::atomic<bool> stepper_running = false;

	//std::atomic<double> ThrottleAndJoystick::throttle_filtered[2] = { 0,0 };
	//std::atomic<int> ThrottleAndJoystick::speed_brakes_filtered = 0;

	std::atomic<int> ThrottleAndJoystick::disengage_auto_throttle = 1;
	std::atomic<bool> ThrottleAndJoystick::auto_throttle_is_disengaged = false;

	std::atomic<bool> ThrottleAndJoystick::enable_at_steppers = false;

	std::atomic<double> ThrottleAndJoystick::adc_filtered[6] = { 0, 0, 0, 0, 0 };

	std::atomic<double> ThrottleAndJoystick::left_n1_commanded = 0.0;
	std::atomic<double> ThrottleAndJoystick::right_n1_commanded = 0.0;
	bool ThrottleAndJoystick::rev_deployed[2] = { false, false };
	const std::set<int> inputs{
		eng_1_idle_cutoff_sw,
		eng_2_idle_cutoff_sw,
		toga_sw,
		auto_throttle_sw,
		parking_brake_sw,
		horn_cutout_sw,
		stab_trim_man_cutout_sw,
		stab_trim_auto_cutout_sw,
	};

	const std::set<int> outputs{
		mux_s0,
		mux_s1,
		mux_s2,
		eng_1_stp,
		eng_1_dir,
		eng_1_MS1,
		eng_1_MS2,
		eng_1_MS3,
		eng_1_EN,
		eng_2_stp,
		eng_2_dir,
		eng_2_MS1,
		eng_2_MS2,
		eng_2_MS3,
		eng_2_EN,
		spoiler_stp,
		spoiler_dir,
		spoiler_MS1,
		spoiler_MS2,
		spoiler_MS3,
		spoiler_EN,
		parking_brake_light,
	};

	const std::set<int> analog_inputs{
		eng_1_reverser_analog_input,
		eng_2_reverser_analog_input,
		eng_1_thrust_analog_input,
		eng_2_thrust_analog_input,
		spoiler_analog_input,
		flaps_analog_input,
	};

	static constexpr auto IDLE_OFFSET = 5;
	static constexpr auto LEFT_REVERSER_FUDGE_FACTOR = 200;  // compensate to the large amount of play in the pots
	static constexpr auto RIGHT_REVERSER_FUDGE_FACTOR = 210;  // compensate to the large amount of play in the pots



	ThrottleAndJoystick::ThrottleAndJoystick(AircraftModel& ac_model, MainWindow* mw) : aircraft_model(ac_model), main_window(mw)
	{

		init_callbacks();
		initialize_switches();

		pokey_alive = init_pokeys();
		const auto joy_status = vjoy_feeder.init_vjoy(1);
		if (joy_status != Vjoy_State::Valid)
		{
			throw std::exception("Virtual Joystick Failure.  See log for details.");
		}
		vjoy_available = vjoy_feeder.aquire();
		if (!vjoy_available)
		{
			auto msg = "Virtual Joystick Failure.  Device " + std::to_string(vjoy_feeder.get_id()) + " may not be enabled.";
			throw std::exception(msg.c_str());
		}

		initialize_calibration();

		update_calibration_display();

		//throttle_filtered[0] = eng_min[0];
		//throttle_filtered[1] = eng_min[1];
		for (auto i = 0; i < 5; i++) {
			butterworth[i].setup(samplingrate, cutoff_frequency);
		}

		LOG() << "Starting Throttle Timer";
		// start timer thread
		timer_thread = std::thread([this]
		{
			this->timer();
		});

	}

	void ThrottleAndJoystick::drop()
	{
		if (stepper_ctrl.is_initialized())
		{
			stepper_ctrl.drop();
		}

		// now we can stop the timer
		std::unique_lock<std::mutex> lk(timer_done_mutex);
		{
			std::lock_guard<std::mutex> lock(throttle_timer_mutex);
			stop_timer = true;
			LOG() << "Request Throttle Timer to terminate";
		}
		condition.wait(lk, [this]
		{
			return (this->timer_has_stopped);
		});

		while (stepper_test_running)
		{
			LOG() << "Waiting on Stepper TEST task";
		}

		while (stepper_running)
		{
			LOG() << "Waiting on Stepper task";
		}
		if (stepper_thread.joinable())
		{
			stepper_thread.join();
		}
		if (timer_thread.joinable())
		{
			timer_thread.join();
		}
		if (pokey_alive)
		{
			DisconnectPoKeysDevice();
			TerminateWinsock(); //Clean up Winsock
		}
		LOG() << "Throttle terminated: Goodby Throttle.";
	}

	////////////////////////////////////////////////////////////////////////////////////////
	//
	//  Private
	//
	////////////////////////////////////////////////////////////////////////////////////////

	void ThrottleAndJoystick::timer()
	{
		stop_timer = false;
		timer_has_stopped = false;
		while (true)
		{
			// do we need to shut down
			{
				std::lock_guard<std::mutex> lock(throttle_timer_mutex);
				if (stop_timer)
				{
					LOG() << "Throttle timer is stopping";
					if (pokey_alive)
					{
						set_parking_brake_output(false);
					}
					break;
				}
			}
			if (pokey_alive)
			{
				try
				{
					{
						std::lock_guard<std::mutex> lock(pokeys_mutex);
						process_throttle();
					}



					if (realtime_display_enabled)
					{
						if (current_cycle % common::FIVE_HZ == 0)
						{
							update_realtime_display();
						}
					}
					if (calibration_enabled)
					{
						calibrate();
					}
					else
					{
						ThrottleAndJoystick::calibration_init = false;
					}
					if (calibration_save)
					{
						save_calibration();
					}
					else if (calibration_cancel)
					{
						// restore display with old calibration values
						update_calibration_display();
						ThrottleAndJoystick::calibration_cancel = false;
						calibration_save = false;
					}


					//
					// Testing
					//
					if (increment_stepper)
					{
						if (!stepper_test_running)
						{
							stepper_thread_for_testing = std::thread([this]
							{
								//this->stepper_ctrl->step_forward(SPDBRK, 200);
								if (stepper_ctrl.is_initialized())
								{
									stepper_ctrl.step_forward(LEFT_THROTTLE, 200);
								}
								//this->stepper_ctrl->step_reverse(RIGHT_THROTTLE, 200);

							});
							stepper_thread_for_testing.detach();
							ThrottleAndJoystick::increment_stepper = false;
						}
						else
						{
							LOG() << "Stepper running";
						}
					}
					else if (decrement_stepper)
					{
						if (!stepper_test_running)
						{
							stepper_thread_for_testing = std::thread([this]
							{
								//this->stepper_ctrl->step_reverse(SPDBRK, 200);
								if (stepper_ctrl.is_initialized())
								{
									stepper_ctrl.step_reverse(LEFT_THROTTLE, 200);
								}
								//this->stepper_ctrl->step_forward(RIGHT_THROTTLE, 200);

							});
							stepper_thread_for_testing.detach();
							ThrottleAndJoystick::decrement_stepper = false;
						}
						else
						{
							LOG() << "Stepper running";
						}
					}
				}
				catch (std::exception& e)
				{
					pokey_alive = false;
					LOG() << "Throttle Exception: " << e.what();
				}
			}
			else
			{
				//
				// Pokeys is NOT Running
				//

				throttle_healthy = false;

				if (current_cycle % common::ONE_SECOND)
				{
					pokey_alive = init_pokeys();
					if (pokey_alive)
					{
						throttle_healthy = true;
					}
				}
			}

			if (!stepper_thread.joinable() && stepper_ctrl.is_initialized())
			{
				LOG() << "Starting Stepper Timer";
				stepper_thread = std::thread(&Step_Controller::timer);
				//std::thread([this]
				//{
				//	this->stepper_ctrl.timer();
				//});
			}
			//
			// 1Hz task
			if (current_cycle >= common::ONE_SECOND)
			{
				update_throttle_health();
				sync_switches = true;
				sync_flaps = true;
			}

			//
			// sleep
			//
			// update rate is 40Hz
			std::this_thread::sleep_for(std::chrono::milliseconds(common::HZ_40));

			// update counters
			current_cycle = (current_cycle >= common::ONE_SECOND) ? 0 : current_cycle + 1;
		}

		// timer  has stopped
		LOG() << "Throttle timer has stopped";

		std::unique_lock<std::mutex> lk(timer_done_mutex);
		timer_has_stopped = true;
		lk.unlock();
		condition.notify_one();
		LOG() << "Throttle timer returning";
	}


	//
	// BeagleBone Joystick
	//
	void ThrottleAndJoystick::updateBeagleBonejoystick(JoystickDataStore & data)
	{
		ThrottleAndJoystick::beagleBoneData = data;
		ThrottleAndJoystick::beagleBoneFresh = true;
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// Process Throttle Inputs and Parking Brake output
	//
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	void ThrottleAndJoystick::process_throttle()
	{
		static unsigned flapSensors[4] = { 0,0,0,0 };
		static unsigned previous_flaps = 0;
		static unsigned flap_steady_state_count = 0;
		static int muxIndex = 0;
		static bool cycle_20hz = true;

		if (pokey_alive)
		{
			//
			// FLAPS
			//
			process_analogs();
			if (cycle_20hz)
			{
				if (GetDigitalAnalogInputs(&deviceStat_flaps, flaps_analog_input) == -1)
			{
				throw std::exception("PoKeys failed to read flaps.");
			}
				flapSensors[muxIndex] = deviceStat_flaps.Pins[flaps_analog_input].AnalogValue > 3700 ? 1 : 0;
			muxIndex++;
			if (muxIndex > 3)
			{
				muxIndex = 0;
				const unsigned flaps = flapSensors[0] + (flapSensors[1] << 1) + (flapSensors[2] << 2) + (flapSensors[3] << 3);
				if (flaps == previous_flaps)
				{
					if (flap_steady_state_count >= 3)
					{
						flap_steady_state_count = 3;
						flaps_xplane_update(flaps);
					}
					else
					{
						flap_steady_state_count += 1;
					}
				}
				else
				{
					flap_steady_state_count = 0;
				}
				previous_flaps = flaps;
			}

			// Select which MUX input to read
			deviceStat_.Pins[mux_s0].DigitalOutputValue = (muxIndex & 1);
			deviceStat_.Pins[mux_s1].DigitalOutputValue = (muxIndex >> 1) & 1;
			deviceStat_.Pins[mux_s2].DigitalOutputValue = LOW;

			//
			// PARKING BRAKE
			//
			set_parking_brake_output(parking_brake == 0);

			//
			// Process Switches
			//
			if (GetDigitalInputsStatus(&deviceStat_) == -1) // reads the current inputs
			{
				throw std::exception("PoKeys failed to read digital Inputs.");
			}
			process_switches();

				//
				// update Virtual Joystick
				//
				if(beagleBoneFresh)
				{
					// Aileron
					vjoy.axisR = ThrottleAndJoystick::beagleBoneData.adc[1];
					// Elevator
					vjoy.axisS = ThrottleAndJoystick::beagleBoneData.adc[0];
					//Rudder
					vjoy.axisT = ThrottleAndJoystick::beagleBoneData.adc[3];
					// left brake capt vs fo
					if (ThrottleAndJoystick::beagleBoneData.adc[2] > 0) {
						vjoy.axisU = ThrottleAndJoystick::beagleBoneData.adc[2];
					}
					else if (ThrottleAndJoystick::beagleBoneData.adc[6] > 100) {
						vjoy.axisU = ThrottleAndJoystick::beagleBoneData.adc[5];
					}
					else {
						vjoy.axisU = 0;
					}
					// right brake capt vs fo
					if (ThrottleAndJoystick::beagleBoneData.adc[5] > 0) {
						vjoy.axisV = ThrottleAndJoystick::beagleBoneData.adc[5];
					}
					else if (ThrottleAndJoystick::beagleBoneData.adc[4] > 100) {
						vjoy.axisV = ThrottleAndJoystick::beagleBoneData.adc[4];
					}
					else {
						vjoy.axisV = 0;
					}

					if (ThrottleAndJoystick::beagleBoneData.ap_disconnect)
					{
//						aircraft_model.push_switch_change(throttle_zcockpit_switches[ThrottleSwitchPosition::ap_disconnect]);
					}	

					// TRIM UP
					if (ThrottleAndJoystick::beagleBoneData.trim_up) {
						aircraft_model.push_switch_change(throttle_zcockpit_switches[ThrottleSwitchPosition::pitch_trim_up_pb]);
					}

					// TRIM DOWN
					if (ThrottleAndJoystick::beagleBoneData.trim_dn) {
						aircraft_model.push_switch_change(throttle_zcockpit_switches[ThrottleSwitchPosition::pitch_trim_down_pb]);
					}

					vjoy_available = vjoy_feeder.update(vjoy, ThrottleAndJoystick::beagleBoneData.lButtons);
					ThrottleAndJoystick::beagleBoneFresh = false;
				}
				else {
					vjoy_available = vjoy_feeder.update(vjoy);
				}

				{
				//
				// 0:blank;  1:ARM;  2:N1;  3:MCP SPD;  4:FMC SPD;  5:GA;  6:THR HLD;  7:RETARD
				//
				auto at_mode = Autothrottle_Modes::UNKNOWN;
				enable_at_steppers = false;
				static auto previous_enable_at_steppers = false;

				if (aircraft_model.z738_is_available())
				{
					if (at_mode != static_cast<Autothrottle_Modes>(aircraft_model.z737InData.pfd_spd_mode))
					{
						at_mode = static_cast<Autothrottle_Modes>(aircraft_model.z737InData.pfd_spd_mode);

						enable_at_steppers = !(at_mode == Autothrottle_Modes::ARM || at_mode == Autothrottle_Modes::THR_HLD || at_mode ==
							Autothrottle_Modes::BLANK || at_mode == Autothrottle_Modes::UNKNOWN);
					}

					if (ThrottleAndJoystick::disengage_auto_throttle == 0)
					{
						enable_at_steppers = false;
						previous_enable_at_steppers = false;
						LOG() << "Stepper Disengage auto throttle";
					}


					if (aircraft_model.z737InData.autothrottle_arm_pos && enable_at_steppers)
					{
						previous_enable_at_steppers = enable_at_steppers;
						auto scaler = (100 - aircraft_model.z737InData.throttle_ratio[0]) / 100 * 0.27;
						left_n1_commanded = 20.6 + (aircraft_model.z737InData.throttle_ratio[0] * (0.93 + scaler));
						if (left_n1_commanded < 0.0)
						{
							left_n1_commanded = 0.0;
						}

						scaler = (100 - aircraft_model.z737InData.throttle_ratio[1]) / 100 * 0.27;
						right_n1_commanded = 20.6 + (aircraft_model.z737InData.throttle_ratio[1] * (0.93 + scaler));
						if (right_n1_commanded < 0.0)
						{
							right_n1_commanded = 0.0;
						}
					}
					else if (previous_enable_at_steppers && !auto_throttle_is_disengaged)
					{
						// don't turn off steppers until they have caught up with the last commanded value
						// Determine % throttle
							const auto left_throttle_precent = 100.0 * (static_cast<double>(adc_normalized[ENG_1] + IDLE_OFFSET)/32768.0 );
						const auto left_delta_value = left_n1_commanded - left_throttle_precent;

							const auto right_throttle_precent = 100.0 *  (static_cast<double>(adc_normalized[ENG_2] + IDLE_OFFSET) / 32768.0);
						const auto right_delta_value = right_n1_commanded - right_throttle_precent;
						if (abs(left_delta_value) > 2.0 || abs(right_delta_value) > 2.0)
						{
							enable_at_steppers = true;
						}
						else
						{
							previous_enable_at_steppers = enable_at_steppers;
						}
					}
				}
			}


			}
			cycle_20hz != cycle_20hz;
		}
	}


	void ThrottleAndJoystick::flaps_xplane_update(const unsigned flaps)
	{
		switch (flaps)
		{
		case Flap_0:
			if (previous_flaps != Flap_0 || sync_flaps)
			{
				aircraft_model.push_switch_change(throttle_zcockpit_switches[ThrottleSwitchPosition::flaps_0]);
				previous_flaps = Flap_0;
			}
			break;
		case Flap_1:
			if (previous_flaps != Flap_1 || sync_flaps)
			{
				aircraft_model.push_switch_change(throttle_zcockpit_switches[ThrottleSwitchPosition::flaps_1]);
				previous_flaps = Flap_1;
			}
			break;
		case Flap_2:
			if (previous_flaps != Flap_2 || sync_flaps)
			{
				aircraft_model.push_switch_change(throttle_zcockpit_switches[ThrottleSwitchPosition::flaps_2]);
				previous_flaps = Flap_2;
			}
			break;
		case Flap_5:
			if (previous_flaps != Flap_5 || sync_flaps)
			{
				aircraft_model.push_switch_change(throttle_zcockpit_switches[ThrottleSwitchPosition::flaps_5]);
				previous_flaps = Flap_5;
			}
			break;
		case Flap_10:
			if (previous_flaps != Flap_10 || sync_flaps)
			{
				aircraft_model.push_switch_change(throttle_zcockpit_switches[ThrottleSwitchPosition::flaps_10]);
				previous_flaps = Flap_10;
			}
			break;
		case Flap_15:
			if (previous_flaps != Flap_15 || sync_flaps)
			{
				aircraft_model.push_switch_change(throttle_zcockpit_switches[ThrottleSwitchPosition::flaps_15]);
				previous_flaps = Flap_15;
			}
			break;
		case Flap_25:
			if (previous_flaps != Flap_25 || sync_flaps)
			{
				aircraft_model.push_switch_change(throttle_zcockpit_switches[ThrottleSwitchPosition::flaps_25]);
				previous_flaps = Flap_25;
			}
			break;
		case Flap_30:
			if (previous_flaps != Flap_30 || sync_flaps)
			{
				aircraft_model.push_switch_change(throttle_zcockpit_switches[ThrottleSwitchPosition::flaps_30]);
				previous_flaps = Flap_30;
			}
			break;
		case Flap_40:
			if (previous_flaps != Flap_40 || sync_flaps)
			{
				aircraft_model.push_switch_change(throttle_zcockpit_switches[ThrottleSwitchPosition::flaps_40]);
				previous_flaps = Flap_40;
			}
			break;
		default:;
		}
		sync_flaps = false;
	}

	void ThrottleAndJoystick::set_parking_brake_output(const bool on)
	{
		if (on)
		{
			// ON
			deviceStat_.Pins[parking_brake_light].DigitalOutputValue = HIGH;
		}
		else
		{
			// OFF
			deviceStat_.Pins[parking_brake_light].DigitalOutputValue = LOW;
		}

		if (SetDigitalOutputs(&deviceStat_) == -1) // Sets the outputs and reads the inputs
		{
			throw std::exception("PoKeys failed to SetDigitalOutputs.");
		}
	}

	//map(value, fromLow, fromHigh, toLow, toHigh)
	//long map_analog(long x, long in_min, long in_max, long out_min, long out_max)
	//{
	//	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
	//}



	extern void simconnect_send_spoiler_position(const double position);

	void ThrottleAndJoystick::process_analogs()
	{

		if (GetAllAnalogInputs(adc_raw, eng_1_reverser_analog_input) == -1)
		{
			LOG() << "PoKeys failed to read Analogs" ;
			throw std::exception("PoKeys failed to read Analogs.");
		}
		//
		// Spike Filter
		//
		for (int i = 0; i < MAX_AXES; i++) {
			if (abs(adc_raw[i] - adc_previous_raw[i]) > SPIKE_THRESHOLD[i]) {
				//LOG() << "REV Spike  i == " << i << " adc raw " << adc_raw[i] << " count " << spike_index[i];
				if (spike_index[i] < MAX_SPIKES[i]) {
					spikes[spike_index[i]][i] = adc_raw[i];
					spike_index[i] += 1;
					adc_raw[i] = adc_previous_raw[i];
				}
				else {
					if (i == 0 || i == 1) {
						LOG() << "MAX REV Spike  i == " << i << " adc raw " << adc_raw[i];
					}
					spike_index[i] = 0;
					int sum = 0;
					for (int j = 0; j < MAX_SPIKES[i]; j++) {
						sum += spikes[j][i];
					}
					spike_index[i] = 0;
					adc_raw[i] = sum / MAX_SPIKES[i];
					adc_previous_raw[i] = adc_raw[i];
				}
			}
			else {
				spike_index[i] = 0;
				adc_previous_raw[i] = adc_raw[i];
			}
		}
	/*
		char buffer[1024];
	#pragma warning(disable:4996)
		sprintf(buffer, "Analog [0] %6d [1] %6d [2] %6d [3] %6d [4] %6d [5] %6d",
			adc_raw[REV_1], adc_raw[REV_2], adc_raw[ENG_1],
			adc_raw[ENG_2], adc_raw[SPBRK], adc_raw[FLAPS]);
		LOG() << buffer;
	
	#pragma warning(enable:4996)
	*/
	
			process_reverser(eng_1_reverser_analog_input, REV_1);
			process_reverser(eng_2_reverser_analog_input, REV_2);
	
			filter_analogs(eng_1_thrust_analog_input, ENG_1);
			if (rev_deployed[0])
			{
				vjoy.axisX = 0.0;
			}
			else if (!enable_at_steppers)
			{
				vjoy.axisX = adc_normalized[ENG_1];
			}
	
			filter_analogs(eng_2_thrust_analog_input, ENG_2);
			if (rev_deployed[1])
			{
				vjoy.axisX = 0.0;
			}
			else if (!enable_at_steppers)
			{
				vjoy.axisY = adc_normalized[ENG_2];
			}
	
			filter_analogs(spoiler_analog_input, SPBRK);
			if (filter_has_settled) {
				double spdbrk = (-adc_normalized[SPBRK] + 32768.0);
				auto current_speed_brake = static_cast<int>(spdbrk / 32768.0 * 208);
				if (std::abs(current_speed_brake - speed_brake) > 5){
					speed_brake = current_speed_brake;
					auto sw = throttle_zcockpit_switches[ThrottleSwitchPosition::speedbrake_lever];

					//
					// Spoiler handle position [0.0: Retracted,  0.089: Armed, 0.67: Flight Detent  1.0: Fully Extended] 
					if (speed_brake < 20) {
						sw.float_hw_value = 0.0f;
					}
					else if(speed_brake < 30) {
						sw.float_hw_value = 0.089f;
					}
					else if(speed_brake < 30) {
						sw.float_hw_value = 0.089f;
					}
					else {
						sw.float_hw_value = spdbrk / 32768.0;
					}
					aircraft_model.push_switch_change(sw);
				}
			}
	}


	void ThrottleAndJoystick::filter_analogs(const int analog_pin, int index) {

		if (adc_filtered[index] < 0.0) {
			adc_filtered[index] = 00.;
		}
		else if (adc_filtered[index] > 4095.0) {
			adc_filtered[index] = 4095.0;
		}

		adc_filtered[index] = butterworth[index].filter(adc_raw[index]);
		//if (index == 1) {
		//	LOG() << "adc_raw = " << adc_raw[1] << " filtered = " << adc_filtered[1];
		//}
		
		//
		// Null Zone
		//
		if (adc_filtered[index] >= adc_min_null_zone[index] && adc_filtered[index] <= adc_max_null_zone[index]) {
			adc_filtered[index] = adc_null_zone[index];
		}
		if (std::abs(adc_filtered[index] - adc_previous_filtered[index]) > adc_threshold[index]) {
			adc_previous_filtered[index] = adc_filtered[index];
			//if (index == 4) {
			//	LOG() << "SpeedBrake Normalize filtered " << adc_filtered[index] << " min " << adc_min[index] <<
			//		" range " << adc_range[index];
			//}
				//
				// Normalize as required
				//
				double normal = static_cast<double>(adc_filtered[index] - adc_min[index]) / adc_range[index] * 32768;
				if (normal < 0) {
					normal = 0;
				}
				else if (normal > 32768.0) {
					normal = 32768.0;
				}
				adc_normalized[index] = static_cast<int32_t>((normal));
		}
		else {
			adc_filtered[index] = adc_previous_filtered[index];
		}
	}


	void ThrottleAndJoystick::send_reverser_value(const int side, const double value) const
	{
		//
		//Pro only. Val:-16383~16383,from Full_Rev to Full_Power
		//
		if (value <= 0.0) {
			const int reverser = static_cast<int>(value * -16383.0/ REVRSER_FULLSCALE);


			if (side == LEFT) {
				auto sw = throttle_zcockpit_switches[ThrottleSwitchPosition::reverse_lever1];
				sw.float_hw_value = reverser;
				aircraft_model.push_switch_change(sw);
			}
			else
			{
				auto sw = throttle_zcockpit_switches[ThrottleSwitchPosition::reverse_lever2];
				sw.float_hw_value = reverser;
				aircraft_model.push_switch_change(sw);
			}

		}
	}

	//
	//  Revised Reverser Processing
	//
	void ThrottleAndJoystick::process_reverser(const int analog_pin, const int side)
	{
		const auto DELAY_CLICK = 200; // milliseconds delay


		filter_analogs(analog_pin, side);
		
		//
		// Give the filter time to settle
		if (filter_evolution[side] < MAX_EVOLUTIONS) {
			filter_evolution[side] += 1;
			return;
		}
		filter_has_settled = true;

		if (rev_first_pass[side])
		{
			rev_clicks[side] = 0;
			rev_first_pass[side] = false;
			previous_rev[side] = rev_min[side];
			expected_rev_nozzle_position[side] = 0.0;
			previous_rev_nozzle_position[side] = 0.0;
			previous_rev_deployed[side] = false;
			rev_waiting_for_click_response[side] = false;
		}

		if (aircraft_model.z738_is_available()) 
		{
			// The code will step once per cycle at 20 Hz.  It is possible we could out run xplane
			// So we should step then wait for the response before steping again -- unless we time out then will try again
			//
			//
			double thrust_and_reverser_lever = 0.0;
			float* reverser = nullptr;
			if(side == LEFT) 
			{
				if(reverser = static_cast<float*>(aircraft_model.get_z_cockpit_switch_data(DataRefName::reverse_lever1))) 
				{
					thrust_and_reverser_lever = *reverser;
				}
			}
			else
			{
				if(reverser = static_cast<float*>(aircraft_model.get_z_cockpit_switch_data(DataRefName::reverse_lever2))) 
				{
					thrust_and_reverser_lever = *reverser;
				}
			}


			// Check if PMDG Thrust Lever slightly above idle
			if (thrust_and_reverser_lever > 0)
			{
				pmdg_thrust_lever_above_idle[side] = true;
				thrust_and_reverser_lever = 0.0;
			}
			else
			{  // Thurst lever is at idle --> conver to positive value (easier to reason about)
				pmdg_thrust_lever_above_idle[side] = false;
				thrust_and_reverser_lever = thrust_and_reverser_lever;
			}

			//
			// Normalize joystick value
			double rev = adc_filtered[side] - adc_min[side];
			if (rev < 100.0)
			{
				rev = 0.0;
			}
			//	LOG() << "Reverser side " << side << " adc value " << rev;

			rev = rev / adc_range[side] * REVRSER_FULLSCALE;

			if (rev > 0.0)
			{
				rev = 0.0;
			}
			else if (rev < REVRSER_FULLSCALE)
			{
				rev = REVRSER_FULLSCALE;
			}

			if (rev < 0.0)
			{
				rev_deployed[side] = true;
			}
			else
			{
				rev_deployed[side] = false;
			}

			// Deployed
			//
			if (previous_rev_deployed[side] == false && rev_deployed[side])
			{
				// Starting Reverser
				//

				// Make sure throttle is at idle -- otherwise reverser will not deploy
				//LOG() << "CUT-THROTTLE PMDG  Send First Click -- then wait  Side " << side << " REV = " << rev << " filtered " << adc_filtered[side];
				send_reverser_value(side, -0.001 * 100);

				// first click lifts reverser, but doesn't change N1
				//simconnect_send_event_data(thrust_lever_event, MOUSE_FLAG_LEFTSINGLE);

				expected_rev_nozzle_position[side] = -0.001;
				rev_waiting_for_click_response[side] = true;
				previous_rev_deployed[side] = rev_deployed[side];
			}
			//
			// Closed
			else if (previous_rev_deployed[side] == true && rev_deployed[side] == false)
			{
				//  Reverser Closed
				//
				//LOG() << "Reverser Closed: Send throttle 0.0";
				send_reverser_value(side, 0.0);
				expected_rev_nozzle_position[side] = 0.0;
				previous_rev_deployed[side] = rev_deployed[side];
			}
			else 
			{
				if (!rev_waiting_for_click_response[side]) 
				{
					if (rev < thrust_and_reverser_lever)
					{
						// we need to increse Reverser
						expected_rev_nozzle_position[side] = rev;
						if (expected_rev_nozzle_position[side] < REVRSER_FULLSCALE) {
							expected_rev_nozzle_position[side] = REVRSER_FULLSCALE;
						}
						start[side] = std::chrono::high_resolution_clock::now();
						rev_waiting_for_click_response[side] = true;
						//simconnect_send_event_data(thrust_lever_event, MOUSE_FLAG_LEFTSINGLE);
						send_reverser_value(side, expected_rev_nozzle_position[side] * 100);
						//LOG() << "Increase REV_THRUST_LEVER  " << thrust_and_reverser_lever << " expected " << expected_rev_nozzle_position[side];
					}
					else if (rev > thrust_and_reverser_lever)
					{
						// we need to decrease Reverser
						expected_rev_nozzle_position[side] = rev;
						if (expected_rev_nozzle_position[side] > 0.0) {
							expected_rev_nozzle_position[side] = 0.0;
						}
						start[side] = std::chrono::high_resolution_clock::now();
						rev_waiting_for_click_response[side] = true;
						//simconnect_send_event_data(thrust_lever_event, MOUSE_FLAG_RIGHTSINGLE);
						send_reverser_value(side, expected_rev_nozzle_position[side] * 100);
						//LOG() << "Decrease REV_THRUST_LEVER  " << thrust_and_reverser_lever << " expected " << expected_rev_nozzle_position[side];

					}
					else
					{
						// Or no changes needed
						//expected_rev_nozzle_position[side] = thrust_and_reverser_lever;
						rev_waiting_for_click_response[side] = false;
						//LOG() << "lever " << thrust_and_reverser_lever << " == expected " << expected_rev_nozzle_position[side];
					}

				}
				else 
				{
					// has PMDG responded -- lever == expecte lever posioion
					if (abs(thrust_and_reverser_lever - expected_rev_nozzle_position[side]) < 0.01)
					{
						rev_waiting_for_click_response[side] = false;
						//LOG() << "Responded lever == " << thrust_and_reverser_lever;
					}
					// Ohterwise check for timeout -- 
					else
					{
						auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start[side]);
						if (elapsed.count() > DELAY_CLICK)
						{
							rev_waiting_for_click_response[side] = false;
							LOG() << "REV timeout";
						}
					}
				}
			}
		}
	}



	void ThrottleAndJoystick::process_switches()
	{
		//LOG() << "disemgaged == " << Throttle::auto_throttle_is_disengaged;

		if (auto_throttle_toggle)
		{
			// Disengauge was press -- waiting for light to toggle off
			if (aircraft_model.z738_is_available() && aircraft_model.z737InData.autothrottle_arm_pos)
			{
				// light is now off
				auto_throttle_toggle = false;
				LOG() << "AT Light toggled off";
			}
		}
		else
		{
			if (aircraft_model.z738_is_available() && aircraft_model.z737InData.autothrottle_arm_pos)
			{
				// light is on --> AT is not disengaged
				ThrottleAndJoystick::auto_throttle_is_disengaged = false;
			}
			else
			{
				ThrottleAndJoystick::auto_throttle_is_disengaged = true;
			}
		}
		for (auto i : inputs)
		{
			const auto value = deviceStat_.Pins[i].DigitalInputValue;
			switch (i)
			{
			case eng_1_idle_cutoff_sw:
				if (eng_1_idle_cutoff != value || sync_switches)
				{
					eng_1_idle_cutoff = value;
					if (value == 1)
					{
						aircraft_model.push_switch_change(throttle_zcockpit_switches[ThrottleSwitchPosition::mixture_ratio1_idle]);
					}
					else
					{
						aircraft_model.push_switch_change(throttle_zcockpit_switches[ThrottleSwitchPosition::mixture_ratio1_cutoff]);
					}
				}
				break;
			case eng_2_idle_cutoff_sw:
				if (eng_2_idle_cutoff != value || sync_switches)
				{
					eng_2_idle_cutoff = value;
					if (value == 1)
					{
						aircraft_model.push_switch_change(throttle_zcockpit_switches[ThrottleSwitchPosition::mixture_ratio2_idle]);
					}
					else
					{
						aircraft_model.push_switch_change(throttle_zcockpit_switches[ThrottleSwitchPosition::mixture_ratio2_cutoff]);
					}
				}
				break;
			case toga_sw:
				if (toga != value)
				{
					toga = value;
					if (value == 0)
					{
						aircraft_model.push_switch_change(throttle_zcockpit_switches[ThrottleSwitchPosition::left_toga_pos]);
					}
				}
				break;
			case auto_throttle_sw:
				if (disengage_auto_throttle != value)
				{
					disengage_auto_throttle = value;
					if (value == 0)
					{
						ThrottleAndJoystick::auto_throttle_is_disengaged = true;
						left_n1_commanded = right_n1_commanded = 0.0;
						if (aircraft_model.z738_is_available() && aircraft_model.z737InData.autothrottle_arm_pos)
						{
							// light is on need to wait for toggle
							auto_throttle_toggle = true;
							LOG() << "AUTO_THROTTLE Toggle == true";
						}
						LOG() << "AUTO THROTTLE  process switches Disengage auto throttle";
						aircraft_model.push_switch_change(throttle_zcockpit_switches[ThrottleSwitchPosition::left_at_dis_press_pb]);
					}
				}
				break;
			case parking_brake_sw:
				if (parking_brake != value || sync_switches)
				{
					parking_brake = value;
					if(parking_brake > 0) {
						parking_brake = 1;
					}
					auto sw = throttle_zcockpit_switches[ThrottleSwitchPosition::parking_brake_pos];
					sw.float_hw_value = parking_brake;
					aircraft_model.push_switch_change(sw);
				}
				break;
			case horn_cutout_sw:
				if (horn_cutout != value)
				{
					horn_cutout = value;
					if (value == 0)
					{
						aircraft_model.push_switch_change(throttle_zcockpit_switches[ThrottleSwitchPosition::gear_horn_cutout_pos]);
					}
				}
				break;
			case stab_trim_man_cutout_sw:
				if (stab_trim_man_cutout != value || sync_switches)
				{
					stab_trim_man_cutout = value;
					if (value == 0)
					{
						aircraft_model.push_switch_change(throttle_zcockpit_switches[ThrottleSwitchPosition::el_trim_pos_normal]);
					}
					else
					{
						// open guard
						aircraft_model.push_switch_change(throttle_zcockpit_switches[ThrottleSwitchPosition::el_trim_pos_cutoff]);
					}
				}
				break;
			case stab_trim_auto_cutout_sw:
				if (stab_trim_auto_cutout != value || sync_switches)
				{
					stab_trim_auto_cutout = value;
					if (value == 0)
					{
						aircraft_model.push_switch_change(throttle_zcockpit_switches[ThrottleSwitchPosition::ap_trim_pos_normal]);
					}
					else
					{
						// open guard
						
						aircraft_model.push_switch_change(throttle_zcockpit_switches[ThrottleSwitchPosition::ap_trim_pos_cutoff]);
					}
				}
				break;
			default:;
			}
		}
		sync_switches = false;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// Initialize Pokeys
	//
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool ThrottleAndJoystick::init_pokeys()
	{
		LOG() << "Pokeys: Starting Initialization";

		// Initialize Winsock
		if (InitWinsock() != 0)
		{
			LOG() << "Throttle: Failed to initialize Winsock for Pokeys.";
			return false;
		};

		// Enumerate PoKeys devices
		const int numDevices = EnumeratePoKeysDevices(PoKeysDevices);

		if (numDevices == 0)
		{
			LOG() << "Throttle: No PoKeys devices found!";
			TerminateWinsock();
			return false;
		}

		// Connect to first PoKeys device
		if (ConnectToPoKeysDevice(&PoKeysDevices[0]) != 0)
		{
			LOG() << "Throttle: Could not connect to PoKeys board!\n";
			TerminateWinsock();
			return false;
		}

		// Fill the device info structure
		GetDeviceData(&deviceStat_);

		// Initialize Pins
		for (auto i = 0; i < 55; i++)
		{
			// Clear all digital outputs
			deviceStat_.Pins[i].DigitalOutputValue = 0;

			if (inputs.find(i) != inputs.end())
			{
				deviceStat_.Pins[i].PinFunction = pinFunctionDigitalInput;
			}
			else if (outputs.find(i) != outputs.end())
			{
				deviceStat_.Pins[i].PinFunction = pinFunctionDigitalOutput;
			}
			else if (analog_inputs.find(i) != analog_inputs.end())
			{
				deviceStat_.Pins[i].PinFunction = pinFunctionAnalogInput;
			}
			else
			{
				deviceStat_.Pins[i].PinFunction = pinFunctionInactive;
			}
		}

		// Set pin directions (this must be called on every device startup since outputs are not activated before!)
		SetPinFunctions(&deviceStat_);

		// Set the outputs
		deviceStat_.Pins[mux_s0].DigitalOutputValue = HIGH;
		deviceStat_.Pins[mux_s1].DigitalOutputValue = HIGH;
		deviceStat_.Pins[mux_s2].DigitalOutputValue = LOW;

		//
		// Set up Stepper Motors
		//
		left_throttle = std::make_unique<Stepper>(eng_1_stp, eng_1_dir, eng_1_EN,
			eng_1_thrust_analog_input, IDLE_OFFSET, eng_min[LEFT], eng_max[LEFT], eng_min[LEFT] + 100, eng_max[LEFT] - 100);

		right_throttle = std::make_unique<Stepper>(eng_2_stp, eng_2_dir, eng_2_EN,
			eng_2_thrust_analog_input, IDLE_OFFSET, eng_min[RIGHT], eng_max[RIGHT], eng_min[RIGHT] + 100, eng_max[RIGHT] - 100);

		spoiler = std::make_unique<Stepper>(spoiler_stp, spoiler_dir, spoiler_EN,
			spoiler_analog_input, 0, spdbrk_min, spdbrk_max, spdbrk_min + 500, spdbrk_max - 400);
		stepper_ctrl.initialize(left_throttle.get(), right_throttle.get(), spoiler.get());

		deviceStat_.Pins[left_throttle->en_].DigitalOutputValue = LOW;
		deviceStat_.Pins[right_throttle->en_].DigitalOutputValue = LOW;
		deviceStat_.Pins[spoiler->en_].DigitalOutputValue = LOW;

		SetDigitalOutputs(&deviceStat_); // Sets the outputs and reads the inputs

		sync_switches = true;
		sync_flaps = true;

		LOG() << "Pokeys: Initialization Successful";

		return true;
	}



	void ThrottleAndJoystick::update_throttle_health()
	{
		if (pokey_alive)
		{
			throttle_healthy = true;
		}
		else
		{
			throttle_healthy = false;
		}
	}

	void ThrottleAndJoystick::update_realtime_display() const
	{
		if (!ThrottleAndJoystick::calibration_enabled) {
			update_eng1_text(adc_normalized[ENG_1]);
			update_eng2_text(adc_normalized[ENG_2]);
			update_spdbrk_text(adc_normalized[SPBRK]);
			update_rev1_text(adc_normalized[REV_1]);
			update_rev2_text(adc_normalized[REV_2]);
		}
		else {
			update_eng1_text(adc_filtered[ENG_1]);
			update_eng2_text(adc_filtered[ENG_2]);
			update_spdbrk_text(adc_filtered[SPBRK]);
			update_rev1_text(adc_filtered[REV_1]);
			update_rev2_text(adc_filtered[REV_2]);
		}
		/*
		update_eng1_text(adc_raw[ENG_1]);
		update_eng2_text(adc_raw[ENG_2]);
		update_spdbrk_text(adc_raw[SPBRK]);
		update_rev1_text(adc_raw[REV_1]);
		update_rev2_text(adc_raw[REV_2]);
		*/
	}
	void ThrottleAndJoystick::update_calibration_display() const
	{
		update_eng1_min_text(eng_min[LEFT]);
		update_eng2_min_text(eng_min[RIGHT]);
		update_spdbrk_min_text(spdbrk_min);
		update_rev1_min_text(rev_min[LEFT]);
		update_rev2_min_text(rev_min[RIGHT]);

		update_eng1_max_text(eng_max[LEFT]);
		update_eng2_max_text(eng_max[RIGHT]);
		update_spdbrk_max_text(spdbrk_max);
		update_rev1_max_text(rev_max[LEFT]);
		update_rev2_max_text(rev_max[RIGHT]);
	}

	void ThrottleAndJoystick::initialize_calibration()
	{
		eng_min[LEFT] = CockpitCfg::ptr->eng1_min;
		eng_min[RIGHT] = CockpitCfg::ptr->eng2_min;
		spdbrk_min = CockpitCfg::ptr->spdbrk_min;
		rev_min[LEFT] = CockpitCfg::ptr->rev1_min;
		rev_min[RIGHT] = CockpitCfg::ptr->rev2_min;
	
		eng_idle[LEFT] = eng_min[LEFT] + 100;
		eng_idle[RIGHT] = eng_min[RIGHT] + 100;
	
	
		eng_max[LEFT] = CockpitCfg::ptr->eng1_max;
		eng_max[RIGHT] = CockpitCfg::ptr->eng2_max;
		spdbrk_max = CockpitCfg::ptr->spdbrk_max;
		rev_max[LEFT] = CockpitCfg::ptr->rev1_max;
		rev_max[RIGHT] = CockpitCfg::ptr->rev2_max;
	
		rev_range[LEFT] = rev_max[LEFT] - rev_min[LEFT];
		rev_range[RIGHT] = rev_max[RIGHT] - rev_min[RIGHT];
	//	eng_scaler[LEFT] = MAX_THROTTLE / (eng_max[LEFT] - eng_min[LEFT]);
	//	eng_scaler[RIGHT] = MAX_THROTTLE / (eng_max[RIGHT] - eng_min[RIGHT]);
	//	spdbrk_scaler = MAX_SPD_BRAKE / (spdbrk_max - spdbrk_min);
	//	rev_scaler[LEFT] = (MAX_REVERSER) / (rev_max[LEFT] - rev_min[LEFT]);
	//	rev_scaler[RIGHT] = (MAX_REVERSER) / (rev_max[RIGHT] - rev_min[RIGHT]);
	//	eng_range[LEFT] = eng_max[LEFT] - eng_min[LEFT];
	//	eng_range[RIGHT] = eng_max[RIGHT] - eng_min[RIGHT];
		adc_min[0] = rev_min[LEFT];
		adc_min[1] = rev_min[RIGHT];
		adc_min[2] = eng_min[LEFT];
		adc_min[3] = eng_min[RIGHT];
		adc_min[4] = spdbrk_min;
		adc_max[0] = rev_max[LEFT];
		adc_max[1] = rev_max[RIGHT];
		adc_max[2] = eng_max[LEFT];
		adc_max[3] = eng_max[RIGHT];
		adc_max[4] = spdbrk_max;
		for (auto i = 0; i < 5; i++) {
			adc_range[i] = adc_max[i] - adc_min[i];
		}
	}

	void ThrottleAndJoystick::calibrate()
	{
		if (!ThrottleAndJoystick::calibration_init)
		{
			cal_eng_min[LEFT] = LONG_MAX;
			cal_eng_min[RIGHT] = LONG_MAX;
			cal_spdbrk_min = LONG_MAX;
			cal_rev_min[LEFT] = LONG_MAX;
			cal_rev_min[RIGHT] = LONG_MAX;

			cal_eng_max[LEFT] = 0;
			cal_eng_max[RIGHT] = 0;
			cal_spdbrk_max = 0;
			cal_rev_max[LEFT] = 0;
			cal_rev_max[RIGHT] = 0;
			ThrottleAndJoystick::calibration_init = true;
		}
		if (adc_filtered[ENG_1] < cal_eng_min[LEFT])
		{
			cal_eng_min[LEFT] = adc_filtered[ENG_1];
			update_eng1_min_text(cal_eng_min[LEFT]);
		}
		if (adc_filtered[ENG_1] > cal_eng_max[LEFT])
		{
			cal_eng_max[LEFT] = adc_filtered[ENG_1];
			update_eng1_max_text(cal_eng_max[LEFT]);
		}

		if (adc_filtered[ENG_2] < cal_eng_min[RIGHT])
		{
			cal_eng_min[RIGHT] = adc_filtered[ENG_2];
			update_eng2_min_text(cal_eng_min[RIGHT]);
		}
		if (adc_filtered[ENG_2] > cal_eng_max[RIGHT])
		{
			cal_eng_max[RIGHT] = adc_filtered[ENG_2];
			update_eng2_max_text(cal_eng_max[RIGHT]);
		}

		if (adc_filtered[SPBRK] < cal_spdbrk_min)
		{
			cal_spdbrk_min = adc_filtered[SPBRK];
			update_spdbrk_min_text(cal_spdbrk_min);
		}
		if (adc_filtered[SPBRK] > cal_spdbrk_max)
		{
			cal_spdbrk_max = adc_filtered[SPBRK];
			update_spdbrk_max_text(cal_spdbrk_max);
		}

		if (adc_filtered[REV_1] < cal_rev_min[LEFT])
		{
			cal_rev_min[LEFT] = adc_filtered[REV_1];
			update_rev1_min_text(cal_rev_min[LEFT]);
		}
		if (adc_filtered[REV_1] > cal_rev_max[LEFT])
		{
			cal_rev_max[LEFT] = adc_filtered[REV_1];
			update_rev1_max_text(cal_rev_max[LEFT]);
		}

		if (adc_filtered[REV_2] < cal_rev_min[RIGHT])
		{
			cal_rev_min[RIGHT] = adc_filtered[REV_2];
			update_rev2_min_text(cal_rev_min[RIGHT]);
		}
		if (adc_filtered[REV_2] > cal_rev_max[RIGHT])
		{
			cal_rev_max[RIGHT] = adc_filtered[REV_2];
			update_rev2_max_text(cal_rev_max[RIGHT]);
		}
	}

	void ThrottleAndJoystick::save_calibration()
	{
		eng_min[LEFT] = cal_eng_min[LEFT];
		eng_min[RIGHT] = cal_eng_min[RIGHT];
	//	eng_range[LEFT] = eng_max[LEFT] - eng_min[LEFT];
	//	eng_range[RIGHT] = eng_max[RIGHT] - eng_min[RIGHT];
	
		spdbrk_min = cal_spdbrk_min;
		rev_min[LEFT] = cal_rev_min[LEFT];
		rev_min[RIGHT] = cal_rev_min[RIGHT];
	
		eng_max[LEFT] = cal_eng_max[LEFT];
		eng_max[RIGHT] = cal_eng_max[RIGHT];
		spdbrk_max = cal_spdbrk_max;
		rev_max[LEFT] = cal_rev_max[LEFT];
		rev_max[RIGHT] = cal_rev_max[RIGHT];
	
	//	eng_scaler[LEFT] = MAX_AXIS / (eng_max[LEFT] - eng_min[LEFT]);
	//	eng_scaler[RIGHT] = MAX_AXIS / (eng_max[RIGHT] - eng_min[RIGHT]);
	//	spdbrk_scaler = MAX_AXIS / (spdbrk_max - spdbrk_min);
	//	rev_scaler[LEFT] = (MAX_REVERSER) / (rev_max[LEFT] - rev_min[LEFT]);
	//	rev_scaler[RIGHT] = (MAX_REVERSER) / (rev_max[RIGHT] - rev_min[RIGHT]);
	
		CockpitCfg::ptr->eng1_min = eng_min[LEFT];
		CockpitCfg::ptr->eng2_min = eng_min[RIGHT];
		CockpitCfg::ptr->spdbrk_min = spdbrk_min;
		CockpitCfg::ptr->rev1_min = rev_min[LEFT];
		CockpitCfg::ptr->rev2_min = rev_min[RIGHT];
	
		CockpitCfg::ptr->eng1_max = eng_max[LEFT];
		CockpitCfg::ptr->eng2_max = eng_max[RIGHT];
		CockpitCfg::ptr->spdbrk_max = spdbrk_max;
		CockpitCfg::ptr->rev1_max = rev_max[LEFT];
		CockpitCfg::ptr->rev2_max = rev_max[RIGHT];
	
		CockpitCfg::save();

		calibration_save = false;
		calibration_cancel = false;
		adc_min[0] = rev_min[LEFT];
		adc_min[1] = rev_min[RIGHT];
		adc_min[2] = eng_min[LEFT];
		adc_min[3] = eng_min[RIGHT];
		adc_min[4] = spdbrk_min;
		adc_max[0] = rev_max[LEFT];
		adc_max[1] = rev_max[RIGHT];
		adc_max[2] = eng_max[LEFT];
		adc_max[3] = eng_max[RIGHT];
		adc_max[4] = spdbrk_max;
		for (auto i = 0; i < 5; i++) {
			adc_range[i] = adc_max[i] - adc_min[i];
		}
	}

	
	void ThrottleAndJoystick::init_callbacks()
	{
		auto cb_calibrate = [this]()
		{
			set_enable_realtime_display(true);
			set_enable_calibration(true);
			set_cancel_calibration(false);			
		};
		main_window->add_callback(CallbackTypes::Calibrate, cb_calibrate);

		auto cb_save_calibration = [this]()
		{
			set_save_calibration(true);		
			set_enable_realtime_display(false);
			set_enable_calibration(false);
			main_window->clear_all_throttle_value_displays();
		};
		main_window->add_callback(CallbackTypes::SaveCalibration, cb_save_calibration);

		auto cb_cancel_calibration = [this]()
		{
			set_enable_realtime_display(false);
			set_enable_calibration(false);
			set_cancel_calibration(true);
			main_window->clear_all_throttle_value_displays();
		};
		main_window->add_callback(CallbackTypes::CancelCalibration, cb_cancel_calibration);

		auto cb_test_throttle = [this]()
		{
			set_enable_realtime_display(true);
		};
		main_window->add_callback(CallbackTypes::ThrottleTest, cb_test_throttle);

		auto cb_stop_throttle_test = [this]()
		{
			set_enable_realtime_display(false);
			main_window->clear_all_throttle_value_displays();
		};
		main_window->add_callback(CallbackTypes::StopThrottleTest, cb_stop_throttle_test);
	}


	//
	// Messages to GUI
	//


	void ThrottleAndJoystick::update_eng1_min_text(const int val) const
	{
		main_window->set_eng1_min(val);
	}

	void ThrottleAndJoystick::update_eng1_text(const int val) const
	{
		main_window->set_eng1_value(val);
	}

	void ThrottleAndJoystick::update_eng1_max_text(const int val) const
	{
		main_window->set_eng1_max(val);
	}

	void ThrottleAndJoystick::update_eng2_min_text(const int val) const
	{
		main_window->set_eng2_min(val);
	}

	void ThrottleAndJoystick::update_eng2_text(const int val) const
	{
		main_window->set_eng2_value(val);
	}

	void ThrottleAndJoystick::update_eng2_max_text(const int val) const
	{
		main_window->set_eng2_max(val);
	}

	void ThrottleAndJoystick::update_spdbrk_min_text(const int val) const
	{
		main_window->set_spd_brk_min(val);
	}

	void ThrottleAndJoystick::update_spdbrk_text(const int val) const
	{
		main_window->set_spd_brk_value(val);
	}

	void ThrottleAndJoystick::update_spdbrk_max_text(const int val) const
	{
		main_window->set_spd_brk_max(val);
	}

	void ThrottleAndJoystick::update_rev1_min_text(const int val) const
	{
		main_window->set_rev1_min(val);
	}

	void ThrottleAndJoystick::update_rev1_text(const int val) const
	{
		main_window->set_rev1_value(val);
	}

	void ThrottleAndJoystick::update_rev1_max_text(const int val) const
	{
		main_window->set_rev1_max(val);
	}

	void ThrottleAndJoystick::update_rev2_min_text(const int val) const
	{
		main_window->set_rev2_min(val);
	}

	void ThrottleAndJoystick::update_rev2_text(const int val) const
	{
		main_window->set_rev2_value(val);
	}

	void ThrottleAndJoystick::update_rev2_max_text(const int val) const
	{
		main_window->set_rev2_max(val);
	}
}