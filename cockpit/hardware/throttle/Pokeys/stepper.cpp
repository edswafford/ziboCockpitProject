#include "stepper.hpp"
#include <thread>
#include "../throttle_hardware.hpp"
//#include "../../ioCards/iocards.h"
//#include "../../../util.h"
#include "shared_types.hpp"
#include "../common/logger.hpp"
#include "../throttle_joystick.hpp"

extern logger LOG;

namespace zcockpit::cockpit::hardware
{
	extern std::atomic<bool> stepper_running;

	bool Step_Controller::stop_timer = true;
	bool Step_Controller::timer_has_stopped = true;
	Stepper *Step_Controller::left_ = nullptr;
	Stepper *Step_Controller::right_ = nullptr;
	Stepper *Step_Controller::spdbrk_ = nullptr;
	int Step_Controller::left_range = 500;
	int Step_Controller::right_range = 500;

	std::mutex Step_Controller::stepper_timer_mutex;
	std::mutex Step_Controller::timer_done_mutex;
	std::condition_variable Step_Controller::condition;

	Stepper::Stepper(const int stp, const int dir, const int en, const int analog_index, const int analog_idle_offset,
		const int min, const int max, const int min_limit, const int max_limit) :
		stp_(stp), dir_(dir), en_(en), analog_index_(analog_index), min_(min), max_(max), analog_idle_offset_(analog_idle_offset)
	{
		// Set Absolute limits
		min_limit_ = min_limit;
		max_limit_ = max_limit;

	}


	Step_Controller::Step_Controller() : initialized(false)
	{
	}

	void Step_Controller::initialize(Stepper * left, Stepper * right, Stepper * spdbrk)
	{
		left_ = left;
		right_ = right;
		spdbrk_ = spdbrk;

		//initialize outputs
		ThrottleAndJoystick::deviceStat_.Pins[left_->stp_].DigitalOutputValue = LOW;
		ThrottleAndJoystick::deviceStat_.Pins[right_->stp_].DigitalOutputValue = LOW;
		ThrottleAndJoystick::deviceStat_.Pins[spdbrk_->stp_].DigitalOutputValue = LOW;

		ThrottleAndJoystick::deviceStat_.Pins[left_->dir_].DigitalOutputValue = LOW;
		ThrottleAndJoystick::deviceStat_.Pins[right_->dir_].DigitalOutputValue = LOW;
		ThrottleAndJoystick::deviceStat_.Pins[spdbrk_->dir_].DigitalOutputValue = LOW;
		// Disable
		ThrottleAndJoystick::deviceStat_.Pins[left_->en_].DigitalOutputValue = LOW; // LOW == disable
		ThrottleAndJoystick::deviceStat_.Pins[right_->en_].DigitalOutputValue = LOW;
		ThrottleAndJoystick::deviceStat_.Pins[spdbrk_->en_].DigitalOutputValue = LOW;

		left_range = left_->max_ - left_->min_;
		right_range = right_->max_ - right_->min_;

		initialized = true;
	}

	void Step_Controller::drop()
	{
		std::unique_lock<std::mutex> lk(timer_done_mutex);
		{
			std::lock_guard<std::mutex> lock(stepper_timer_mutex);
			stop_timer = true;
			LOG() << "Request Stepper Timer to terminate";
		}
		condition.wait(lk, [this]
		{
			return (this->timer_has_stopped);
		});

		LOG() << "Stepper terminated: Goodby Stepper.";
		stepper_running = false;
	}

	void Step_Controller::timer()
	{
		stop_timer = false;
		timer_has_stopped = false;
		stepper_running = true;
		while (true)
		{
			// do we need to shut down
			{
				std::lock_guard<std::mutex> lock(stepper_timer_mutex);
				if (stop_timer)
				{
					LOG() << "Stepper timer is stopping";
					break;
				}
			}
			try
			{
				process_throttle_steppers();

				process_spdbrk_stepper();

			}
			catch (std::exception& e)
			{
				LOG() << "Steppper Exception: " << e.what();
			}

			//
			// sleep
			//
			// update rate is 20Hz
			std::this_thread::sleep_for(std::chrono::milliseconds(common::UPDATE_RATE));
		}

		// timer  has stopped
		LOG() << "Stepper timer has stopped";

		std::unique_lock<std::mutex> lk(timer_done_mutex);
		timer_has_stopped = true;
		lk.unlock();
		condition.notify_one();
		LOG() << "Stepper timer returning";
	}


