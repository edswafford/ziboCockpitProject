
#include "interfaceit.hpp"
#include "logger.hpp"

extern logger LOG;

namespace zcockpit::cockpit::hardware
{
	bool InterfaceIT::checkDeviceInProgress = false;
	bool InterfaceIT::avaliable = false;
	bool InterfaceIT::overHeadAvailable = false;
	bool InterfaceIT::mipAvailable = false;
	bool InterfaceIT::mipFound = false;
	bool InterfaceIT::overHeadFound = false;
	unsigned long InterfaceIT::mipSession = -1;
	unsigned long InterfaceIT::overHeadSession = -1;
	// "158300000532"	32E0 -- 32 switches, 32 LEDs
	// "158300000592"	32E1 -- 64 switches 256 LEDs
	const char* InterfaceIT::mip = "158300000532";
	const char* InterfaceIT::overhead = "158300000592";


	const unsigned long FSWRITE = 0X9400;

	unsigned char* switchState[20];
	unsigned int eecOn1;
	unsigned int eecOn2;
//	int InterfaceIT::overhead_previoussw_state[OVERHEAD_SW_SIZE];
//	int InterfaceIT::mip_previous_sw_state[MIP_SW_SIZE];

		// xplane auto=0 v1=1,vr=2,wt=3,vref=4,bug=5,set=6
		constexpr int XPLANE_SPD_REF_MODE_AUTO = 0;
		constexpr int XPLANE_SPD_REF_MODE_VR = 2;
		constexpr int XPLANE_SPD_REF_MODE_WT = 3;
		constexpr int XPLANE_SPD_REF_MODE_VREF = 4;
		constexpr int XPLANE_SPD_REF_MODE_BUG5 = 5;
		constexpr int XPLANE_SPD_REF_MODE_SET = 6;
		constexpr int XPLANE_SPD_REF_MODE_V1 = 1;

		// xplane[0,1,2,3,4,5] rto, off, 1,2,3,max
		constexpr int XPLANE_AUTO_BRAKE_RTO = 0;
		constexpr int XPLANE_AUTO_BRAKE_OFF = 1;  
		constexpr int XPLANE_AUTO_BRAKE_1 = 2; 
		constexpr int XPLANE_AUTO_BRAKE_2 = 3; 
		constexpr int XPLANE_AUTO_BRAKE_3 = 4; 
		constexpr int XPLANE_AUTO_BRAKE_MAX = 5;

		constexpr int XPLANE_FUEL_FLOW_USED = 1;
		constexpr int XPLANE_FUEL_FLOW_RATE = 0;
		constexpr int XPLANE_FUEL_FLOW_RESET = -1;


		// xplane[1, 0, -1]  up==test, center==brt, dn==dim 
		constexpr int XPLANE_MASTER_LIGHTS_TEST = 1;
		constexpr int XPLANE_MASTER_LIGHTS_BRT = 0;

		constexpr int XPLANE_GEAR_LEVER_UP = 0;
		constexpr int XPLANE_GEAR_LEVER_OFF = 1;
		constexpr int XPLANE_GEAR_LEVER_DN = 2;

		constexpr int AP_DISENGAGE_LIGHT_TEST_1 = 1;
		constexpr int AP_DISENGAGE_LIGHT_TEST_2 = -1;

			constexpr int XPLANE_LANDING_LIGHT_1_ON			= 1;
		constexpr int XPLANE_LANDING_LIGHT_1_OFF		= 0;
		constexpr int XPLANE_LANDING_LIGHT_2_ON			= 1;
		constexpr int XPLANE_LANDING_LIGHT_2_OFF		= 0;
		constexpr int XPLANE_LANDING_LIGHT_3_ON			= 1;
		constexpr int XPLANE_LANDING_LIGHT_3_OFF		= 0;
		constexpr int XPLANE_LANDING_LIGHT_4_ON			= 1;
		constexpr int XPLANE_LANDING_LIGHT_4_OFF		= 0;
		constexpr int XPLANE_RUNWAY_TURNOFF_LIGHT_L_ON	= 1;
		constexpr int XPLANE_RUNWAY_TURNOFF_LIGHT_L_OFF	= 0;
		constexpr int XPLANE_RUNWAY_TURNOFF_LIGHT_R_ON	= 1;
		constexpr int XPLANE_RUNWAY_TURNOFF_LIGHT_R_OFF	= 0;
		constexpr int XPLANE_TAXI_LIGHT_ON				= 2;
		constexpr int XPLANE_TAXI_LIGHT_OFF				= 0;


		constexpr int XPLANE_ELECTRICAL_BAT_ON = 2;		// requires two commands once's
		constexpr int XPLANE_ELECTRICAL_BAT_OFF = -1;   // requires two commands once's
		constexpr int XPLANE_APU_ON = 1;				// requires two commands once's
		constexpr int XPLANE_APU_OFF = 0;				// requires two commands once's
		constexpr int XPLANE_APU_START = 2;
		constexpr int XPLANE_IGNITION_SELECT_BOTH = 0;
		constexpr int XPLANE_IGNITION_SELECT_L = -1;
		constexpr int XPLANE_IGNITION_SELECT_R = 1;
		constexpr int XPLANE_FMC_SOURCE_SELECT_L = -1;
		constexpr int XPLANE_FMC_SOURCE_SELECT_NORMAL = 0;
		constexpr int XPLANE_FMC_SOURCE_SELECT_R = 1;

		constexpr int XPLANE_FUEL_CTR_R_PUMP_OFF = 0;
		constexpr int XPLANE_FUEL_CTR_R_PUMP_ON = 1;
		constexpr int XPLANE_FUEL_FWD_1_PUMP_ON = 1;
		constexpr int XPLANE_FUEL_FWD_1_PUMP_OFF = 0;
		constexpr int XPLANE_FUEL_AFT_1_PUMP_ON = 1;
		constexpr int XPLANE_FUEL_AFT_1_PUMP_OFF = 0;
		constexpr int XPLANE_FUEL_AFT_2_PUMP_OFF = 0;
		constexpr int XPLANE_FUEL_AFT_2_PUMP_ON = 1;
		constexpr int XPLANE_FLTCTRL_YAW_DAMPER_OFF = 0;
		constexpr int XPLANE_FLTCTRL_YAW_DAMPER_ON = 1;
		constexpr int XPLANE_ELECTRICAL_CAB_UTIL_ON = 1;
		constexpr int XPLANE_ELECTRICAL_CAB_UTIL_OFF = 0;
		constexpr int XPLANE_STANDBY_POWER_OFF = 0;
		constexpr int XPLANE_STANDBY_POWER_BAT = -1;
		constexpr int XPLANE_STANDBY_POWER_AUTO = 1;
		constexpr int XPLANE_GRD_PWR_DOWN = 1;
		constexpr int XPLANE_GRD_PWR_UP = -1;

	std::unique_ptr<HidInterfaceIT> InterfaceIT::hid = nullptr;

