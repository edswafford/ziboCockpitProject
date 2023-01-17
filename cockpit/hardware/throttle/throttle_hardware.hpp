#pragma once
#include <set>
namespace zcockpit::cockpit::hardware
{
	const auto LOW = 1;
	const auto HIGH = 0;
	//
	// INPUTS
	//
	const auto eng_1_idle_cutoff_sw = 0;	// pin 1 # 1 Idle/Cutoff
	const auto eng_2_idle_cutoff_sw = 1;	// pin 2 # 1 Idle/Cutoff
	const auto toga_sw = 2;					// pin 3 TOGA
	const auto auto_throttle_sw = 3;		// pin 4 AT
	const auto parking_brake_sw = 4;		// pin 5 Parking Brake
	const auto horn_cutout_sw = 5;			// pin 6 hort cutout


	//
	// OUTPUTS
	//
	const auto mux_s0 = 6;	// pin 7
	const auto mux_s1 = 7;	// pin 8
	const auto mux_s2 = 8;	// pin 9

							// Steppers
	const auto eng_1_stp = 9;  // pin 10
	const auto eng_1_dir = 10; // pin 11
	const auto eng_1_MS1 = 11; // pin 12
	const auto eng_1_MS2 = 11; // pin 
	const auto eng_1_MS3 = 11; // pin 
	const auto eng_1_EN = 12;  // pin 13

	const auto eng_2_stp = 13; // pin 14
	const auto eng_2_dir = 14; // pin 15
	const auto eng_2_MS1 = 15; // pin 16
	const auto eng_2_MS2 = 15; // pin 
	const auto eng_2_MS3 = 15; // pin  
	const auto eng_2_EN = 16;  // pin 17

	const auto spoiler_stp = 17; // pin 18
	const auto spoiler_dir = 18; // pin 19
	const auto spoiler_MS1 = 19; // pin 20
	const auto spoiler_MS2 = 19; // pin 
	const auto spoiler_MS3 = 19; // pin    
	const auto spoiler_EN = 20;  // pin 21

	const auto stab_trim_man_cutout_sw = 27;	// pin 28
	const auto stab_trim_auto_cutout_sw = 28;	// pin 29

	const auto parking_brake_light = 29;	// pin 30
	//
	// ANALOG INPUTS
	//
	const auto eng_1_reverser_analog_input = 40;	// pin 41
	const auto eng_2_reverser_analog_input = 41;	// pin 42
	const auto eng_1_thrust_analog_input = 42;		// pin 43
	const auto eng_2_thrust_analog_input = 43;		// pin 44
	const auto spoiler_analog_input = 44;			// pin 45
	const auto flaps_analog_input = 45;				// pin 46
}