	extern std::atomic<bool> stepper_test_running;
	void Step_Controller::step_forward(StepperMotor stepper, const unsigned number_of_steps) const
	{
		const auto start = std::chrono::high_resolution_clock::now();

		stepper_test_running = true;
		Stepper* sptr = left_;
		if(stepper == RIGHT_THROTTLE)
		{
			sptr = right_;
		}
		else if(stepper == SPDBRK)
		{
			sptr = spdbrk_;
		}
		{
			std::lock_guard<std::mutex> lock(pokeys_mutex);
			ThrottleAndJoystick::deviceStat_.Pins[sptr->en_].DigitalOutputValue = HIGH;
			SetDigitalOutputs(&ThrottleAndJoystick::deviceStat_); // Sets the outputs and reads the inputs

			//Pull direction pin low to move "forward"
			ThrottleAndJoystick::deviceStat_.Pins[sptr->dir_].DigitalOutputValue = LOW;
			SetDigitalOutputs(&ThrottleAndJoystick::deviceStat_); // Sets the outputs and reads the inputs
		}
		for (auto i = 0U; i < number_of_steps; i++)
		{
			//Trigger one step forward
			{
				std::lock_guard<std::mutex> lock(pokeys_mutex);

				ThrottleAndJoystick::deviceStat_.Pins[sptr->stp_].DigitalOutputValue = HIGH;
				SetDigitalOutputs(&ThrottleAndJoystick::deviceStat_); // Sets the outputs and reads the inputs
			}
			std::this_thread::sleep_for(std::chrono::microseconds(10));

			//Pull step pin low so it can be triggered again
			{
				std::lock_guard<std::mutex> lock(pokeys_mutex);
				ThrottleAndJoystick::deviceStat_.Pins[sptr->stp_].DigitalOutputValue = LOW;
				SetDigitalOutputs(&ThrottleAndJoystick::deviceStat_);
			}
			std::this_thread::sleep_for(std::chrono::microseconds(10));
		}
		{
			std::lock_guard<std::mutex> lock(pokeys_mutex);
			ThrottleAndJoystick::deviceStat_.Pins[sptr->en_].DigitalOutputValue = LOW;
		}
		const auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed1 = end - start;
		LOG() << "#1 " << elapsed1.count();
		stepper_test_running = false;
	}