	InterfaceIT::InterfaceIT(AircraftModel& ac_model, std::unique_ptr<HidInterfaceIT> hid_interface_it) : aircraft_model(ac_model)
	{
		hid = std::move(hid_interface_it);

		// Initialize switch state to -1  i.e. need initial switch position 0 or 1
		//for (int& sw : InterfaceIT::overhead_previoussw_state)
		//{
		//	sw = -1;
		//}
		//for (int& sw : InterfaceIT::mip_previous_sw_state)
		//{
		//	sw = -1;
		//}

		//
		// Used to indicate switch that share OFF position
		// some switches are spring loaded to OFF so there is need
		// for extra processing because they will always initiialize to off/zero
		// Examples: AT/AP test, Fuel Flow used/reset, Elec Gnd Pwr, Gen 1/2 & Gen APU
		// Other switches don't use OFF, so those can be skipped
		// Eamples: Speed Ref (Auto, Vr,Wt,Vref...) and Auto Brakes 
		//
		// Overhead 
		//
		// FMC Left/Both/Right
		//overhead_multiway_sw[17].push_back(19);
		//overhead_multiway_sw[19].push_back(17);

		//// StandBy Power Auto/OFF/BAT
		//overhead_multiway_sw[45].push_back(44);
		//overhead_multiway_sw[44].push_back(45);

		//// APU
		//overhead_multiway_sw[8].push_back(9);
		//overhead_multiway_sw[9].push_back(8);

		//// ENG INGITERS Left/Both/Right
		//overhead_multiway_sw[14].push_back(12);
		//overhead_multiway_sw[12].push_back(14);

		////
		//// MIP
		////
		//// Gear DN OFF UP
		//mip_multiway_sw[31].push_back(28);
		//mip_multiway_sw[28].push_back(31);

		mip_zcockpit_switches[0]  = ZcockpitSwitch();										// no connection	0
		mip_zcockpit_switches[1]  = ZcockpitSwitch(DataRefName::spd_ref, mip_switches[1].type, 0);		// AUTO				1		
		mip_zcockpit_switches[2]  = ZcockpitSwitch(DataRefName::spd_ref, mip_switches[2].type, 0);		// VR				2
		mip_zcockpit_switches[3]  = ZcockpitSwitch(DataRefName::spd_ref, mip_switches[3].type, 0);		// WT				3
		mip_zcockpit_switches[4]  = ZcockpitSwitch(DataRefName::spd_ref, mip_switches[4].type, 0);		// VREF				4
		mip_zcockpit_switches[5]  = ZcockpitSwitch(DataRefName::spd_ref, mip_switches[5].type, 0);		// BUG5				5
		mip_zcockpit_switches[6]  = ZcockpitSwitch(DataRefName::spd_ref, mip_switches[6].type, 0);		// SET				6
		mip_zcockpit_switches[7]  = ZcockpitSwitch(DataRefName::spd_ref, mip_switches[7].type, 0);		// V1				7
		mip_zcockpit_switches[8]  = ZcockpitSwitch();					 							// no connection	8
		mip_zcockpit_switches[9]  = ZcockpitSwitch(DataRefName::autobrake_pos, mip_switches[9].type, 0);	// RTO				9
		mip_zcockpit_switches[10] = ZcockpitSwitch(DataRefName::autobrake_pos, mip_switches[10].type, 0);	// OFF				10
		mip_zcockpit_switches[11] = ZcockpitSwitch(DataRefName::autobrake_pos, mip_switches[11].type, 0);	// 1				11
		mip_zcockpit_switches[12] = ZcockpitSwitch(DataRefName::autobrake_pos, mip_switches[12].type, 0);	// 2				12
		mip_zcockpit_switches[13] = ZcockpitSwitch(DataRefName::autobrake_pos, mip_switches[13].type, 0);	// 3				13
		mip_zcockpit_switches[14] = ZcockpitSwitch(DataRefName::autobrake_pos, mip_switches[14].type, 0);	// MAX				14
	
		mip_zcockpit_switches[15] = ZcockpitSwitch(DataRefName::fuel_flow_pos, mip_switches[15].type, 0);						// FUEL_FLOW_USED				15
		mip_zcockpit_switches[16] = ZcockpitSwitch(DataRefName::fuel_flow_pos, mip_switches[16].type, 0);						// FUEL_FLOW_RESET				16
		mip_zcockpit_switches[17] = ZcockpitSwitch(DataRefName::master_caution1_pb, mip_switches[17].type, 0);			// MASTER_CAUTION				17
		mip_zcockpit_switches[18] = ZcockpitSwitch(DataRefName::capt_six_pack_pb, mip_switches[18].type, 0);				// SixPack ANNUNCIATOR			18
		mip_zcockpit_switches[19] = ZcockpitSwitch(DataRefName::chrono_capt_et_mode_pb, mip_switches[19].type, 0);	// CLOCK CAPT ET				19
		mip_zcockpit_switches[20] = ZcockpitSwitch(DataRefName::fire_bell_light1_pb, mip_switches[20].type, 1);			// FIRE WARN LIGHT				20
		mip_zcockpit_switches[21] = ZcockpitSwitch(DataRefName::bright_test, mip_switches[21].type, 0);							// Annun Bright/Test			21
		mip_zcockpit_switches[22] = ZcockpitSwitch();																								// STAB Out of Trim				22
		mip_zcockpit_switches[23] = ZcockpitSwitch(DataRefName::MFD_ENG_pb, mip_switches[23].type, 0);							// MFD_ENG						23
		mip_zcockpit_switches[24] = ZcockpitSwitch(DataRefName::MFD_SYS_pb, mip_switches[24].type, 0);							// MFD_SYS						24
		mip_zcockpit_switches[25] = ZcockpitSwitch(DataRefName::ap_light_pilot,  mip_switches[25].type, 0);				// AUTOPILOT_DIS_LIGHT			25
		mip_zcockpit_switches[26] = ZcockpitSwitch(DataRefName::at_light_pilot,  mip_switches[26].type, 0);				// AUTOTHROTTLE_DIS_LIGHT		26
		mip_zcockpit_switches[27] = ZcockpitSwitch(DataRefName::fms_light_pilot, mip_switches[27].type, 0);			// FMC_ALERT_LIGHT				27
		mip_zcockpit_switches[28] = ZcockpitSwitch(DataRefName::landing_gear, mip_switches[28].type, 0);						// GEAR_LEVER_UP				28
		mip_zcockpit_switches[29] = ZcockpitSwitch(DataRefName::ap_discon_test1, mip_switches[29].type, 0);					// Capt AP LIGHT TEST 2 (dn)	29
		mip_zcockpit_switches[30] = ZcockpitSwitch(DataRefName::ap_discon_test1, mip_switches[30].type, 0);					// Capt AP LIGHT TEST 1 (up)	30
		mip_zcockpit_switches[31] = ZcockpitSwitch(DataRefName::landing_gear, mip_switches[31].type, 0);						// GEAR_LEVER_DN				31
		mip_zcockpit_switches[32] = ZcockpitSwitch();																								//	{ -1, -1 } no connection	32


		overhead_zcockpit_switches[0]  = ZcockpitSwitch();												// no connection	 0
		overhead_zcockpit_switches[1]  = ZcockpitSwitch(DataRefName::land_lights_ret_left_pos,  overhead_switches[1].type, 0);			// LANDING_LIGHT_1								1
		overhead_zcockpit_switches[2]  = ZcockpitSwitch(DataRefName::land_lights_ret_right_pos, overhead_switches[2].type, 0);			// LANDING_LIGHT_2								2
		overhead_zcockpit_switches[3]  = ZcockpitSwitch(DataRefName::land_lights_left_pos,      overhead_switches[3].type, 0);			// LANDING_LIGHT_3								3
		overhead_zcockpit_switches[4]  = ZcockpitSwitch(DataRefName::land_lights_right_pos,     overhead_switches[4].type, 0);			// LANDING_LIGHT_4								4
		overhead_zcockpit_switches[5]  = ZcockpitSwitch(DataRefName::rwy_light_left, overhead_switches[5].type, 0);					// RUNWAY_TURNOFF_LIGHT_L						5
		overhead_zcockpit_switches[6]  = ZcockpitSwitch(DataRefName::rwy_light_right, overhead_switches[6].type, 0);					// RUNWAY_TURNOFF_LIGHT_R						6
		overhead_zcockpit_switches[7]  = ZcockpitSwitch(DataRefName::taxi_light_brightness_pos, overhead_switches[7].type, 0);		// TAXI_LIGHT									7


		overhead_zcockpit_switches[8]  = ZcockpitSwitch(DataRefName::APU_start_pos, overhead_switches[8].type, 0);								// APU_ON ==1, APU_OFF == 0	 -- toggle			8
		overhead_zcockpit_switches[9]  = ZcockpitSwitch(DataRefName::APU_start_pos, overhead_switches[9].type,XPLANE_APU_ON);		// APU_ON ==1, APU_START ==2 -- string loaded	9

		overhead_zcockpit_switches[10] = ZcockpitSwitch(DataRefName::hydro_pumps2_pos, overhead_switches[10].type, 0);				// HYDRAULIC_ENG_PUMP_2							10
		overhead_zcockpit_switches[11] = ZcockpitSwitch(DataRefName::electric_hydro_pumps1_pos, overhead_switches[11].type, 0);	// HYDRAULIC_ELECTRIC_PUMP_1					11
		overhead_zcockpit_switches[12] = ZcockpitSwitch(DataRefName::eng_start_source, overhead_switches[12].type, 0);				// XPLANE_IGNITION_SELECT_BOTH=0, IGNITION_SELECT_2=1	12
		overhead_zcockpit_switches[13] = ZcockpitSwitch(DataRefName::electric_hydro_pumps2_pos, overhead_switches[13].type, 0);	// HYDRAULIC_ELECTRIC_PUMP_2					13
		overhead_zcockpit_switches[14] = ZcockpitSwitch(DataRefName::eng_start_source, overhead_switches[14].type, 0);				// XPLANE_IGNITION_SELECT_BOTH =0 IGNITION_SELECT_1=-1 14
		overhead_zcockpit_switches[15] = ZcockpitSwitch(DataRefName::hydro_pumps1_pos, overhead_switches[15].type, 0);				// HYDRAULIC_ENG_PUMP_1							15
		overhead_zcockpit_switches[16] = ZcockpitSwitch();												// no connection	 16
		overhead_zcockpit_switches[17] = ZcockpitSwitch(DataRefName::fmc_source, overhead_switches[17].type, 0);						// FMC_SOURCE_SELECT_NORMAL=0, FMC_SOURCE_L=-1	17
		overhead_zcockpit_switches[18] = ZcockpitSwitch(DataRefName::logo_light, overhead_switches[18].type, 0);						//LOGO_LIGHT									18
		overhead_zcockpit_switches[19] = ZcockpitSwitch(DataRefName::fmc_source, overhead_switches[19].type, 0);						// FMC_SOURCE_SELECT_NORMAL=0 FMC_SOURCE_R=1	19
		overhead_zcockpit_switches[20] = ZcockpitSwitch(DataRefName::beacon_on, overhead_switches[20].type, 0);						// ANTI_COLLISION_LIGHT -- no COMMAND			20      ************************
		overhead_zcockpit_switches[21] = ZcockpitSwitch(DataRefName::wing_light, overhead_switches[21].type, 0);						// WING_LIGHT on=1 off=0						21
		overhead_zcockpit_switches[22] = ZcockpitSwitch(DataRefName::wheel_light, overhead_switches[22].type, 0);						// WHEEL_WELL_LIGHT on=1 off=0					22

		overhead_zcockpit_switches[23] = ZcockpitSwitch(DataRefName::attend, overhead_switches[23].type, 0);							// attend Call									23
		overhead_zcockpit_switches[24] = ZcockpitSwitch(DataRefName::grd_call, overhead_switches[24].type, 0);							// ground Call									24
		
		overhead_zcockpit_switches[25] = ZcockpitSwitch(DataRefName::eq_cool_supply, overhead_switches[25].type, 0);					// COOLING_SUPPLY_ALTN=1 COOLING_SUPPLY_NORM=0	25
		overhead_zcockpit_switches[26] = ZcockpitSwitch(DataRefName::eq_cool_exhaust, overhead_switches[26].type, 0);				// COOLING_EXHAUST_ALTN=1 COOLING_EXHST_NORM=0	26
		overhead_zcockpit_switches[27] = ZcockpitSwitch(DataRefName::emer_exit_lights, overhead_switches[27].type, 0);				// EMER_LIGHT_ARMED==1, EMER_LIGHT_ON=2			27
		overhead_zcockpit_switches[28] = ZcockpitSwitch(DataRefName::emer_exit_lights, overhead_switches[28].type, 0);				// EMER_LIGHT_ARMED=1, EMER_LIGHT_OFF=0			28
		overhead_zcockpit_switches[29] = ZcockpitSwitch(DataRefName::seatbelt_sign_pos, overhead_switches[29].type, 0);				// FASTEN_BELTS_AUTO=1, FASTEN_BELTS_ON=2		29
		overhead_zcockpit_switches[30] = ZcockpitSwitch(DataRefName::seatbelt_sign_pos, overhead_switches[30].type, 0);				// FASTEN_BELTS_AUTO=1, FASTEN_BELTS_OFF=0		30
		overhead_zcockpit_switches[31] = ZcockpitSwitch(DataRefName::no_smoking_pos, overhead_switches[31].type, 0);					// NO_SMOKING_AUTO=1, NO_SMOKING_ON=2			31
		overhead_zcockpit_switches[32] = ZcockpitSwitch(DataRefName::no_smoking_pos, overhead_switches[32].type, 0);					// NO_SMOKING_AUTO=1, NO_SMOKING_OFF=0			32
		overhead_zcockpit_switches[33] = ZcockpitSwitch(DataRefName::cross_feed_pos, overhead_switches[33].type, 0);					// CROSSFEED_ON=1, CROSSFEED_OFF=0				33
		overhead_zcockpit_switches[34] = ZcockpitSwitch(DataRefName::fuel_tank_pos_rgt2, overhead_switches[34].type, 0);				// FUEL_FWD_2									34
		overhead_zcockpit_switches[35] = ZcockpitSwitch(DataRefName::fuel_tank_pos_ctr1, overhead_switches[35].type, 0);				// FUEL_CTR_L									35
		overhead_zcockpit_switches[36] = ZcockpitSwitch();												// no connection	 36
		overhead_zcockpit_switches[37] = ZcockpitSwitch(DataRefName::fuel_tank_pos_ctr2, overhead_switches[37].type, 0);				// FUEL_CTR_R_									37
		overhead_zcockpit_switches[38] = ZcockpitSwitch(DataRefName::fuel_tank_pos_lft2, overhead_switches[38].type, 0);				// FUEL_FWD_1									38
		overhead_zcockpit_switches[39] = ZcockpitSwitch(DataRefName::fuel_tank_pos_lft1, overhead_switches[39].type, 0);				// FUEL_AFT_1									39
		overhead_zcockpit_switches[40] = ZcockpitSwitch(DataRefName::fuel_tank_pos_rgt1, overhead_switches[40].type, 0);				// FUEL_AFT_2									40
		overhead_zcockpit_switches[41] = ZcockpitSwitch(DataRefName::yaw_dumper_pos, overhead_switches[41].type, 0);					// YAW_DAMPER									41
		overhead_zcockpit_switches[42] = ZcockpitSwitch(DataRefName::cab_util_pos, overhead_switches[42].type, 0);					// CAB_UTIL										42
		overhead_zcockpit_switches[43] = ZcockpitSwitch(DataRefName::battery_pos, overhead_switches[43].type, 0);						// BAT_ON, BAT_OFF								43
		overhead_zcockpit_switches[44] = ZcockpitSwitch(DataRefName::standby_bat_pos, overhead_switches[44].type, 0);				// STANDBY_POWER_OFF=0, STANDBY_POWER_BAT=-1	44
		overhead_zcockpit_switches[45] = ZcockpitSwitch(DataRefName::standby_bat_pos, overhead_switches[45].type, 0);				// STANDBY_POWER_OFF=0, STANDBY_POWER_AUTO=1	45
		overhead_zcockpit_switches[46] = ZcockpitSwitch(DataRefName::gpu_pos, overhead_switches[46].type, 0);						// Center=0 GRD_PWR_DOWN=1						46 
		overhead_zcockpit_switches[47] = ZcockpitSwitch(DataRefName::gpu_pos, overhead_switches[47].type, 0);						// Center=0 GRD_PWR_UP=-1						47

		// ***** overhead_zcockpit_switches[48] = ZcockpitSwitch(DataRefName::, overhead_switches[].type  );//{ KEY_COMMAND_ELECTRICAL_BUS_TRANSFER_AUTO, KEY_COMMAND_ELECTRICAL_BUS_TRANSFER_OFF }, // 48

		overhead_zcockpit_switches[49] = ZcockpitSwitch(DataRefName::gen2_pos, overhead_switches[49].type, 0);						// GENERATOR_2_DOWN	=1									49
		overhead_zcockpit_switches[50] = ZcockpitSwitch(DataRefName::gen2_pos, overhead_switches[50].type, 0);						// GENERATOR_2_UP = -1									50
		overhead_zcockpit_switches[51] = ZcockpitSwitch(DataRefName::apu_gen2_pos, overhead_switches[51].type, 0);					// APU_GENERATOR_2_DOWN									51
		overhead_zcockpit_switches[52] = ZcockpitSwitch(DataRefName::apu_gen2_pos, overhead_switches[52].type, 0);					// APU_GENERATOR_2_UP									52
		overhead_zcockpit_switches[53] = ZcockpitSwitch(DataRefName::apu_gen1_pos, overhead_switches[53].type, 0);					// APU_GENERATOR_1_DOWN =1								53
		overhead_zcockpit_switches[54] = ZcockpitSwitch(DataRefName::apu_gen1_pos, overhead_switches[54].type, 0);					// APU_GENERATOR_1_UP = -1								54
		overhead_zcockpit_switches[55] = ZcockpitSwitch(DataRefName::gen1_pos, overhead_switches[55].type, 0);						// GENERATOR_1_DOWN	=1									55
		overhead_zcockpit_switches[56] = ZcockpitSwitch(DataRefName::gen1_pos, overhead_switches[56].type, 0);							// GENERATOR_1_UP = -1									56
		overhead_zcockpit_switches[57] = ZcockpitSwitch(DataRefName::window_heat_l_side_pos, overhead_switches[57].type, 0);			// L_SIDE_WINDOW_HEAT_ON=1, L_SIDE_WINDOW_HEAT_OFF=0	57
		overhead_zcockpit_switches[58] = ZcockpitSwitch(DataRefName::window_heat_l_fwd_pos, overhead_switches[58].type, 0);			// L_FWD_WINDOW_HEAT_ON=1, L_FWD_WINDOW_HEAT_OFF=0		58
		overhead_zcockpit_switches[59] = ZcockpitSwitch(DataRefName::window_ovht_test, overhead_switches[59].type, 0);				// WINDOW_HEAT_TEST_UP= -1								59
		overhead_zcockpit_switches[60] = ZcockpitSwitch(DataRefName::window_heat_r_fwd_pos, overhead_switches[60].type, 0);			// R_FWD_WINDOW_HEAT_ON=1, R_FWD_WINDOW_HEAT_OFF=0		60	
		overhead_zcockpit_switches[61] = ZcockpitSwitch(DataRefName::window_heat_r_side_pos, overhead_switches[61].type, 0);			// R_SIDE_WINDOW_HEAT_ON=1, R_SIDE_WINDOW_HEAT_OFF=0	61
		overhead_zcockpit_switches[62] = ZcockpitSwitch(DataRefName::fo_probes_pos, overhead_switches[62].type, 0);					// PROBE_B_HEAT_ON=1, PROBE_B_HEAT_OFF=0				62
		overhead_zcockpit_switches[63] = ZcockpitSwitch(DataRefName::capt_probes_pos, overhead_switches[63].type, 0);				// PROBE_A_HEAT_ON=1, PROBE_A_HEAT_OFF=0				63
		overhead_zcockpit_switches[64] = ZcockpitSwitch(DataRefName::window_ovht_test, overhead_switches[64].type, 0);				// WINDOW_HEAT_TEST_DOWN=1								64



	}



