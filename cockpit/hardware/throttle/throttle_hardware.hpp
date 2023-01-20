#pragma once
#include <set>
namespace zcockpit::cockpit::hardware
{
	constexpr auto LOW = 1;
	constexpr auto HIGH = 0;
	//
	// INPUTS
	//
	constexpr auto eng_1_idle_cutoff_sw = 0;	// pin 1 # 1 Idle/Cutoff
	constexpr auto eng_2_idle_cutoff_sw = 1;	// pin 2 # 1 Idle/Cutoff
	constexpr auto toga_sw = 2;					// pin 3 TOGA
	constexpr auto auto_throttle_sw = 3;		// pin 4 AT
	constexpr auto parking_brake_sw = 4;		// pin 5 Parking Brake
	constexpr auto horn_cutout_sw = 5;			// pin 6 hort cutout


	//
	// OUTPUTS
	//
	constexpr auto mux_s0 = 6;	// pin 7
	constexpr auto mux_s1 = 7;	// pin 8
	constexpr auto mux_s2 = 8;	// pin 9

							// Steppers
	constexpr auto eng_1_stp = 9;  // pin 10
	constexpr auto eng_1_dir = 10; // pin 11
	constexpr auto eng_1_MS1 = 11; // pin 12
	constexpr auto eng_1_MS2 = 11; // pin 
	constexpr auto eng_1_MS3 = 11; // pin 
	constexpr auto eng_1_EN = 12;  // pin 13

	constexpr auto eng_2_stp = 13; // pin 14
	constexpr auto eng_2_dir = 14; // pin 15
	constexpr auto eng_2_MS1 = 15; // pin 16
	constexpr auto eng_2_MS2 = 15; // pin 
	constexpr auto eng_2_MS3 = 15; // pin  
	constexpr auto eng_2_EN = 16;  // pin 17

	constexpr auto spoiler_stp = 17; // pin 18
	constexpr auto spoiler_dir = 18; // pin 19
	constexpr auto spoiler_MS1 = 19; // pin 20
	constexpr auto spoiler_MS2 = 19; // pin 
	constexpr auto spoiler_MS3 = 19; // pin    
	constexpr auto spoiler_EN = 20;  // pin 21

	constexpr auto stab_trim_man_cutout_sw = 27;	// pin 28
	constexpr auto stab_trim_auto_cutout_sw = 28;	// pin 29

	constexpr auto parking_brake_light = 29;	// pin 30
	//
	// ANALOG INPUTS
	//
	constexpr auto eng_1_reverser_analog_input = 40;	// pin 41
	constexpr auto eng_2_reverser_analog_input = 41;	// pin 42
	constexpr auto eng_1_thrust_analog_input = 42;		// pin 43
	constexpr auto eng_2_thrust_analog_input = 43;		// pin 44
	constexpr auto spoiler_analog_input = 44;			// pin 45
	constexpr auto flaps_analog_input = 45;				// pin 46
}