	void Step_Controller::step_reverse(StepperMotor stepper, const unsigned number_of_steps) const
	{
		const auto start = std::chrono::high_resolution_clock::now();

		stepper_test_running = true;
		Stepper* sptr = left_;
		if (stepper == RIGHT_THROTTLE)
		{
			sptr = right_;
		}
		else if (stepper == SPDBRK)
		{
			sptr = spdbrk_;
		}
		{
			std::lock_guard<std::mutex> lock(pokeys_mutex);
			ThrottleAndJoystick::deviceStat_.Pins[sptr->en_].DigitalOutputValue = HIGH;
			SetDigitalOutputs(&ThrottleAndJoystick::deviceStat_);

			ThrottleAndJoystick::deviceStat_.Pins[sptr->dir_].DigitalOutputValue = HIGH;
			SetDigitalOutputs(&ThrottleAndJoystick::deviceStat_); 
		}


		for (auto i = 1U; i < number_of_steps; i++) //Loop the forward stepping enough times for motion to be visible
		{
			//Trigger one step forward
			{
				std::lock_guard<std::mutex> lock(pokeys_mutex);
				ThrottleAndJoystick::deviceStat_.Pins[sptr->stp_].DigitalOutputValue = HIGH;
				SetDigitalOutputs(&ThrottleAndJoystick::deviceStat_); // Sets the outputs and reads the inputs
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1));

			//Pull step pin low so it can be triggered again
			{
				std::lock_guard<std::mutex> lock(pokeys_mutex);
				ThrottleAndJoystick::deviceStat_.Pins[sptr->stp_].DigitalOutputValue = LOW;
				SetDigitalOutputs(&ThrottleAndJoystick::deviceStat_);
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		{
			std::lock_guard<std::mutex> lock(pokeys_mutex);
			ThrottleAndJoystick::deviceStat_.Pins[sptr->en_].DigitalOutputValue = LOW;
		}
		const auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed1 = end - start;
		LOG() << "#1 " << elapsed1.count();
		stepper_test_running = false;
	}


	void Step_Controller::process_throttle_steppers()
	{
		//if (Ifly737::shareMemSDK != nullptr && Ifly737::shareMemSDK->IFLY737NG_STATE)
		//{
		//	if (ThrottleAndJoystick::enable_at_steppers)
		//	{
		//		step_both_throttles();
		//	}
		//}
	}

	enum Speed_brake_states
	{
		DEPLOYING,
		DEPLOYED,
		RETRACTING,
		STOWED,
	};

	void Step_Controller::process_spdbrk_stepper()
	{
		static auto SPDBRK_RANGE = spdbrk_->max_ - spdbrk_->min_;

		static auto SPDBRK_ARMED_DETENT = spdbrk_->min_ + 200;
		static int SPDBRK_FULLY_DEPLOYED = spdbrk_->max_limit_ - 100;
		static int SPDBRK_RETRACTED = spdbrk_->min_limit_;
		static Speed_brake_states spdbrk_state = STOWED;

		//if (Ifly737::shareMemSDK != nullptr && Ifly737::shareMemSDK->IFLY737NG_STATE && FsxSimConnect::sim_on_ground)
		//{
		//	const bool reverser_deployed = ThrottleAndJoystick::rev_deployed[LEFT] && ThrottleAndJoystick::rev_deployed[RIGHT];
		//	const bool throttles_idle = ThrottleAndJoystick::adc_filtered[ThrottleAndJoystick::ENG_1] < left_->min_limit_ + 100 && ThrottleAndJoystick::adc_filtered[ThrottleAndJoystick::ENG_2] < right_->min_limit_ + 100;

		//	const bool auto_spdbrk_armed = Ifly737::shareMemSDK->SPEED_BRAKE_ARMED_Light_Status && ThrottleAndJoystick::adc_filtered[ThrottleAndJoystick::SPBRK] > SPDBRK_ARMED_DETENT;

		//	const bool deploy_speed_brakes = (auto_spdbrk_armed || reverser_deployed) && throttles_idle && FsxSimConnect::ground_speed > 60.0;
		//	std::atomic<int> speed_brakes_filtered = ThrottleAndJoystick::adc_filtered[ThrottleAndJoystick::SPBRK];
		//	if( spdbrk_state == DEPLOYING || deploy_speed_brakes )
		//	{
		//		if(speed_brakes_filtered >= SPDBRK_FULLY_DEPLOYED)
		//		{
		//			spdbrk_state = DEPLOYED;
		//		}
		//		else {
		//			spdbrk_state = DEPLOYING;
		//			step_commanded(spdbrk_, true, 100.0, speed_brakes_filtered);

		//			LOG() << "Speed Brakes Engaged: " << speed_brakes_filtered << 
		//				" throttle L " << ThrottleAndJoystick::adc_filtered[ThrottleAndJoystick::ENG_1] << " throttle R " << ThrottleAndJoystick::adc_filtered[ThrottleAndJoystick::ENG_2] <<
		//				" grd spd " << FsxSimConnect::ground_speed;
		//		}
		//	}
		//	else if(spdbrk_state == RETRACTING || ((spdbrk_state == DEPLOYED) && !reverser_deployed &&
		//		(ThrottleAndJoystick::adc_filtered[ThrottleAndJoystick::ENG_1] > left_->min_limit_ + 100 || ThrottleAndJoystick::adc_filtered[ThrottleAndJoystick::ENG_2] > right_->min_limit_ + 100)))
		//	{
		//		if (speed_brakes_filtered <= SPDBRK_RETRACTED)
		//		{
		//			spdbrk_state = STOWED;
		//		}
		//		else
		//		{
		//			// Disengage Speed Brakes
		//			spdbrk_state = RETRACTING;
		//			step_commanded(spdbrk_, true, 0.0, speed_brakes_filtered);
		//			LOG() << "Speed Brakes Disengaged " << speed_brakes_filtered << " throttle L " << ThrottleAndJoystick::adc_filtered[ThrottleAndJoystick::ENG_1] <<
		//				" throttle R " << ThrottleAndJoystick::adc_filtered[ThrottleAndJoystick::ENG_2] << " grd spd " << FsxSimConnect::ground_speed;
		//		}
		//	}
		//}



		////if (client->aircraft_is_running)
		////{
		////	const double commanded = FsxSimConnect::spoiler_position * 100;
		////	step_commanded(spdbrk_, true, commanded, Throttle::speed_brakes_filtered);

		////}
	}

	void Step_Controller::step_both_throttles() 
	{

		//Pull direction pin low to move "forward"
		const unsigned char left_forward = LOW;
		const unsigned char left_reverse = HIGH;

		const unsigned char right_forward = HIGH;
		const unsigned char right_reverse = LOW;

		auto left_enable = HIGH;
		auto right_enable = HIGH;
		auto left_direction = 0;
		auto right_direction = 0;


		auto left_throttle_precent = 100.0 / left_range * (ThrottleAndJoystick::adc_filtered[ThrottleAndJoystick::ENG_1] - left_->min_) + left_->analog_idle_offset_;
		auto left_delta_value = ThrottleAndJoystick::left_n1_commanded - left_throttle_precent;
		
		//if (abs(left_delta_value) < 2.0)
		//{
		//	LOG() << "LEFT range " << left_range << " raw " << Throttle::throttle_filtered[LEFT] << " min/max " << left_->min_ << "/" << left_->max_ <<
		//		" % " << left_throttle_precent << " cmd " << left_commanded;
		//}

		if (left_delta_value > 2.0)
		{
			if (ThrottleAndJoystick::adc_filtered[ThrottleAndJoystick::ENG_1] > left_->max_limit_)
			{
				LOG() << "Stepper: LEFT Reach alsolute MAX limit of lever " << ThrottleAndJoystick::adc_filtered[ThrottleAndJoystick::ENG_1];
				left_enable = LOW;
			}
			else {

				left_direction = left_forward;
			}
		}
		else if (left_delta_value < -2.0)
		{
			if (ThrottleAndJoystick::adc_filtered[ThrottleAndJoystick::ENG_1] < left_->min_limit_)
			{
				LOG() << "Stepper: LEFT Reach alsolute MIN limit of lever " << ThrottleAndJoystick::adc_filtered[ThrottleAndJoystick::ENG_1];
				left_enable = LOW;
			}
			else
			{
				left_direction = left_reverse;
			}
		}
		else
		{
	//		LOG() << "Stepper Less than 2.0 LEFT " << left_delta_value;
			left_enable = LOW;
		}


		//
		// RIGHT
		//
		auto right_throttle_precent = 100.0 / right_range * (ThrottleAndJoystick::adc_filtered[ThrottleAndJoystick::ENG_2] - right_->min_) + right_->analog_idle_offset_;
		auto right_delta_value = ThrottleAndJoystick::right_n1_commanded - right_throttle_precent;

		//if (abs(right_delta_value) < 2.0)
		//{
		//	LOG() << "RIGHT range " << right_range << " raw " << Throttle::throttle_filtered[RIGHT] << " min/max " << right_->min_ << "/" << right_->max_ <<
		//		" % " << right_throttle_precent << " cmd " << right_commanded;
		//}

		if (right_delta_value > 2.0)
		{
			if (ThrottleAndJoystick::adc_filtered[ThrottleAndJoystick::ENG_2] > right_->max_limit_)
			{
				LOG() << "Stepper: RIGHT Reach alsolute MAX limit of lever " << ThrottleAndJoystick::adc_filtered[ThrottleAndJoystick::ENG_2];
				right_enable = LOW;
			}
			else
			{
				right_direction = right_forward;
			}
		}
		else if (right_delta_value < -2.0)
		{
			if (ThrottleAndJoystick::adc_filtered[ThrottleAndJoystick::ENG_2] < right_->min_limit_)
			{
				LOG() << "Stepper: RIGHT Reach alsolute MIN limit of lever " << ThrottleAndJoystick::adc_filtered[ThrottleAndJoystick::ENG_2];
				right_enable = LOW;
			}
			else
			{
				right_direction = right_reverse;
			}
		}
		else
		{
	//		LOG() << "Stepper Less than 2.0 RIGHT " << right_delta_value;
			right_enable = LOW;
		}

		
		if ((left_enable == HIGH || right_enable == HIGH) && !ThrottleAndJoystick::auto_throttle_is_disengaged)
		{
			{
				std::lock_guard<std::mutex> lock(pokeys_mutex);
				if (left_enable == HIGH)
				{
					ThrottleAndJoystick::deviceStat_.Pins[left_->en_].DigitalOutputValue = HIGH;
				}
				if (right_enable == HIGH)
				{
					ThrottleAndJoystick::deviceStat_.Pins[right_->en_].DigitalOutputValue = HIGH;
				}
				SetDigitalOutputs(&ThrottleAndJoystick::deviceStat_);
				std::this_thread::sleep_for(std::chrono::milliseconds(1));


				ThrottleAndJoystick::deviceStat_.Pins[left_->dir_].DigitalOutputValue = left_direction;
				ThrottleAndJoystick::deviceStat_.Pins[right_->dir_].DigitalOutputValue = right_direction;
				SetDigitalOutputs(&ThrottleAndJoystick::deviceStat_);
			}

			auto loop_cnt = 1;
			while (left_enable == HIGH || right_enable == HIGH && ThrottleAndJoystick::enable_at_steppers)
			{
				if (ThrottleAndJoystick::disengage_auto_throttle == 0)
				{
					//Throttle::enable_at_steppers = false;
					LOG() << "AUTO THROTTLE  both throttles Disengage auto throttle";
					break;
				}

				if (loop_cnt % 10 == 0)
				{
					left_throttle_precent = 100.0 / left_range * (ThrottleAndJoystick::adc_filtered[ThrottleAndJoystick::ENG_1] - left_->min_) + left_->analog_idle_offset_;
					left_delta_value = ThrottleAndJoystick::left_n1_commanded - left_throttle_precent;
					right_throttle_precent = 100.0 / right_range * (ThrottleAndJoystick::adc_filtered[ThrottleAndJoystick::ENG_2] - right_->min_) + right_->analog_idle_offset_;
					right_delta_value = ThrottleAndJoystick::right_n1_commanded - right_throttle_precent;
					if (left_delta_value >= 0.0)
					{
						if (left_direction != left_forward)
						{
							left_direction = left_forward;
						}
					}
					else
					{
						if (left_direction != left_reverse)
						{
							left_direction = left_reverse;
						}
					}
					if (right_delta_value >= 0.0)
					{
						if (right_direction != right_forward)
						{
							right_direction = right_forward;
						}
					}
					else
					{
						if (right_direction != right_reverse)
						{
							right_direction = right_reverse;
						}
					}
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(2));
				if (abs(left_delta_value) > 2.0 &&
					((left_direction == left_forward && ThrottleAndJoystick::adc_filtered[ThrottleAndJoystick::ENG_1] < left_->max_limit_) ||
						(left_direction == left_reverse && ThrottleAndJoystick::adc_filtered[ThrottleAndJoystick::ENG_1] > left_->min_limit_)))
				{
					//Trigger one step
					std::lock_guard<std::mutex> lock(pokeys_mutex);
					ThrottleAndJoystick::deviceStat_.Pins[left_->stp_].DigitalOutputValue = HIGH;
					SetDigitalOutputs(&ThrottleAndJoystick::deviceStat_); // Sets the outputs and reads the inputs

					//Pull step pin low so it can be triggered again
					ThrottleAndJoystick::deviceStat_.Pins[left_->stp_].DigitalOutputValue = LOW;
					SetDigitalOutputs(&ThrottleAndJoystick::deviceStat_);
				}
				else
				{
					left_enable = LOW;
					//LOG() << "Left " << left_delta_value << " cmd " << left_commanded << " throttle % " << left_throttle_precent;
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(2));
				if (abs(right_delta_value) > 2.0 && 
					((right_direction == right_forward && ThrottleAndJoystick::adc_filtered[ThrottleAndJoystick::ENG_2] < right_->max_limit_ ) ||
						(right_direction == right_reverse && ThrottleAndJoystick::adc_filtered[ThrottleAndJoystick::ENG_2] > right_->min_limit_)))
				{
					std::lock_guard<std::mutex> lock(pokeys_mutex);
					ThrottleAndJoystick::deviceStat_.Pins[right_->stp_].DigitalOutputValue = HIGH;
					SetDigitalOutputs(&ThrottleAndJoystick::deviceStat_); // Sets the outputs and reads the inputs

					ThrottleAndJoystick::deviceStat_.Pins[right_->stp_].DigitalOutputValue = LOW;
					SetDigitalOutputs(&ThrottleAndJoystick::deviceStat_);
				}
				else
				{
					right_enable = LOW;
					//LOG() << "Right " << right_delta_value << " cmd " << right_commanded << " throttle % " << right_throttle_precent;
				}
				loop_cnt += 1;
				;
			}
			LOG() << "StepBoth, LOOP COUNT = " << loop_cnt << " LEFT comm " << ThrottleAndJoystick::left_n1_commanded << " analog " << ThrottleAndJoystick::adc_filtered[ThrottleAndJoystick::ENG_1] << " RIGHT comm " << ThrottleAndJoystick::right_n1_commanded << " analog " << ThrottleAndJoystick::adc_filtered[ThrottleAndJoystick::ENG_2];
		}

		{
			std::lock_guard<std::mutex> lock(pokeys_mutex);
			ThrottleAndJoystick::deviceStat_.Pins[left_->en_].DigitalOutputValue = LOW;
			ThrottleAndJoystick::deviceStat_.Pins[right_->en_].DigitalOutputValue = LOW;
			SetDigitalOutputs(&ThrottleAndJoystick::deviceStat_);
		}
	}

	void Step_Controller::step_commanded(const Stepper* stepper, const bool dir, const double commanded, std::atomic<int>& filtered_value)
	{
		//Pull direction pin low to move "forward"
		unsigned char forward = LOW;
		unsigned char reverse = HIGH;
		const auto range = stepper->max_ - stepper->min_;

		if (!dir)
		{
			forward = HIGH;
			reverse = LOW;
		}

		auto enable = HIGH;
		auto direction = 0;

		
		auto stepper_precent = 0.0;
		if(filtered_value > stepper->max_)
		{
			stepper_precent = 100.0;
		}
		else
		{
			const auto scaled = filtered_value - stepper->min_;
			if (scaled > 0.0)
			{
				stepper_precent = 100.0 / range * scaled;
			}
		}
		auto delta_value = commanded - stepper_precent;
		//LOG() << "Stepper % " << stepper_precent << " filtered Value " << filtered_value;

		if (delta_value > 2.0)
		{
			if (filtered_value > stepper->max_limit_)
			{
				LOG() << "Stepper: Stepper Reach alsolute MAX limit of lever " << filtered_value;
				enable = LOW;
			}
			else {

				direction = forward;
			}
		}
		else if (delta_value < -2.0)
		{
			if (filtered_value < stepper->min_)
			{
				LOG() << "Stepper: Stepper Reach alsolute MIN limit of lever " << filtered_value;
				enable = LOW;
			}
			else
			{
				direction = reverse;
			}
		}
		else
		{
			//		LOG() << "Stepper Less than 2.0 " << delta_value;
			enable = LOW;
		}




		if (enable == HIGH)
		{
			{
				std::lock_guard<std::mutex> lock(pokeys_mutex);
				if (enable == HIGH)
				{
					ThrottleAndJoystick::deviceStat_.Pins[stepper->en_].DigitalOutputValue = HIGH;
				}
				SetDigitalOutputs(&ThrottleAndJoystick::deviceStat_);
				std::this_thread::sleep_for(std::chrono::milliseconds(1));


				ThrottleAndJoystick::deviceStat_.Pins[stepper->dir_].DigitalOutputValue = direction;
				SetDigitalOutputs(&ThrottleAndJoystick::deviceStat_);
			}

			auto loop_cnt = 1;
			while (enable == HIGH)
			{
				//LOG() << "Stepper % " << stepper_precent << " filtered Value " << filtered_value << " min limit " << stepper->min_limit_;

				if (loop_cnt % 5 == 0)
				{
					if (filtered_value > stepper->max_)
					{
						stepper_precent = 100.0;
					}
					else
					{
						const auto scaled = filtered_value - stepper->min_;
						if (scaled > 0.0)
						{
							stepper_precent = 100.0 / range * scaled;
						}
					}
					delta_value = commanded - stepper_precent;
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(2));
				if ( (delta_value > 2.0 && (filtered_value <= stepper->max_limit_)) || (delta_value < -2.0 && (filtered_value > stepper->min_limit_)) )
				{
					//Trigger one step forward
					std::lock_guard<std::mutex> lock(pokeys_mutex);
					ThrottleAndJoystick::deviceStat_.Pins[stepper->stp_].DigitalOutputValue = HIGH;
					SetDigitalOutputs(&ThrottleAndJoystick::deviceStat_);

					ThrottleAndJoystick::deviceStat_.Pins[stepper->stp_].DigitalOutputValue = LOW;
					SetDigitalOutputs(&ThrottleAndJoystick::deviceStat_);
				}
				else
				{
					enable = LOW;
					LOG() << "Delta " << delta_value << " cmd " << commanded << " stepper % " << stepper_precent;
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(2));
				loop_cnt += 1;
			}
			LOG() << "SPDNRK, LOOP COUNT = " << loop_cnt << " cmd " << commanded << " analog " << filtered_value;
		}

		{
			std::lock_guard<std::mutex> lock(pokeys_mutex);
			if (stepper->en_ >= 0 && stepper->en_ < 55) {
				ThrottleAndJoystick::deviceStat_.Pins[stepper->en_].DigitalOutputValue = LOW;
				SetDigitalOutputs(&ThrottleAndJoystick::deviceStat_);
			}
		}
	}
}