		const InterfaceITSwitch InterfaceIT::mip_switches[] = {
			{common::SwitchType::unused, 0, 0 }, // 0

			{common::SwitchType::rotary_2_commands, 0, XPLANE_SPD_REF_MODE_AUTO}, // 1

			{common::SwitchType::rotary_2_commands, 0, XPLANE_SPD_REF_MODE_VR }, // 2
			{common::SwitchType::rotary_2_commands, 0, XPLANE_SPD_REF_MODE_WT }, // 3
			{common::SwitchType::rotary_2_commands, 0, XPLANE_SPD_REF_MODE_VREF }, // 4
			{common::SwitchType::rotary_2_commands, 0, XPLANE_SPD_REF_MODE_BUG5 }, // 5
			{common::SwitchType::rotary_2_commands, 0, XPLANE_SPD_REF_MODE_SET }, // 6
			{common::SwitchType::rotary_2_commands, 0, XPLANE_SPD_REF_MODE_V1 }, // 7

			{common::SwitchType::unused, 0, 0}, // 8

			{common::SwitchType::rotary_multi_commands, 0, XPLANE_AUTO_BRAKE_RTO }, // 9
			{common::SwitchType::rotary_multi_commands, 0, XPLANE_AUTO_BRAKE_OFF }, // 10
			{common::SwitchType::rotary_multi_commands, 0, XPLANE_AUTO_BRAKE_1 }, // 11

			{common::SwitchType::rotary_multi_commands, 0, XPLANE_AUTO_BRAKE_2 }, // 12
			{common::SwitchType::rotary_multi_commands, 0, XPLANE_AUTO_BRAKE_3 }, // 13
			{common::SwitchType::rotary_multi_commands, 0, XPLANE_AUTO_BRAKE_MAX }, // 14

			{common::SwitchType::spring_loaded, XPLANE_FUEL_FLOW_RATE, XPLANE_FUEL_FLOW_USED }, // 15
			{common::SwitchType::spring_loaded, XPLANE_FUEL_FLOW_RATE, XPLANE_FUEL_FLOW_RESET }, // 16

			{common::SwitchType::pushbutton, 0, 1}, // XPLANE_WARNING_MASTER_CAUTION }, // 17
			{common::SwitchType::pushbutton, 0, 1}, // XPLANE_WARNING_SYSTEM_ANNUNCIATOR }, // 18
			{common::SwitchType::pushbutton, 0, 1}, // XPLANE_INSTRUMENT_CLOCK_CAPT_CHR }, // 19
			{common::SwitchType::spring_loaded, 0, 1}, // XPLANE_FIRE_MASTER_FIRE_WARN_LIGHT }, // 20

			{common::SwitchType::toggle, XPLANE_MASTER_LIGHTS_BRT, XPLANE_MASTER_LIGHTS_TEST }, // 21   NOTE  DIM is not wired
			{common::SwitchType::unused, 0, 0 }, //STAB Out of Trim	 22
			{common::SwitchType::pushbutton, 0, 1}, // KEY_COMMAND_ENGAPU_MFD_ENG }, // 23
			{common::SwitchType::pushbutton, 0, 1}, // KEY_COMMAND_ENGAPU_MFD_SYS }, // 24
			{common::SwitchType::spring_loaded, 0, 1}, // KEY_COMMAND_AUTOMATICFLIGHT_AUTOPILOT_DIS_LIGHT }, // 25
			{common::SwitchType::spring_loaded, 0, 1}, // KEY_COMMAND_AUTOMATICFLIGHT_AUTOTHROTTLE_DIS_LIGHT }, // 26
			{common::SwitchType::spring_loaded, 0, 1}, // KEY_COMMAND_FMS_FMC_ALERT_LIGHT }, // 27
			{common::SwitchType::rotary_multi_commands, XPLANE_GEAR_LEVER_OFF, XPLANE_GEAR_LEVER_UP }, // 28
			{common::SwitchType::spring_loaded, 0, AP_DISENGAGE_LIGHT_TEST_2}, // KEY_COMMAND_AUTOMATICFLIGHT_DISENGAGE_LIGHT_TEST_2 }, // 29
			{common::SwitchType::spring_loaded, 0, AP_DISENGAGE_LIGHT_TEST_1}, // KEY_COMMAND_AUTOMATICFLIGHT_DISENGAGE_LIGHT_TEST_1 }, // 30
			{common::SwitchType::rotary_multi_commands, XPLANE_GEAR_LEVER_OFF, XPLANE_GEAR_LEVER_DN}, // 31
			{common::SwitchType::unused, 0, 0 },  // 0 no connection   32
		};


		const InterfaceITSwitch InterfaceIT::overhead_switches[] = {
		{ common::SwitchType::unused, 0, 0 }, // 0
		{ common::SwitchType::toggle,  XPLANE_LANDING_LIGHT_1_ON, XPLANE_LANDING_LIGHT_1_OFF }, // 1
		{ common::SwitchType::toggle,  XPLANE_LANDING_LIGHT_2_ON, XPLANE_LANDING_LIGHT_2_OFF }, // 2
		{ common::SwitchType::toggle,  XPLANE_LANDING_LIGHT_3_ON, XPLANE_LANDING_LIGHT_3_OFF }, // 3
		{ common::SwitchType::toggle,  XPLANE_LANDING_LIGHT_4_ON, XPLANE_LANDING_LIGHT_4_OFF }, // 4
		{ common::SwitchType::toggle,  XPLANE_RUNWAY_TURNOFF_LIGHT_L_ON, XPLANE_RUNWAY_TURNOFF_LIGHT_L_OFF }, // 5
		{ common::SwitchType::toggle,  XPLANE_RUNWAY_TURNOFF_LIGHT_R_ON, XPLANE_RUNWAY_TURNOFF_LIGHT_R_OFF }, // 6
		{ common::SwitchType::toggle,  XPLANE_TAXI_LIGHT_ON, XPLANE_TAXI_LIGHT_OFF }, // 7

		{ common::SwitchType::toggle, XPLANE_APU_ON, XPLANE_APU_OFF },			// XPLANE_APU_ON, XPLANE_APU_OFF	8
		{ common::SwitchType::spring_loaded, XPLANE_APU_ON, XPLANE_APU_START },	// XPLANE_APU_ON, XPLANE_APU_START	9
		  
		{ common::SwitchType::toggle, 1, 0 },  //{ KEY_COMMAND_HYDRAULIC_ENG_PUMP_2_ON, KEY_COMMAND_HYDRAULIC_ENG_PUMP_2_OFF }, //10
		{ common::SwitchType::toggle, 1, 0 },  //{ KEY_COMMAND_HYDRAULIC_ELECTRIC_PUMP_1_ON, KEY_COMMAND_HYDRAULIC_ELECTRIC_PUMP_1_OFF }, // 11
		{ common::SwitchType::toggle, XPLANE_IGNITION_SELECT_BOTH, XPLANE_IGNITION_SELECT_R },  //{ XPLANE_IGNITION_SELECT_BOTH, XPLANE_IGNITION_SELECT_R }, // 12
		{ common::SwitchType::toggle, 1, 0 },  //{ KEY_COMMAND_HYDRAULIC_ELECTRIC_PUMP_2_ON, KEY_COMMAND_HYDRAULIC_ELECTRIC_PUMP_2_OFF }, // 13
		{ common::SwitchType::toggle, XPLANE_IGNITION_SELECT_BOTH, XPLANE_IGNITION_SELECT_L },  //{ XPLANE_IGNITION_SELECT_BOTH, XPLANE_IGNITION_SELECT_L }, // 14
		{ common::SwitchType::toggle, 1, 0 },  //{ KEY_COMMAND_HYDRAULIC_ENG_PUMP_1_ON, KEY_COMMAND_HYDRAULIC_ENG_PUMP_1_OFF }, // 15
 
		{ common::SwitchType::unused, 0, 0 }, // 16

		{ common::SwitchType::toggle, XPLANE_FMC_SOURCE_SELECT_NORMAL, XPLANE_FMC_SOURCE_SELECT_L },  //{ XPLANE_FMC_SOURCE_SELECT_NORMAL, XPLANE_FMC_SOURCE_SELECT_L }, // 17
		{ common::SwitchType::toggle, 1, 0 },  //{ XPLANE_LOGO_LIGHT_ON, XPLANE_LOGO_LIGHT_OFF }, // 18
		{ common::SwitchType::toggle, XPLANE_FMC_SOURCE_SELECT_NORMAL, XPLANE_FMC_SOURCE_SELECT_R },  //{ XPLANE_FMC_SOURCE_SELECT_NORMAL, XPLANE_FMC_SOURCE_SELECT_R }, // 19
		{ common::SwitchType::toggle, 1, 0 },  //{ XPLANE_ANTI_COLLISION_LIGHT_ON, XPLANE_ANTI_COLLISION_LIGHT_OFF }, // 20
		{ common::SwitchType::toggle, 1, 0 },  //{ XPLANE_WING_LIGHT_ON, XPLANE_WING_LIGHT_OFF }, // 21
		{ common::SwitchType::toggle, 1, 0 },  //{ XPLANE_WHEEL_WELL_LIGHT_ON, XPLANE_WHEEL_WELL_LIGHT_OFF }, // 22
  
		{ common::SwitchType::pushbutton, 0, 1 }, // attend Call																				   23
		{ common::SwitchType::spring_loaded, 1, 0 }, // ground Call																				   24

		{ common::SwitchType::pushbutton, 1, 0 },  //{ KEY_COMMAND_AIRSYSTEM_COOLING_SUPPLY_ALTN, KEY_COMMAND_AIRSYSTEM_COOLING_SUPPLY_NORM }, // 25
		{ common::SwitchType::pushbutton, 1, 0 },  //{ KEY_COMMAND_AIRSYSTEM_COOLING_EXHAUST_ALTN, KEY_COMMAND_AIRSYSTEM_COOLING_EXHAUST_NORM }, // 26
  
		{ common::SwitchType::toggle, 1, 2 },  //{ XPLANE_EMER_LIGHT_ARMED, XPLANE_EMER_LIGHT_ON }, // 27
		{ common::SwitchType::toggle, 1, 0 },  //{ XPLANE_EMER_LIGHT_ARMED, XPLANE_EMER_LIGHT_OFF }, // 28
 
		{ common::SwitchType::toggle, 1, 2 },  //{ XPLANE_FASTEN_BELTS_AUTO, XPLANE_FASTEN_BELTS_ON }, // 29
		{ common::SwitchType::toggle, 1, 0 },  //{ XPLANE_FASTEN_BELTS_AUTO, XPLANE_FASTEN_BELTS_OFF }, // 30
		{ common::SwitchType::toggle, 1, 2 },  //{ XPLANE_NO_SMOKING_AUTO, XPLANE_NO_SMOKING_ON }, // 31
		{ common::SwitchType::toggle, 1, 0 },  //{ XPLANE_NO_SMOKING_AUTO, XPLANE_NO_SMOKING_OFF }, // 32
		{ common::SwitchType::toggle, 1, 0 },  //{ KEY_COMMAND_FUEL_CROSSFEED_ON, KEY_COMMAND_FUEL_CROSSFEED_OFF }, // 33
		{ common::SwitchType::toggle, 0, 1 },  //{ KEY_COMMAND_FUEL_FWD_2_PUMP_OFF, KEY_COMMAND_FUEL_FWD_2_PUMP_ON }, // 34
		{ common::SwitchType::toggle, 0, 1 },  //{ KEY_COMMAND_FUEL_CTR_L_PUMP_OFF, KEY_COMMAND_FUEL_CTR_L_PUMP_ON }, // 35
 
		{ common::SwitchType::unused, 0, 0 }, // 36
 
		{ common::SwitchType::toggle,	XPLANE_FUEL_CTR_R_PUMP_OFF,						XPLANE_FUEL_CTR_R_PUMP_ON },	// XPLANE_FUEL_CTR_R_PUMP_OFF,		XPLANE_FUEL_CTR_R_PUMP_ON			37
		{ common::SwitchType::toggle,	XPLANE_FUEL_FWD_1_PUMP_ON,		XPLANE_FUEL_FWD_1_PUMP_OFF },	// XPLANE_FUEL_FWD_1_PUMP_ON,		XPLANE_FUEL_FWD_1_PUMP_OFF			38
		{ common::SwitchType::toggle,	XPLANE_FUEL_AFT_1_PUMP_ON,		XPLANE_FUEL_AFT_1_PUMP_OFF },	// XPLANE_FUEL_AFT_1_PUMP_ON,		XPLANE_FUEL_AFT_1_PUMP_OFF			39
		{ common::SwitchType::toggle,	XPLANE_FUEL_AFT_2_PUMP_OFF,						XPLANE_FUEL_AFT_2_PUMP_ON },	// XPLANE_FUEL_AFT_2_PUMP_OFF,		XPLANE_FUEL_AFT_2_PUMP_ON			40
		{ common::SwitchType::toggle,	XPLANE_FLTCTRL_YAW_DAMPER_OFF,					XPLANE_FLTCTRL_YAW_DAMPER_ON },	// XPLANE_FLTCTRL_YAW_DAMPER_OFF,	XPLANE_FLTCTRL_YAW_DAMPER_ON		41
		{ common::SwitchType::toggle,	XPLANE_ELECTRICAL_CAB_UTIL_ON,	XPLANE_ELECTRICAL_CAB_UTIL_OFF },// XPLANE_ELECTRICAL_CAB_UTIL_ON,	XPLANE_ELECTRICAL_CAB_UTIL_OFF		42

		{ common::SwitchType::toggle,		XPLANE_ELECTRICAL_BAT_ON, XPLANE_ELECTRICAL_BAT_OFF }, // 43
  
		{ common::SwitchType::toggle, XPLANE_STANDBY_POWER_OFF, XPLANE_STANDBY_POWER_BAT},  //{ XPLANE_STANDBY_POWER_OFF, XPLANE_STANDBY_POWER_BAT }, //44
		{ common::SwitchType::toggle, XPLANE_STANDBY_POWER_OFF, XPLANE_STANDBY_POWER_AUTO},  //{ XPLANE_STANDBY_POWER_OFF, XPLANE_STANDBY_POWER_AUTO }, //45
		{ common::SwitchType::spring_loaded, 0, XPLANE_GRD_PWR_UP },  //{ -1,XPLANE_GRD_PWR_DOWN }, // 46
		{ common::SwitchType::spring_loaded, 0, XPLANE_GRD_PWR_DOWN},  //{ -1,XPLANE_GRD_PWR_UP }, // 47
		{ common::SwitchType::toggle, 1, 0 },  //{ KEY_COMMAND_ELECTRICAL_BUS_TRANSFER_AUTO, KEY_COMMAND_ELECTRICAL_BUS_TRANSFER_OFF }, // 48  **** NO XPLANE COMMAND/DATAREF!!!!!
  
		{ common::SwitchType::spring_loaded, 0, -1 },  //{ -1, XPLANE_ELECTRICAL_GENERATOR_2_DOWN }, // 49
		{ common::SwitchType::spring_loaded, 0, 1 },  //{ -1, XPLANE_ELECTRICAL_GENERATOR_2_UP }, // 50
		{ common::SwitchType::spring_loaded, 0, -1 },  //{ -1, XPLANE_ELECTRICAL_APU_GENERATOR_2_DOWN }, // 51
		{ common::SwitchType::spring_loaded, 0, 1 },  //{ -1, XPLANE_ELECTRICAL_APU_GENERATOR_2_UP }, // 52
		{ common::SwitchType::spring_loaded, 0, -1 },  //{ -1, XPLANE_ELECTRICAL_APU_GENERATOR_1_DOWN }, // 53
		{ common::SwitchType::spring_loaded, 0, 1 },  //{ -1, XPLANE_ELECTRICAL_APU_GENERATOR_1_UP }, // 54
		{ common::SwitchType::spring_loaded, 0, -1 },  //{ -1, XPLANE_ELECTRICAL_GENERATOR_1_DOWN }, // 55
		{ common::SwitchType::spring_loaded, 0, 1 },  //{ -1, XPLANE_ELECTRICAL_GENERATOR_1_UP }, // 56
		{ common::SwitchType::toggle, 1, 0 },  //{ XPLANE_ANTIICE_LEFT_SIDE_WINDOW_HEAT_ON, KEY_COMMAND_ANTIICE_LEFT_SIDE_WINDOW_HEAT_OFF }, // 57
		{ common::SwitchType::toggle, 1, 0 },  //{ XPLANE_ANTIICE_LEFT_FWD_WINDOW_HEAT_ON, KEY_COMMAND_ANTIICE_LEFT_FWD_WINDOW_HEAT_OFF }, // 58
		{ common::SwitchType::spring_loaded, 0, 1 },  //{ -1, XPLANE_ANTIICE_WINDOW_HEAT_TEST_UP }, // 59
		{ common::SwitchType::toggle, 1, 0 },  //{ XPLANE_ANTIICE_RIGHT_FWD_WINDOW_HEAT_ON, KEY_COMMAND_ANTIICE_RIGHT_FWD_WINDOW_HEAT_OFF }, // 60	
		{ common::SwitchType::toggle, 1, 0 },  //{ XPLANE_ANTIICE_RIGHT_SIDE_WINDOW_HEAT_ON, KEY_COMMAND_ANTIICE_RIGHT_SIDE_WINDOW_HEAT_OFF }, // 61
		{ common::SwitchType::toggle, 1, 0 },  //{ XPLANE_ANTIICE_PROBE_B_HEAT_ON, KEY_COMMAND_ANTIICE_PROBE_B_HEAT_OFF }, // 62
		{ common::SwitchType::toggle, 1, 0 },  //{ XPLANE_ANTIICE_PROBE_A_HEAT_ON, KEY_COMMAND_ANTIICE_PROBE_A_HEAT_OFF }, // 63
		{ common::SwitchType::spring_loaded, 0, -1 },  //{ -1, XPLANE_ANTIICE_WINDOW_HEAT_TEST_DOWN }, // 64
		};


