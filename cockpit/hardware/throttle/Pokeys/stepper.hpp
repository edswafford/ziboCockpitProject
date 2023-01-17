#pragma once
#include "PoKeysLib.h"
#include <utility>
#include <memory>
#include <mutex>


namespace zcockpit::cockpit::hardware
{
	static const int LEFT = 0;
	static const int RIGHT = 1;


	enum StepperMotor
	{
		LEFT_THROTTLE,
		RIGHT_THROTTLE,
		SPDBRK,
	};

	class Stepper
	{
	public:
		Stepper(const int stp, const int dir, const int en, const int analog_input, const int analog_idle_offset,
			const int min, const int max, const int min_limit, const int max_limit);

		int stp_;
		int dir_;
		int en_;

		int analog_index_;
		int min_;
		int max_;

		int min_limit_;
		int max_limit_;
		int analog_idle_offset_;
	};

	class Step_Controller
	{
	public:
		Step_Controller();
		void initialize(Stepper *left, Stepper *right, Stepper *spdbrk);

		void drop();
		void static timer();

		bool is_initialized() { return  initialized; }

		void step_forward(StepperMotor stepper, const unsigned number_of_steps) const;
		void step_reverse(StepperMotor stepper, const unsigned number_of_steps) const;
		

	private:
		void static step_both_throttles(
		);


		void static step_commanded(const Stepper* stepper, const bool dir, const double commanded, std::atomic<int>& filtered_value);

		void static process_throttle_steppers();
		void static process_spdbrk_stepper();


		static Stepper *left_;
		static Stepper *right_;
		static Stepper *spdbrk_;
		static int left_range;
		static int right_range;
		bool initialized;

		static bool stop_timer;
		static bool timer_has_stopped;

		static std::mutex stepper_timer_mutex;
		static std::mutex timer_done_mutex;
		static std::condition_variable condition;
	};
}