	void InterfaceIT::turnOffMipLEDs() const
	{
		for (int i = 1; i < 65; i++)
		{
			hid->pLED_Set(mipSession, i, 0);
		}
	}

	void InterfaceIT::turnOnMipLEDs() const
	{
		const int mipLeds[] = {
			1, // WARN_annunMASTER_CAUTION[0]
			2, // WARN_annunELEC
			3, // WARN_annunFLT_CONT
			4, // WARN_annunFUEL
			5, // WARN_annunOVHT_DET
			6, // WARN_annunMASTER_CAUTION[0] // second light
			7, // WARN_annunIRS
			8, // WARN_annunAPU
			9, // MAIN_annun_AP_RED[0]) // Red
			10, // MAIN_annun_AP_AMBER[0]) // Amber
			11, // MAIN_annun_AT_RED[0]) // Red
			12, // MAIN_annun_AT_AMBER[0]) // Amber
			13, // MAIN_annun_FMC_AMBER[0]
			14, // CABIN_ALTITUDE_Light_Status
			15, // TAKEOFF_CONFIG_Light_Status
			16, // MAIN_annunBELOW_GS[0]
			17, // MAIN_annunSPEEDBRAKE_ARMED
			18, // MAIN_annunSPEEDBRAKE_DO_NOT_ARM
			19, // MAIN_annunANTI_SKID_INOP
			20, // MAIN_annunSTAB_OUT_OF_TRIM
			21, // MAIN_annunAUTO_BRAKE_DISARM
			22, // MAIN_annunGEAR_locked[0]
			23, // MAIN_annunLE_FLAPS_TRANSIT
			24, // MAIN_annunLE_FLAPS_EXT
			25, // MAIN_annunGEAR_transit[1]
			26, // TAKEOFF_CONFIG_Light_Status
			27, // MAIN_annunGEAR_transit[0]
			28, // MAIN_annunGEAR_locked[2]
			29, // MAIN_annunGEAR_locked[1]
			30, // MAIN_annunGEAR_transit[2]
			31, // CABIN_ALTITUDE_Light_Status
			// 32
			33, // MAIN_annunSPEEDBRAKE_EXTENDED
			34, // MAIN_annun_AT_AMBER[1]) // Amber
			35, // MAIN_annun_AP_AMBER[1]) // Amber
			// 36
			37, // MAIN_annunBELOW_GS[1]
			38, // MAIN_annun_AP_RED[1]) // Red
			39, // MAIN_annun_AT_RED[1]) // Red
			40, // MAIN_annun_FMC_AMBER[1]
			41, // WARN_annunMASTER_CAUTION[1]
			42, // WARN_annunHYD
			43, // WARN_annunOVERHEAD
			44, // WARN_annunAIR_COND
			45, // WARN_annunFIRE_WARN[1]
			46, // WARN_annunENG
			47, // WARN_annunANTI_ICE
			48, // WARN_annunDOORS
			49, //  runway INOP
			50, // GPWS_annunINOP
			// 51,
			// 52,
			// 53,
			// 54,
			// 55,
			// 56,
			// 57,
			58, // WARN_annunFIRE_WARN[0]
			// 59,
			// 60,
			// 61,
			// 62,
			// 63,
			// 64,
		};
		for (auto led : mipLeds)
		{
			hid->pLED_Set(mipSession, led, 1);
		}
	}

	void InterfaceIT::turnOffOverheadLEDs() const
	{
		for (int i = 2; i < 211; i++)
		{
			hid->pLED_Set(overHeadSession, i, 0);
		}
	}

	void InterfaceIT::turnOnOverheadLEDs() const
	{
		const int overheadLeds[] =
		{// skip CALL 1
			2, // FCTL_annunLOW_QUANTITY
			3, // FCTL_annunLOW_STBY_RUD_ON
			4, // FCTL_annunFEEL_DIFF_PRESS
			5, // FCTL_annunSPEED_TRIM_FAIL
			6, // FCTL_annunMACH_TRIM_FAIL
			7, // FCTL_annunAUTO_SLAT_FAIL
			8, // FCTL_annunYAW_DAMPER
			9, // ENG_VALVE_CLOSED_Light_1_Status // Bright
			//10, // FUEL_annunENG_VALVE_CLOSED[0] // Dim
			11, // FCTL_annunFC_LOW_PRESSURE[0]
			12, // AIR_annunEquipCoolingSupplyOFF
			13, // AIR_annunEquipCoolingExhaustOFF
			14, // LTS_annunEmerNOT_ARMED
			15, // FCTL_annunFC_LOW_PRESSURE[1]
			// skip 16
			17, //ENG_VALVE_CLOSED_Light_2_Status
			//18, // FUEL_annunENG_VALVE_CLOSED[1]
			19, // FUEL_annunFILTER_BYPASS[1]
			20, // FUEL_annunFILTER_BYPASS[0]
			21, // FUEL_annunLOWPRESS_Ctr[1]
			22, // FUEL_annunLOWPRESS_Ctr[0]
			23, // FUEL_annunLOWPRESS_Aft[0]
			24, // FUEL_annunLOWPRESS_Fwd[0]
			//skip 25,
			//skip 26,
			// 27, aircraft_model.z737InData.FUEL_annunLOWPRESS_Fwd[1]
			// 28, aircraft_model.z737InData.FUEL_annunLOWPRESS_Aft[1]
			29, // ELEC_annunBAT_DISCHARGE
			30, // ELEC_annunTR_UNIT
			31, // ELEC_annunELEC
			// skip 32
			 33, // SPAR_VALVE_CLOSED_Light_1_Status // Bright
			//34, // FUEL_annunSPAR_VALVE_CLOSED[0] // Dim
			35, // ELEC_annunDRIVE[0]
			36, // ELEC_annunSTANDBY_POWER_OFF
			37, // ELEC_annunDRIVE[1]
			// skip 38
			// skip 39
			// skip 40
			41, // SPAR_VALVE_CLOSED_Light_2_Status // Bright
			//42, // FUEL_annunSPAR_VALVE_CLOSED[1] // Dim
			// skip 43
			// skip 44
			// skip 45
			// skip 46
			// skip 47
			// skip 48
			49, // APU_annunMAINT
			// skip 50
			51, // APU_annunLOW_OIL_PRESSURE
			52, // APU_annunFAULT
			53, // APU_annunOVERSPEED
			54, // ELEC_annunSOURCE_OFF[1]
			55, // ELEC_annunTRANSFER_BUS_OFF[1]
			56, // ELEC_annunTRANSFER_BUS_OFF[0]
			57, // ELEC_annunGEN_BUS_OFF[0]
			// skip 58
			59, // ELEC_annunSOURCE_OFF[0]
			// skip 60
			// skip 61
			// skip 62
			// skip 63
			// skip 64
			65, // ELEC_annunGEN_BUS_OFF[1]
			// skip 66
			67, // HYD_annunLOW_PRESS_eng[0]
			68, // HYD_annunLOW_PRESS_elec[1]
			69, // HYD_annunLOW_PRESS_elec[0]
			70, // HYD_annunLOW_PRESS_eng[1]
			71, // HYD_annunOVERHEAT_elec[0]
			72, // HYD_annunOVERHEAT_elec[1]
			73, // ELEC_annunAPU_GEN_OFF_BUS
			// skip 74
			75, // DOOR_annunFWD_ENTRY
			76, // DOOR_annunFWD_SERVICE
			77, // DOOR_annunFWD_CARGO
			78, // DOOR_annunRIGHT_FWD_OVERWING
			79, // DOOR_annunLEFT_FWD_OVERWING
			80, // DOOR_annunAFT_CARGO
			81, // ELEC_annunGRD_POWER_AVAILABLE
			// skip 82
			83, // DOOR_annunRIGHT_AFT_OVERWING
			84, // DOOR_annunLEFT_AFT_OVERWING
			85, // DOOR_annunEQUIP
			86, // DOOR_annunAFT_SERVICE
			87, // DOOR_annunAFT_ENTRY
			// skip 88
			// skip 89
			// skip 90
			91, // ICE_annunCOWL_ANTI_ICE[1]
			92, // ICE_annunCOWL_ANTI_ICE[0]
			93, // AIR_annunZoneTemp[2]
			94, // AIR_annunZoneTemp[1]
			// skip 95
			96, // AIR_annunZoneTemp[0]
			// 97, R_VALVE_OPEN_Lights_Status // Bright
			98, // ICE_annunVALVE_OPEN[1] // Dim
			// skip 99
			// skip 100
			// skip 101
			// skip 102
			// skip 103
			// skip 104
			105, // COWL_VALVE_OPEN_Lights_2_Status // Bright
			//106, // ICE_annunCOWL_VALVE_OPEN[1] // Dim
			// skip 107
			// skip 108
			// skip 109
			// skip 110
			// skip 111
			// skip 112
			113, // L_VALVE_OPEN_Lights_Status // Bright
			//114, // ICE_annunVALVE_OPEN[0] // Dim
			// skip 115
			// skip 116
			// skip 117
			// skip 118
			// skip 119
			// skip 120
			// skip 121
			// skip 122
			123, // ICE_annunON[1]
			124, // ICE_annunON[0]
			125, // ICE_annun_CAPT_PITOT
			126, // ICE_annun_L_ELEV_PITOT
			127, // ICE_annun_L_ALPHA_VANE
			128, // ICE_annun_L_TEMP_PROBE
			129, // AIR_annunRamDoorL
			130, // AIR_annunOFFSCHED_DESCENT
			131, // AIR_annunDualBleed
			132, // AIR_annunAUTO_FAIL
			// skip 133
			134, // AIR_annunALTN
			135, // AIR_annunMANUAL
			// skip 136
			137, // AIR_annunRamDoorR
			// skip 138
			139, // AIR_annunPackTripOff[1]
			140, // AIR_annunWingBodyOverheat[1]
			141, // AIR_annunBleedTripOff[1]
			142, // AIR_annunPackTripOff[0]
			143, // AIR_annunBleedTripOff[0]
			144, // AIR_annunWingBodyOverheat[0]
			// skip 145
			// skip 146
			// skip 147
			// skip 148
			// skip 149
			// skip 150
			// skip 151
			// skip 152
			// skip 153
			154, // ICE_annunOVERHEAT[0]
			// skip 155
			156, // ICE_annunOVERHEAT[1]
			157, // ICE_annunOVERHEAT[2]
			158, // ICE_annunOVERHEAT[3]
			159, // ICE_annunON[3]
			160, // ICE_annunON[2]
			// skip 161
			// skip 162
			163, // ICE_annun_AUX_PITOT
			164, // ICE_annun_R_ALPHA_VANE
			165, // ICE_annun_R_ELEV_PITOT
			166, // ICE_annun_FO_PITOT
			//
			// Aft Overhead
			//
			169, //eecOn2 == 1
			170, // ENG_annunREVERSER[0]
			171, //eecOn1 == 1
			172, // ENG_annunALTN[0]
			173, // ENG_annunENGINE_CONTROL[0]
			174, // ENG_annunREVERSER[1]
			175, // ENG_annunALTN[1]
			176, // ENG_annunENGINE_CONTROL[1]
			177, // IRS_annunDC_FAIL[1]
			178, // GEAR_annunOvhdLEFT
			179, // GEAR_annunOvhdNOSE
			180, // IRS_annunON_DC[1]
			181, // IRS_annunFAULT[1]
			182, // GEAR_annunOvhdRIGHT
			183, // IRS_annunALIGN[1]
			184, // IRS_annunGPS
			// skip 185
			186, // IRS_annunON_DC[0]
			187, // IRS_annunFAULT[0]
			188, // WARN_annunPSEU
			189, // IRS_annunDC_FAIL[0]
			190, // IRS_annunALIGN[0]
			//
			// End Aft Overhead
			//
			//
			// Forward Overhead cont.
			//
			194, // FUEL_annunLOWPRESS_Fwd[1]
			193, // FUEL_annunLOWPRESS_Aft[1]
			201, // FUEL_annunXFEED_VALVE_OPEN // Dim
			202, // FUEL_annunXFEED_VALVE_OPEN // Bright
			// 209, COWL_VALVE_OPEN_Lights_1_Status// Bright
			210, // ICE_annunCOWL_VALVE_OPEN[0] // Dim
		};

		for (auto led : overheadLeds)
		{
			hid->pLED_Set(overHeadSession, led, 1);
		}
	}

	//
	// TODO Only call pLED() when LED changes state!!!!
	//
	void InterfaceIT::updateMipLEDs() const
	{
		hid->pLED_Set(mipSession, 1, aircraft_model.z737InData.WARN_annunMASTER_CAUTION);
		hid->pLED_Set(mipSession, 2, aircraft_model.z737InData.WARN_annunELEC);
		hid->pLED_Set(mipSession, 3, aircraft_model.z737InData.WARN_annunFLT_CONT);
		hid->pLED_Set(mipSession, 4, aircraft_model.z737InData.WARN_annunFUEL);
		hid->pLED_Set(mipSession, 5, aircraft_model.z737InData.WARN_annunOVHT_DET);
		hid->pLED_Set(mipSession, 6, aircraft_model.z737InData.WARN_annunMASTER_CAUTION);
		hid->pLED_Set(mipSession, 7, aircraft_model.z737InData.WARN_annunIRS);
		hid->pLED_Set(mipSession, 8, aircraft_model.z737InData.WARN_annunAPU);

		hid->pLED_Set(mipSession, 9, ( aircraft_model.z737InData.MAIN_annun_AP_RED[0])); // Red
		hid->pLED_Set(mipSession, 10, ( aircraft_model.z737InData.MAIN_annun_AP_AMBER[0])); // Amber

		hid->pLED_Set(mipSession, 11, ( aircraft_model.z737InData.MAIN_annun_AT_RED[0])); // Red
		hid->pLED_Set(mipSession, 12, ( aircraft_model.z737InData.MAIN_annun_AT_AMBER[0])); // Amber
		hid->pLED_Set(mipSession, 13, aircraft_model.z737InData.MAIN_annun_FMC_AMBER[0]);
		hid->pLED_Set(mipSession, 14, aircraft_model.z737InData.annun_cabin_alt);
		hid->pLED_Set(mipSession, 15, aircraft_model.z737InData.takeoff_config);
		hid->pLED_Set(mipSession, 16, aircraft_model.z737InData.MAIN_annunBELOW_GS);
		hid->pLED_Set(mipSession, 17, aircraft_model.z737InData.MAIN_annunSPEEDBRAKE_ARMED);
		hid->pLED_Set(mipSession, 18, aircraft_model.z737InData.MAIN_annunSPEEDBRAKE_DO_NOT_ARM);
		hid->pLED_Set(mipSession, 19, aircraft_model.z737InData.MAIN_annunANTI_SKID_INOP);
		hid->pLED_Set(mipSession, 20, aircraft_model.z737InData.MAIN_annunSTAB_OUT_OF_TRIM);

		hid->pLED_Set(mipSession, 21, aircraft_model.z737InData.MAIN_annunAUTO_BRAKE_DISARM);
		hid->pLED_Set(mipSession, 22, aircraft_model.z737InData.Left_Gear_Safe);		// Gear Locked 0 Left
		hid->pLED_Set(mipSession, 23, aircraft_model.z737InData.MAIN_annunLE_FLAPS_TRANSIT);
		hid->pLED_Set(mipSession, 24, aircraft_model.z737InData.MAIN_annunLE_FLAPS_EXT);
		hid->pLED_Set(mipSession, 25, aircraft_model.z737InData.MAIN_annunGEAR_transit[1]);
		hid->pLED_Set(mipSession, 26, aircraft_model.z737InData.takeoff_config);
		hid->pLED_Set(mipSession, 27, aircraft_model.z737InData.MAIN_annunGEAR_transit[0]);
		hid->pLED_Set(mipSession, 28, aircraft_model.z737InData.Right_Gear_Safe);		// Gear Locked 2 Right
		hid->pLED_Set(mipSession, 29, aircraft_model.z737InData.Nose_Gear_Safe);		// Gear Locked 1 Nose
		hid->pLED_Set(mipSession, 30, aircraft_model.z737InData.MAIN_annunGEAR_transit[2]);
		hid->pLED_Set(mipSession, 31, aircraft_model.z737InData.annun_cabin_alt);
		// 32
		hid->pLED_Set(mipSession, 33, aircraft_model.z737InData.MAIN_annunSPEEDBRAKE_EXTENDED);
		hid->pLED_Set(mipSession, 34, ( aircraft_model.z737InData.MAIN_annun_AT_AMBER[1])); // Amber
		hid->pLED_Set(mipSession, 35, ( aircraft_model.z737InData.MAIN_annun_AP_AMBER[1])); // Amber

		hid->pLED_Set(mipSession, 37, aircraft_model.z737InData.MAIN_annunBELOW_GS);
		hid->pLED_Set(mipSession, 38, ( aircraft_model.z737InData.MAIN_annun_AP_RED[1])); // Red
		hid->pLED_Set(mipSession, 39, ( aircraft_model.z737InData.MAIN_annun_AT_RED[1])); // Red
		hid->pLED_Set(mipSession, 40, aircraft_model.z737InData.MAIN_annun_FMC_AMBER[1]);
		hid->pLED_Set(mipSession, 41, aircraft_model.z737InData.WARN_annunMASTER_CAUTION);
		hid->pLED_Set(mipSession, 42, aircraft_model.z737InData.WARN_annunHYD);
		hid->pLED_Set(mipSession, 43, aircraft_model.z737InData.WARN_annunOVERHEAD);
		hid->pLED_Set(mipSession, 44, aircraft_model.z737InData.WARN_annunAIR_COND);
		hid->pLED_Set(mipSession, 45, aircraft_model.z737InData.WARN_annunFIRE_WARN[1]);
		hid->pLED_Set(mipSession, 46, aircraft_model.z737InData.WARN_annunENG);
		hid->pLED_Set(mipSession, 47, aircraft_model.z737InData.WARN_annunANTI_ICE);
		hid->pLED_Set(mipSession, 48, aircraft_model.z737InData.WARN_annunDOORS);
		//hid->pLED_Set(mipSession, 49, aircraft_model.z737InData.);		// runway INOP
		hid->pLED_Set(mipSession, 50, aircraft_model.z737InData.GPWS_annunINOP);
		//hid->pLED_Set(mipSession, 51, aircraft_model.z737InData.);
		//hid->pLED_Set(mipSession, 52, aircraft_model.z737InData.);
		//hid->pLED_Set(mipSession, 53, aircraft_model.z737InData.);
		//hid->pLED_Set(mipSession, 54, aircraft_model.z737InData.);
		//hid->pLED_Set(mipSession, 55, aircraft_model.z737InData.);
		//hid->pLED_Set(mipSession, 56, aircraft_model.z737InData.);
		//hid->pLED_Set(mipSession, 57, aircraft_model.z737InData.);
		hid->pLED_Set(mipSession, 58, aircraft_model.z737InData.WARN_annunFIRE_WARN[0]);
		//hid->pLED_Set(mipSession, 59, aircraft_model.z737InData.);
		//hid->pLED_Set(mipSession, 60, aircraft_model.z737InData.);
		//hid->pLED_Set(mipSession, 61, aircraft_model.z737InData.);
		//hid->pLED_Set(mipSession, 62, aircraft_model.z737InData.);
		//hid->pLED_Set(mipSession, 63, aircraft_model.z737InData.);
		//hid->pLED_Set(mipSession, 64, aircraft_model.z737InData.);
	}

	void InterfaceIT::updateOverheadLEDs() const
	{
		// skip CALL 1
		hid->pLED_Set(overHeadSession, 2, aircraft_model.z737InData.FCTL_annunLOW_QUANTITY);
		hid->pLED_Set(overHeadSession, 3, aircraft_model.z737InData.FCTL_annunLOW_STBY_RUD_ON);
		hid->pLED_Set(overHeadSession, 4, aircraft_model.z737InData.FCTL_annunFEEL_DIFF_PRESS);
		hid->pLED_Set(overHeadSession, 5, aircraft_model.z737InData.FCTL_annunSPEED_TRIM_FAIL);
		hid->pLED_Set(overHeadSession, 6, aircraft_model.z737InData.FCTL_annunMACH_TRIM_FAIL);
		hid->pLED_Set(overHeadSession, 7, aircraft_model.z737InData.FCTL_annunAUTO_SLAT_FAIL);
		hid->pLED_Set(overHeadSession, 8, aircraft_model.z737InData.FCTL_annunYAW_DAMPER);

		//	hid->pLED_Set(overHeadSession, 9, (( aircraft_model.z737InData.ENG_VALVE_CLOSED_Light_1_Status) >> 1) & 1); // Bright
		hid->pLED_Set(overHeadSession, 10, ( aircraft_model.z737InData.FUEL_annunENG_VALVE_CLOSED[0])); // Dim
		hid->pLED_Set(overHeadSession, 11, aircraft_model.z737InData.FCTL_annunFC_LOW_PRESSURE[0]);
		hid->pLED_Set(overHeadSession, 12, aircraft_model.z737InData.AIR_annunEquipCoolingSupplyOFF);
		hid->pLED_Set(overHeadSession, 13, aircraft_model.z737InData.AIR_annunEquipCoolingExhaustOFF);
		hid->pLED_Set(overHeadSession, 14, aircraft_model.z737InData.LTS_annunEmerNOT_ARMED);
		hid->pLED_Set(overHeadSession, 15, aircraft_model.z737InData.FCTL_annunFC_LOW_PRESSURE[1]);
		// skip 16

		//	hid->pLED_Set(overHeadSession, 17, (( aircraft_model.z737InData.ENG_VALVE_CLOSED_Light_2_Status) >> 1) & 1);
		hid->pLED_Set(overHeadSession, 18, ( aircraft_model.z737InData.FUEL_annunENG_VALVE_CLOSED[1]));
		hid->pLED_Set(overHeadSession, 19, aircraft_model.z737InData.FUEL_annunFILTER_BYPASS[1]);
		hid->pLED_Set(overHeadSession, 20, aircraft_model.z737InData.FUEL_annunFILTER_BYPASS[0]);
		hid->pLED_Set(overHeadSession, 21, aircraft_model.z737InData.FUEL_annunLOWPRESS_Ctr[1]);
		hid->pLED_Set(overHeadSession, 22, aircraft_model.z737InData.FUEL_annunLOWPRESS_Ctr[0]);
		hid->pLED_Set(overHeadSession, 23, aircraft_model.z737InData.FUEL_annunLOWPRESS_Aft[0]);
		hid->pLED_Set(overHeadSession, 24, aircraft_model.z737InData.FUEL_annunLOWPRESS_Fwd[0]);

		//skip 25, 
		//skip 26, 
		//hid->pLED_Set(overHeadSession, 27, aircraft_model.z737InData.FUEL_annunLOWPRESS_Fwd[1]);
		//hid->pLED_Set(overHeadSession, 28, aircraft_model.z737InData.FUEL_annunLOWPRESS_Aft[1]);
		hid->pLED_Set(overHeadSession, 29, aircraft_model.z737InData.ELEC_annunBAT_DISCHARGE);
		hid->pLED_Set(overHeadSession, 30, aircraft_model.z737InData.ELEC_annunTR_UNIT);
		hid->pLED_Set(overHeadSession, 31, aircraft_model.z737InData.ELEC_annunELEC);
		// skip 32

		//	hid->pLED_Set(overHeadSession, 33, (( aircraft_model.z737InData.SPAR_VALVE_CLOSED_Light_1_Status) >> 1) & 1); // Bright
		hid->pLED_Set(overHeadSession, 34, ( aircraft_model.z737InData.FUEL_annunSPAR_VALVE_CLOSED[0])); // Dim
		hid->pLED_Set(overHeadSession, 35, aircraft_model.z737InData.ELEC_annunDRIVE[0]);
		hid->pLED_Set(overHeadSession, 36, aircraft_model.z737InData.ELEC_annunSTANDBY_POWER_OFF);
		hid->pLED_Set(overHeadSession, 37, aircraft_model.z737InData.ELEC_annunDRIVE[1]);
		// skip 38
		// skip 39
		// skip 40

		//	hid->pLED_Set(overHeadSession, 41, (( aircraft_model.z737InData.SPAR_VALVE_CLOSED_Light_2_Status) >> 1) & 1); // Bright
		hid->pLED_Set(overHeadSession, 42, ( aircraft_model.z737InData.FUEL_annunSPAR_VALVE_CLOSED[1])); // Dim
		// skip 43
		// skip 44
		// skip 45
		// skip 46
		// skip 47
		// skip 48

		hid->pLED_Set(overHeadSession, 49, aircraft_model.z737InData.APU_annunMAINT);
		// skip 50
		hid->pLED_Set(overHeadSession, 51, aircraft_model.z737InData.APU_annunLOW_OIL_PRESSURE);
		hid->pLED_Set(overHeadSession, 52, aircraft_model.z737InData.APU_annunFAULT);
		hid->pLED_Set(overHeadSession, 53, aircraft_model.z737InData.APU_annunOVERSPEED);
		hid->pLED_Set(overHeadSession, 54, aircraft_model.z737InData.ELEC_annunSOURCE_OFF[1]);
		hid->pLED_Set(overHeadSession, 55, aircraft_model.z737InData.ELEC_annunTRANSFER_BUS_OFF[1]);
		hid->pLED_Set(overHeadSession, 56, aircraft_model.z737InData.ELEC_annunTRANSFER_BUS_OFF[0]);

		hid->pLED_Set(overHeadSession, 57, aircraft_model.z737InData.ELEC_annunGEN_BUS_OFF[0]);
		// skip 58
		hid->pLED_Set(overHeadSession, 59, aircraft_model.z737InData.ELEC_annunSOURCE_OFF[0]);
		// skip 60
		// skip 61
		// skip 62
		// skip 63
		// skip 64

		hid->pLED_Set(overHeadSession, 65, aircraft_model.z737InData.ELEC_annunGEN_BUS_OFF[1]);
		// skip 66
		hid->pLED_Set(overHeadSession, 67, aircraft_model.z737InData.HYD_annunLOW_PRESS_eng[0]);
		hid->pLED_Set(overHeadSession, 68, aircraft_model.z737InData.HYD_annunLOW_PRESS_elec[0]);
		hid->pLED_Set(overHeadSession, 69, aircraft_model.z737InData.HYD_annunLOW_PRESS_elec[1]);
		hid->pLED_Set(overHeadSession, 70, aircraft_model.z737InData.HYD_annunLOW_PRESS_eng[1]);
		hid->pLED_Set(overHeadSession, 71, aircraft_model.z737InData.HYD_annunOVERHEAT_elec[0]);
		hid->pLED_Set(overHeadSession, 72, aircraft_model.z737InData.HYD_annunOVERHEAT_elec[1]);

		hid->pLED_Set(overHeadSession, 73, aircraft_model.z737InData.ELEC_annunAPU_GEN_OFF_BUS);
		// skip 74
		hid->pLED_Set(overHeadSession, 75, aircraft_model.z737InData.DOOR_annunFWD_ENTRY);
		hid->pLED_Set(overHeadSession, 76, aircraft_model.z737InData.DOOR_annunFWD_SERVICE);
		hid->pLED_Set(overHeadSession, 77, aircraft_model.z737InData.DOOR_annunFWD_CARGO);
		hid->pLED_Set(overHeadSession, 78, aircraft_model.z737InData.DOOR_annunRIGHT_FWD_OVERWING);
		hid->pLED_Set(overHeadSession, 79, aircraft_model.z737InData.DOOR_annunLEFT_FWD_OVERWING);
		hid->pLED_Set(overHeadSession, 80, aircraft_model.z737InData.DOOR_annunAFT_CARGO);

		hid->pLED_Set(overHeadSession, 81, aircraft_model.z737InData.ELEC_annunGRD_POWER_AVAILABLE);
		// skip 82
		hid->pLED_Set(overHeadSession, 83, aircraft_model.z737InData.DOOR_annunRIGHT_AFT_OVERWING);
		hid->pLED_Set(overHeadSession, 84, aircraft_model.z737InData.DOOR_annunLEFT_AFT_OVERWING);
		hid->pLED_Set(overHeadSession, 85, aircraft_model.z737InData.DOOR_annunEQUIP);
		hid->pLED_Set(overHeadSession, 86, aircraft_model.z737InData.DOOR_annunAFT_SERVICE);
		hid->pLED_Set(overHeadSession, 87, aircraft_model.z737InData.DOOR_annunAFT_ENTRY);
		// skip 88

		// skip  89
		// skip  90
		hid->pLED_Set(overHeadSession, 91, aircraft_model.z737InData.ICE_annunCOWL_ANTI_ICE[1]);
		hid->pLED_Set(overHeadSession, 92, aircraft_model.z737InData.ICE_annunCOWL_ANTI_ICE[0]);
		hid->pLED_Set(overHeadSession, 93, aircraft_model.z737InData.AIR_annunZoneTemp[2]);
		hid->pLED_Set(overHeadSession, 94, aircraft_model.z737InData.AIR_annunZoneTemp[1]);
		// skip 95
		hid->pLED_Set(overHeadSession, 96, aircraft_model.z737InData.AIR_annunZoneTemp[0]);

		//	hid->pLED_Set(overHeadSession, 97, (( aircraft_model.z737InData.R_VALVE_OPEN_Lights_Status) >> 1) & 1); // Bright
		hid->pLED_Set(overHeadSession, 98, ( aircraft_model.z737InData.ICE_annunVALVE_OPEN[1])); // Dim
		// skip 99
		// skip 100
		// skip 101
		// skip 102
		// skip 103
		// skip 104

		//	hid->pLED_Set(overHeadSession, 105, (( aircraft_model.z737InData.COWL_VALVE_OPEN_Lights_2_Status) >> 1) & 1); // Bright
		hid->pLED_Set(overHeadSession, 106, ( aircraft_model.z737InData.ICE_annunCOWL_VALVE_OPEN[1])); // Dim
		// skip 107
		// skip 108
		// skip 109
		// skip 110
		// skip 111
		// skip 112

		//	hid->pLED_Set(overHeadSession, 113, (( aircraft_model.z737InData.L_VALVE_OPEN_Lights_Status) >> 1) & 1); // Bright
		hid->pLED_Set(overHeadSession, 114, ( aircraft_model.z737InData.ICE_annunVALVE_OPEN[0])); // Dim
		// skip 115
		// skip 116
		// skip 117
		// skip 118
		// skip 119
		// skip 120

		// skip 121
		// skip 122
		hid->pLED_Set(overHeadSession, 123, aircraft_model.z737InData.ICE_annunON[1]);
		hid->pLED_Set(overHeadSession, 124, aircraft_model.z737InData.ICE_annunON[0]);
		hid->pLED_Set(overHeadSession, 125, aircraft_model.z737InData.ICE_annun_CAPT_PITOT);		// CAPT PITOT
		hid->pLED_Set(overHeadSession, 126, aircraft_model.z737InData.ICE_annun_CAPT_PITOT);	// ELEV_PITOT
		hid->pLED_Set(overHeadSession, 127, aircraft_model.z737InData.ICE_annun_CAPT_PITOT);	// ALPHA_PITOT
		hid->pLED_Set(overHeadSession, 128, aircraft_model.z737InData.ICE_annun_CAPT_PITOT);	// TEMP_PROBE

		hid->pLED_Set(overHeadSession, 129, aircraft_model.z737InData.AIR_annunRamDoorL);
		hid->pLED_Set(overHeadSession, 130, aircraft_model.z737InData.AIR_annunOFFSCHED_DESCENT);
		hid->pLED_Set(overHeadSession, 131, aircraft_model.z737InData.AIR_annunDualBleed);
		hid->pLED_Set(overHeadSession, 132, aircraft_model.z737InData.AIR_annunAUTO_FAIL);
		// skip 133
		hid->pLED_Set(overHeadSession, 134, aircraft_model.z737InData.AIR_annunALTN);
		hid->pLED_Set(overHeadSession, 135, aircraft_model.z737InData.AIR_annunMANUAL);
		// skip 136

		hid->pLED_Set(overHeadSession, 137, aircraft_model.z737InData.AIR_annunRamDoorR);
		// skip 138
		hid->pLED_Set(overHeadSession, 139, aircraft_model.z737InData.AIR_annunPackTripOff[1]);
		hid->pLED_Set(overHeadSession, 140, aircraft_model.z737InData.AIR_annunWingBodyOverheat[1]);
		hid->pLED_Set(overHeadSession, 141, aircraft_model.z737InData.AIR_annunBleedTripOff[1]);
		hid->pLED_Set(overHeadSession, 142, aircraft_model.z737InData.AIR_annunPackTripOff[0]);
		hid->pLED_Set(overHeadSession, 143, aircraft_model.z737InData.AIR_annunBleedTripOff[0]);
		hid->pLED_Set(overHeadSession, 144, aircraft_model.z737InData.AIR_annunWingBodyOverheat[0]);

		// skip 145
		// skip 146
		// skip 147
		// skip 148
		// skip 149
		// skip 150
		// skip 151
		// skip 152

		// skip 153
		hid->pLED_Set(overHeadSession, 154, aircraft_model.z737InData.ICE_annunOVERHEAT[0]);
		// skip 155
		hid->pLED_Set(overHeadSession, 156, aircraft_model.z737InData.ICE_annunOVERHEAT[1]);
		hid->pLED_Set(overHeadSession, 157, aircraft_model.z737InData.ICE_annunOVERHEAT[2]);
		hid->pLED_Set(overHeadSession, 158, aircraft_model.z737InData.ICE_annunOVERHEAT[3]);
		hid->pLED_Set(overHeadSession, 159, aircraft_model.z737InData.ICE_annunON[3]);
		hid->pLED_Set(overHeadSession, 160, aircraft_model.z737InData.ICE_annunON[2]);

		// skip 161
		// skip 162
		hid->pLED_Set(overHeadSession, 163, aircraft_model.z737InData.ICE_annun_FO_PITOT);		// AUX PITOT
		hid->pLED_Set(overHeadSession, 164, aircraft_model.z737InData.ICE_annun_FO_PITOT);	// R ALPHA VANE
		hid->pLED_Set(overHeadSession, 165, aircraft_model.z737InData.ICE_annun_FO_PITOT);	// R ELEV PITOT
		hid->pLED_Set(overHeadSession, 166, aircraft_model.z737InData.ICE_annun_FO_PITOT);		// FO PITOT

		//
		// Aft Overhead
		//
		eecOn1 = (( aircraft_model.z737InData.ENG_annunALTN[0]));
		eecOn1 = (~eecOn1) & 1;
		eecOn2 = (( aircraft_model.z737InData.ENG_annunALTN[1]));
		eecOn2 = (~eecOn2) & 1;
		hid->pLED_Set(overHeadSession, 169, eecOn2 == 1);
		hid->pLED_Set(overHeadSession, 170, ( aircraft_model.z737InData.ENG_annunREVERSER[0]));
		hid->pLED_Set(overHeadSession, 171, eecOn1 == 1);
		hid->pLED_Set(overHeadSession, 172, (( aircraft_model.z737InData.ENG_annunALTN[0])));
		hid->pLED_Set(overHeadSession, 173, ( aircraft_model.z737InData.ENG_annunENGINE_CONTROL[0]));
		hid->pLED_Set(overHeadSession, 174, ( aircraft_model.z737InData.ENG_annunREVERSER[1]));
		hid->pLED_Set(overHeadSession, 175, (( aircraft_model.z737InData.ENG_annunALTN[1])));
		hid->pLED_Set(overHeadSession, 176, ( aircraft_model.z737InData.ENG_annunENGINE_CONTROL[1]));

		hid->pLED_Set(overHeadSession, 177, ( aircraft_model.z737InData.IRS_annunDC_FAIL[1]));
		hid->pLED_Set(overHeadSession, 178, ( aircraft_model.z737InData.Left_Gear_Safe));	// GEAR Ovhd LEFT
		hid->pLED_Set(overHeadSession, 179, ( aircraft_model.z737InData.Nose_Gear_Safe));	// GEAR Ovhd NOSE

		hid->pLED_Set(overHeadSession, 180, ( aircraft_model.z737InData.IRS_annunON_DC[1]));
		hid->pLED_Set(overHeadSession, 181, ( aircraft_model.z737InData.IRS_annunFAULT[1]));
		hid->pLED_Set(overHeadSession, 182, ( aircraft_model.z737InData.Right_Gear_Safe));	// GEAR Ovhd RIGHT
		hid->pLED_Set(overHeadSession, 183, ( aircraft_model.z737InData.IRS_annunALIGN[1]));
		hid->pLED_Set(overHeadSession, 184, ( aircraft_model.z737InData.IRS_annunGPS));
		// skip 185
		hid->pLED_Set(overHeadSession, 186, ( aircraft_model.z737InData.IRS_annunON_DC[0]));
		hid->pLED_Set(overHeadSession, 187, ( aircraft_model.z737InData.IRS_annunFAULT[0]));
		hid->pLED_Set(overHeadSession, 188, ( aircraft_model.z737InData.WARN_annunPSEU));
		hid->pLED_Set(overHeadSession, 189, ( aircraft_model.z737InData.IRS_annunDC_FAIL[0]));
		hid->pLED_Set(overHeadSession, 190, ( aircraft_model.z737InData.IRS_annunALIGN[0]));


		//
		// End Aft Overhead
		//


		//
		// Forward Overhead cont.
		//
		hid->pLED_Set(overHeadSession, 194, aircraft_model.z737InData.FUEL_annunLOWPRESS_Fwd[1]);
		hid->pLED_Set(overHeadSession, 193, aircraft_model.z737InData.FUEL_annunLOWPRESS_Aft[1]);

		hid->pLED_Set(overHeadSession, 201, ( aircraft_model.z737InData.FUEL_annunXFEED_VALVE_OPEN >> 1) & 1); // Dim
		hid->pLED_Set(overHeadSession, 202, (( aircraft_model.z737InData.FUEL_annunXFEED_VALVE_OPEN)) & 1); // Bright

		//	hid->pLED_Set(overHeadSession, 209, (( aircraft_model.z737InData.COWL_VALVE_OPEN_Lights_1_Status) >> 1) & 1); // Bright
		hid->pLED_Set(overHeadSession, 210, ( aircraft_model.z737InData.ICE_annunCOWL_VALVE_OPEN[0])); // Dim
	}



	void InterfaceIT::updateOverheadSwitches(const int nswitch, const int state)
	{
		const InterfaceITSwitch interface_it_switch = overhead_switches[nswitch];
		const int switch_value = interface_it_switch.state_to_value[state];
		if (interface_it_switch.type != common::SwitchType::unused)
		{
			const auto dataref_name = overhead_zcockpit_switches[nswitch].dataref_name;
			if (dataref_name != DataRefName::DataRefName_unused) {
				overhead_zcockpit_switches[nswitch].int_hw_value = switch_value;

				// Standby Bat Power
				if (nswitch == 44 || nswitch == 45) {
					// Switch is only off when both pins (44 && 45) are off
					if (overhead_zcockpit_switches[44].int_hw_value == XPLANE_STANDBY_POWER_OFF &&
						overhead_zcockpit_switches[45].int_hw_value == XPLANE_STANDBY_POWER_OFF) {
						aircraft_model.push_switch_change(overhead_zcockpit_switches[nswitch]);
						//LOG() << "Switch Change: nswitch " << nswitch << " value " << switch_value;
					}
					else {
						if (overhead_zcockpit_switches[nswitch].int_hw_value != XPLANE_STANDBY_POWER_OFF) {
							aircraft_model.push_switch_change(overhead_zcockpit_switches[nswitch]);
							//LOG() << "Switch Change: nswitch " << nswitch << " value " << switch_value;
						}
					}
				}
				else {
					if (common::SwitchType::pushbutton == interface_it_switch.type && switch_value == 1)
					{
						aircraft_model.push_switch_change(overhead_zcockpit_switches[nswitch]);
					}
					if (common::SwitchType::toggle == interface_it_switch.type || common::SwitchType::spring_loaded == interface_it_switch.type)
					{
						aircraft_model.push_switch_change(overhead_zcockpit_switches[nswitch]);
					}
					else if (common::SwitchType::rotary_2_commands == interface_it_switch.type)
					{
						// for multi-way switches if one of the poles is One then others must be Zero
						// we only want to use the pole that has changed to one
						if (state == 1)
						{
							aircraft_model.push_switch_change(overhead_zcockpit_switches[nswitch]);
						}
					}
					//LOG() << "Switch Change: nswitch " << nswitch << " value " << switch_value;
				}
			}



				//int command = overhead_switches[nswitch][0];

				//if (command == KEY_COMMAND_ELECTRICAL_STANDBY_POWER_OFF)
				//{
				//	if (parameter == 2)
				//	{
				//		sendMessageInt(KEY_COMMAND_ELECTRICAL_STANDBY_POWER_AUTO, 0);
				//	}
				//	else
				//	{
				//		sendMessageInt(KEY_COMMAND_ELECTRICAL_STANDBY_POWER_BAT, 0);
				//	}
				//}
			
				//if (parameter == XPLANE_EMER_LIGHT_OFF)
				//{
				//	if (in_data == 1) // ARM
				//	{
				//		// close the guard
				//		sendMessageInt(XPLANE_EMER_LIGHT_ARMED, 0);
				//	}
				//	else
				//	{
				//		// open the guard
				//		sendMessageInt(XPLANE_EMER_LIGHT_OFF, 0);
				//	}
				//}
				//else
//				{
//////					sendMessageInt(in_data, 0);
//				}


			//if (value == KEY_COMMAND_ELECTRICAL_BAT_ON)
			//{
			//	Bat_is_on(true);
			//}
			//else if (value == KEY_COMMAND_ELECTRICAL_BAT_OFF)
			//{
			//	Bat_is_on(false);
			//}

		}
	}


	void InterfaceIT::updateMipSwitches(const int nswitch, const int state)
	{
		const InterfaceITSwitch hw_switch = mip_switches[nswitch];
		const int switch_value = hw_switch.state_to_value[state];
		const auto dataref_name = mip_zcockpit_switches[nswitch].dataref_name;

		if (dataref_name != DataRefName::DataRefName_unused) {
			if (hw_switch.type != common::SwitchType::unused)
			{
				if (nswitch == 19)
				{
					// Left Clock Pressed

				}
				// GEAR UP
				if (nswitch == 28)
				{
					gear_lever_up = state;
					if(gear_lever_up == 0 && gear_lever_dn == 0) {
						// gear off
						mip_zcockpit_switches[nswitch].int_hw_value = switch_value;
						aircraft_model.push_switch_change(mip_zcockpit_switches[nswitch]);
					}
					else if(gear_lever_up == 1) {
						// gear up
						mip_zcockpit_switches[nswitch].int_hw_value = switch_value;
						aircraft_model.push_switch_change(mip_zcockpit_switches[nswitch]);
					}
				}
				// GEAR DOWN
				else if (nswitch == 31)
				{
					gear_lever_dn = state;
					if(gear_lever_up == 0 && gear_lever_dn == 0) {
						// gear off
						mip_zcockpit_switches[nswitch].int_hw_value = switch_value;
						aircraft_model.push_switch_change(mip_zcockpit_switches[nswitch]);
					}
					else if(gear_lever_dn == 1) {
						// gear down
						mip_zcockpit_switches[nswitch].int_hw_value = switch_value;
						aircraft_model.push_switch_change(mip_zcockpit_switches[nswitch]);
					}
				}
				else {
					mip_zcockpit_switches[nswitch].int_hw_value = switch_value;
					if(common::SwitchType::pushbutton == hw_switch.type && state == 1) {
						aircraft_model.push_switch_change(mip_zcockpit_switches[nswitch]);

					}
					else if (common::SwitchType::toggle == hw_switch.type || common::SwitchType::spring_loaded == hw_switch.type) 
					{
						aircraft_model.push_switch_change(mip_zcockpit_switches[nswitch]);
					}
					else if ((common::SwitchType::rotary_2_commands == hw_switch.type || common::SwitchType::rotary_multi_commands == hw_switch.type) && state == 1)
					{
						// for multi-way switches if one of the poles is One then others must be Zero
						// we only want to use the pole that has changed to one
						aircraft_model.push_switch_change(mip_zcockpit_switches[nswitch]);
					}
					//LOG() << "Switch Change: nswitch " << nswitch << " value " << switch_value;

				}
			}
	}
	}


	int InterfaceIT::KeyPressedProc(int status, const char* controller, unsigned long* session, int nSwitch, int nState)
	{
		if (status == IITAPI_ERR_OK)
		{
			if (*session == overHeadSession)
			{
				updateOverheadSwitches(nSwitch, nState);
			}
			else
			{
				updateMipSwitches(nSwitch, nState);
			}
		}
		return status;
	}

	void InterfaceIT::drop()
	{
		zcockpit::cockpit::hardware::InterfaceIT::hid->drop();
		static std::mutex mutex;
		std::lock_guard<std::mutex> lock(mutex);
	}

	void InterfaceIT::validateDevices()
	{
		LOG() << "InterfaceIT: validating devices";
		int nRet = IITAPI_ERR_OK;
		unsigned long dwSize = 0;
		int nPos = 0;
		char* pBuffer = nullptr;
		char* pszBoard = nullptr;

		mipFound = false;
		overHeadFound = false;

		//Find out how much memory we need to allocate by passing nullptr
		nRet = hid->pGetDeviceList(nullptr, &dwSize, pszBoard);
		if (nRet == IITAPI_ERR_CONTROLLERS_NOT_OPENED)
		{
			if ((nRet = hid->pOpenControllers()) == IITAPI_ERR_OK)
			{
				avaliable = true;
				hid->pGetDeviceList(nullptr, &dwSize, pszBoard);
			}
		}
		// Allocate Buffer
		const auto temp_bufffer = std::make_unique<char[]>(dwSize + 1);
		pBuffer = temp_bufffer.get();  //new char[dwSize + 1];

		//Retrieve a list of all connected boards
		nRet = hid->pGetDeviceList(pBuffer, &dwSize, pszBoard);
		if (nRet == IITAPI_ERR_CONTROLLERS_NOT_OPENED)
		{
			if ((nRet = hid->pOpenControllers()) == IITAPI_ERR_OK)
			{
				avaliable = true;
				nRet = hid->pGetDeviceList(pBuffer, &dwSize, pszBoard);
			}
		}

		if (nRet == IITAPI_ERR_OK && dwSize != 1)
		{
			char szData[MAX_PATH] = { 0 };
			strcpy_s(szData, _countof(szData), pBuffer + nPos);
			int nSize = strlen(szData);
			// Check if we are done processing the list
			if (nSize != 0)
			{
				if (strcmp(szData, InterfaceIT::mip) == 0)
				{
					mipFound = true;
					LOG() << "InterfaceIT: validating -- mip found";
				}
				else if (strcmp(szData, InterfaceIT::overhead) == 0)
				{
					overHeadFound = true;
					LOG() << "InterfaceIT: validating -- overhead found";
				}
				nPos += nSize + 1;
				strcpy_s(szData, _countof(szData), pBuffer + nPos);
				nSize = strlen(szData);
				// Check if we are done processing the list
				if (nSize != 0)
				{
					if (strcmp(szData, InterfaceIT::mip) == 0)
					{
						mipFound = true;
						LOG() << "InterfaceIT: validating -- mip found";
					}
					else if (strcmp(szData, InterfaceIT::overhead) == 0)
					{
						overHeadFound = true;
						LOG() << "InterfaceIT: validating -- overhead found";
					}
				}
			}
		}
	}

	void InterfaceIT::initialize()
	{

		LOG() << "InterfaceIT: Initializing ...";

		avaliable = false;
		mipAvailable = false;
		overHeadAvailable = false;

		int nRet = IITAPI_ERR_OK;

		if (checkDeviceInProgress)
		{
			return;
		}



		/*
		 *	Open the controllers / API
		 */
		if ((nRet = hid->pOpenControllers()) == IITAPI_ERR_OK)
		{
			LOG() << "InterfaceIT: opened controller";
			avaliable = true;
			validateDevices();

			if (mipFound && (nRet = bindController(mip, &mipSession)) == IITAPI_ERR_OK)
			{
				mipAvailable = true;
			}


			if (overHeadFound && (nRet = bindController(overhead, &overHeadSession)) == IITAPI_ERR_OK)
			{
				overHeadAvailable = true;
			}
			if ((nRet = hid->pEnable_Device_Change_CallBack(true, DeviceChangeProc)) != IITAPI_ERR_OK)
			{
				avaliable = false;
			}
		}
		else
		{
			LOG() << "InterfaceIT: failed to open controller";
			hid->pCloseControllers();
		}
	}


	void InterfaceIT::processMipState()
	{
		int nSwitch = 0;
		int nState = -1;
		int status = IITAPI_ERR_OK;
		if (!checkDeviceInProgress && mipFound)
		{
			if (!mipAvailable)
			{
				if ((status = bindController(mip, &mipSession)) == IITAPI_ERR_OK)
				{
					mipAvailable = true;
				}
			}
			//
			// Loop over Switches that have changed since the last cycle
			while (mipAvailable && (status == IITAPI_ERR_OK))
			{
				status = hid->pSwitch_Get_Item(mipSession, &nSwitch, &nState);
				// in polling mode -- no items is most likely
				if (status == IITAPI_ERR_NO_ITEMS)
				{
					break;
				}
				else if (status != IITAPI_ERR_OK)
				{
					// there was some type of error
					LOG() << "Error InterfaceIT MIP Read Switches";
					unbindMip();
					return;
				}
				else
				{
					status = KeyPressedProc(status, mip, &mipSession, nSwitch, nState);
				}
			}
			if (status != IITAPI_ERR_NO_ITEMS && status != IITAPI_ERR_OK)
			{
				// there was some type of error
				unbindMip();
				return;
			}
			// Process LEDs
//// TODO accessing mip_zcockpit_switches DOESN'T WORK
//			if ( mip_zcockpit_switches[BATTERY_SWITCH].int_hw_value > 0) // 0: false  true: ON
//			{
				//if ( aircraft_model.hw_switch_data.bright_test == 0)
				//{
				//	// light test
				//	turnOnMipLEDs();
				//	lightTestToggleMip = true;
				//}
				//else
				//{
				//	if (lightTestToggleMip)
				//	{
				//		turnOffMipLEDs();
				//		lightTestToggleMip = false;
				//	}
				//	updateMipLEDs();
				//}
				updateMipLEDs();
		//	}
		//	else
		//	{
		//		turnOffMipLEDs();
		//		lightTestToggleMip = false;
		//	}
		
		
		}
	}

	void InterfaceIT::processOverHeadState()
	{
		int nSwitch = 0;
		int nState = -1;
		int status = IITAPI_ERR_OK;

		if (!checkDeviceInProgress && overHeadFound)
		{
			if (!overHeadAvailable)
			{
				if ((status = bindController(overhead, &overHeadSession)) == IITAPI_ERR_OK)
				{
					overHeadAvailable = true;
				}
			}
			while (overHeadAvailable && (status == IITAPI_ERR_OK))
			{
				status = hid->pSwitch_Get_Item(overHeadSession, &nSwitch, &nState);

				// in polling mode -- no items is most likely
				if (status == IITAPI_ERR_NO_ITEMS)
				{
					break;
				}
				else if (status != IITAPI_ERR_OK)
				{
					// there was some type of error
					unbindOverhead();
					return;
				}
				else
				{
					status = KeyPressedProc(status, overhead, &overHeadSession, nSwitch, nState);
				}
			}
			if (status != IITAPI_ERR_NO_ITEMS && status != IITAPI_ERR_OK)
			{
				// there was some type of error
				unbindOverhead();
				return;
			}
			// Process LEDs
//// TODO accessing mip_zcockpit_switches DOESN'T WORK
//			if ( mip_zcockpit_switches[BATTERY_SWITCH].int_hw_value > 0)
//			{
				//if ( aircraft_model.hw_switch_data.bright_test == 0)
				//{
				//	// light test
				//	if (!lightTestToggleOverhead)
				//	{
				//		turnOnOverheadLEDs();
				//		lightTestToggleOverhead = true;
				//	}
				//}
				//else
				//{
				//	if (lightTestToggleOverhead)
				//	{
				//		turnOffOverheadLEDs();
				//		lightTestToggleOverhead = false;
				//	}
				//	updateOverheadLEDs();
				//}
				updateOverheadLEDs();
		//	}
		//	else
		//	{
		//		turnOffOverheadLEDs();
		//		lightTestToggleOverhead = false;
		//	}
		
		
		}
	}

	BOOL CALLBACK InterfaceIT::DeviceChangeProc(int nAction)
	{
		LOG() << "InterfaceIT: calling check controller";
		InterfaceIT::checkController();
		return TRUE;
	}

	void InterfaceIT::checkController()
	{
		int nRet;
		checkDeviceInProgress = true;

		// first release what we have been using
		unbindMip();
		unbindOverhead();
		hid->pCloseControllers();
		avaliable = false;
		mipFound = false;
		overHeadFound = false;

		// re-open controller
		if ((nRet = hid->pOpenControllers()) == IITAPI_ERR_OK)
		{
			avaliable = true;
			LOG() << "InterfaceIT: passed re-controller";
			// see what is still available
			validateDevices();

			// now try to use them
			if (mipFound && (nRet = bindController(mip, &mipSession)) == IITAPI_ERR_OK)
			{
				mipAvailable = true;
				LOG() << "InterfaceIT: mip available";
			}
			if (overHeadFound && (nRet = bindController(overhead, &overHeadSession)) == IITAPI_ERR_OK)
			{
				overHeadAvailable = true;
				LOG() << "InterfaceIT: overhead available";
			}
		}
		checkDeviceInProgress = false;
	}


	int InterfaceIT::bindController(const char* controller, unsigned long* session)
	{
		int status;
		if ((status = hid->pBind(controller, session)) == IITAPI_ERR_OK)
		{
			if ((status = hid->pSwitch_Enable_Poll(*session, true)) == IITAPI_ERR_OK)
			{
				status = hid->pLED_Enable(*session, true);
			}
			LOG() << "InterfaceIT: binding status = " << status;
		}
		return status;
	}


	void InterfaceIT::unbindMip()
	{
		if (avaliable)
		{
			if (mipFound && mipAvailable)
			{
				hid->pSwitch_Enable_Poll(mipSession, false);
				hid->pLED_Enable(mipSession, false);
				hid->pUnBind(mipSession);
			}
		}
		LOG() << "InterfaceIT: unbinding mip";
		mipAvailable = false;
	}

	void InterfaceIT::unbindOverhead()
	{
		if (avaliable)
		{
			if (overHeadFound && overHeadAvailable)
			{
				hid->pSwitch_Enable_Poll(overHeadSession, false);
				hid->pLED_Enable(overHeadSession, false);
				hid->pUnBind(overHeadSession);
			}
		}
		LOG() << "InterfaceIT: unbinding overhead";
		overHeadAvailable = false;
	}

	void InterfaceIT::closeInterfaceITController()
	{
		LOG() << "InterfaceIT: closing";
		try
		{
			if (avaliable)
			{
				avaliable = false;
				if (mipFound && mipAvailable)
				{
					mipAvailable = false;
					//   			pSwitch_Enable_Poll(mipSession, false);
					//   			pLED_Enable(mipSession, false);
					hid->pUnBind(mipSession);
				}
				if (overHeadFound && overHeadAvailable)
				{
					overHeadAvailable = false;
					//    			pSwitch_Enable_Poll(overHeadSession, false);
					//	    		pLED_Enable(overHeadSession, false);
					hid->pUnBind(overHeadSession);
				}
				hid->pCloseControllers();
			}
		}
		catch (std::exception ex)
		{
			LOG() << "InterfaceIT failed to close";
		}
		avaliable = false;
		mipAvailable = false;
		overHeadAvailable = false;
	}





	//void InterfaceIT::syncMipSwitches()
	//{
	//	int nDirection;
	//	int nDirection2;
	//	int status = IITAPI_ERR_OK;
	//	int status2 = IITAPI_ERR_OK;

	//	//
	//	// pmdg selector // 0: SET  1: AUTO  2: V1  3: VR  4: WT  5: VREF  6: Bug  
	//	//
	//	status = hid->pSwitch_Get_State(mipSession, 1, &nDirection);
	//	if (status == IITAPI_ERR_OK)
	//	{
	//		if (nDirection == 1)
	//		{
	//			// ON
	//			if ( aircraft_model.hw_switch_data.spd_ref != 1)
	//			{
	//				simconnect_send_event_data(EVT_MPM_SPEED_REFERENCE_SELECTOR, 1);
	//			}
	//		}
	//	}
	//	status = hid->pSwitch_Get_State(mipSession, 7, &nDirection);
	//	if (status == IITAPI_ERR_OK)
	//	{
	//		if (nDirection == 1)
	//		{
	//			// ON
	//			if ( aircraft_model.hw_switch_data.spd_ref != 2)
	//			{
	//				simconnect_send_event_data(EVT_MPM_SPEED_REFERENCE_SELECTOR, 2);
	//			}
	//		}
	//	}
	//	status = hid->pSwitch_Get_State(mipSession, 2, &nDirection);
	//	if (status == IITAPI_ERR_OK)
	//	{
	//		if (nDirection == 1)
	//		{
	//			// ON
	//			if ( aircraft_model.hw_switch_data.spd_ref != 3)
	//			{
	//				simconnect_send_event_data(EVT_MPM_SPEED_REFERENCE_SELECTOR, 3);
	//			}
	//		}
	//	}
	//	status = hid->pSwitch_Get_State(mipSession, 3, &nDirection);
	//	if (status == IITAPI_ERR_OK)
	//	{
	//		if (nDirection == 1)
	//		{
	//			// ON
	//			if ( aircraft_model.hw_switch_data.spd_ref != 4)
	//			{
	//				simconnect_send_event_data(EVT_MPM_SPEED_REFERENCE_SELECTOR, 4);
	//			}
	//		}
	//	}
	//	status = hid->pSwitch_Get_State(mipSession, 4, &nDirection);
	//	if (status == IITAPI_ERR_OK)
	//	{
	//		if (nDirection == 1)
	//		{
	//			// ON
	//			if ( aircraft_model.hw_switch_data.spd_ref != 5)
	//			{
	//				simconnect_send_event_data(EVT_MPM_SPEED_REFERENCE_SELECTOR, 5);
	//			}
	//		}
	//	}
	//	status = hid->pSwitch_Get_State(mipSession, 5, &nDirection);
	//	if (status == IITAPI_ERR_OK)
	//	{
	//		if (nDirection == 1)
	//		{
	//			// ON
	//			if ( aircraft_model.hw_switch_data.spd_ref != 6)
	//			{
	//				simconnect_send_event_data(EVT_MPM_SPEED_REFERENCE_SELECTOR, 6);
	//			}
	//		}
	//	}
	//	status = hid->pSwitch_Get_State(mipSession, 6, &nDirection);
	//	if (status == IITAPI_ERR_OK)
	//	{
	//		if (nDirection == 1)
	//		{
	//			// ON
	//			if ( aircraft_model.hw_switch_data.spd_ref != 0)
	//			{
	//				simconnect_send_event_data(EVT_MPM_SPEED_REFERENCE_SELECTOR, 0);
	//			}
	//		}
	//	}
	//	status = hid->pSwitch_Get_State(mipSession, 9, &nDirection);
	//	if (status == IITAPI_ERR_OK)
	//	{
	//		if (nDirection == 1)
	//		{
	//			// ON
	//			if ( aircraft_model.hw_switch_data.autobrake_pos != 0)
	//			{
	//				//  simconnect_send_event_data(EVT_MPM_AUTOBRAKE_SELECTOR, 0);
	//			}
	//		}
	//	}
	//	status = hid->pSwitch_Get_State(mipSession, 10, &nDirection);
	//	if (status == IITAPI_ERR_OK)
	//	{
	//		if (nDirection == 1)
	//		{
	//			// ON
	//			if ( aircraft_model.hw_switch_data.autobrake_pos != 1)
	//			{
	//				//  simconnect_send_event_data(EVT_MPM_AUTOBRAKE_SELECTOR, 1);
	//			}
	//		}
	//	}
	//	status = hid->pSwitch_Get_State(mipSession, 11, &nDirection);
	//	if (status == IITAPI_ERR_OK)
	//	{
	//		if (nDirection == 1)
	//		{
	//			// ON
	//			if ( aircraft_model.hw_switch_data.autobrake_pos != 2)
	//			{
	//				//   simconnect_send_event_data(EVT_MPM_AUTOBRAKE_SELECTOR, 2);
	//			}
	//		}
	//	}
	//	status = hid->pSwitch_Get_State(mipSession, 12, &nDirection);
	//	if (status == IITAPI_ERR_OK)
	//	{
	//		if (nDirection == 1)
	//		{
	//			// ON
	//			if ( aircraft_model.hw_switch_data.autobrake_pos != 3)
	//			{
	//				// simconnect_send_event_data(EVT_MPM_AUTOBRAKE_SELECTOR, 3);
	//			}
	//		}
	//	}
	//	status = hid->pSwitch_Get_State(mipSession, 13, &nDirection);
	//	if (status == IITAPI_ERR_OK)
	//	{
	//		if (nDirection == 1)
	//		{
	//			// ON
	//			if ( aircraft_model.hw_switch_data.autobrake_pos != 4)
	//			{
	//				//  simconnect_send_event_data(EVT_MPM_AUTOBRAKE_SELECTOR, 4);
	//			}
	//		}
	//	}
	//	status = hid->pSwitch_Get_State(mipSession, 14, &nDirection);
	//	if (status == IITAPI_ERR_OK)
	//	{
	//		if (nDirection == 1)
	//		{
	//			// ON
	//			if ( aircraft_model.hw_switch_data.autobrake_pos != 5)
	//			{
	//				//   simconnect_send_event_data(EVT_MPM_AUTOBRAKE_SELECTOR, 5);
	//			}
	//		}
	//	}
	//	status = hid->pSwitch_Get_State(mipSession, 21, &nDirection);
	//	if (status == IITAPI_ERR_OK)
	//	{
	//		if (nDirection == 0)
	//		{
	//			// ON
	//			if ( aircraft_model.hw_switch_data.bright_test == 0) // 0: TEST  1: BRT  2: DIM
	//			{
	//				simconnect_send_event_data(EVT_DSP_CPT_MASTER_LIGHTS_SWITCH, 1);
	//			}
	//		}
	//		else
	//		{
	//			if ( aircraft_model.hw_switch_data.bright_test != 0)
	//			{
	//				simconnect_send_event_data(EVT_DSP_CPT_MASTER_LIGHTS_SWITCH, 0);
	//			}
	//		}
	//	}
	//	status = hid->pSwitch_Get_State(mipSession, 28, &nDirection);
	//	status2 = hid->pSwitch_Get_State(mipSession, 31, &nDirection2);
	//	if (status == IITAPI_ERR_OK && status2 == IITAPI_ERR_OK)
	//	{
	//		if (nDirection == 0)
	//		{
	//			if (nDirection2 == 0)
	//			{
	//				// ON
	//				if ( aircraft_model.hw_switch_data.landing_gear != 1) // 0: UP  1: OFF  2: DOWN
	//				{
	//					if ( aircraft_model.hw_switch_data.landing_gear == 2)
	//					{
	//						simconnect_send_event_data(EVT_GEAR_LEVER, MOUSE_FLAG_RIGHTSINGLE); // Move up (down -> off)
	//					}
	//					else
	//					{
	//						simconnect_send_event_data(EVT_GEAR_LEVER, MOUSE_FLAG_LEFTSINGLE); // Move down (up -> off)
	//					}
	//				}
	//			}
	//			else
	//			{
	//				// Down
	//				if ( aircraft_model.hw_switch_data.landing_gear != 2)
	//				{
	//					if ( aircraft_model.hw_switch_data.landing_gear == 0)
	//					{
	//						simconnect_send_event_data(EVT_GEAR_LEVER, MOUSE_FLAG_LEFTSINGLE); // Move down (up -> off)
	//					}
	//					simconnect_send_event_data(EVT_GEAR_LEVER, MOUSE_FLAG_LEFTSINGLE); // Move down (off -> down)
	//				}
	//			}
	//		}
	//		else
	//		{
	//			// UP
	//			if ( aircraft_model.hw_switch_data.landing_gear != 0)
	//			{
	//				simconnect_send_event_data(EVT_GEAR_LEVER, MOUSE_FLAG_RIGHTSINGLE); // Move up (down -> up)
	//			}
	//		}
	//	}
	//}


	void InterfaceIT::syncOverheadSwitches()
	{

	}


}