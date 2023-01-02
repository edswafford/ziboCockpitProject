
#include <array>
#include <vector>
#include "aircraft_parameters.hpp"

namespace zcockpit::cockpit {


	// -----------------------------------------------------------------------------------------------
	//
	// HARDWARE SWITCH VALUES (We need to make sure xplane matches these values)
	//
	// -----------------------------------------------------------------------------------------------
	struct Z737SwitchData
	{
		int battery_pos{0};			// Writable:	GUARD must be UP --  off=-1, bat=0, on-1
		int bright_test{0};			// READ ONLY:	dim=-1, brt=0, test=1
		int spd_ref{0};				// Writable:	auto=0 v1=1,vr=2,wt=3,vref=4,bug=5,set=6 
		int autobrake_pos{1};		// READ ONLY:	rto=0, off=1, (position 1)=2, (position 2)=3,(position 3)=4, max=5
		int landing_gear{2};		// Writable:	up=0, off=1, dn=2		
		int fuel_flow_pos{0};		// Writable:	reset=-1, rate=0, 0used=1

		int master_caution1_pb{0};		// READ ONLY:	pushbutton  pushed=1
		int capt_six_pack_pb{0};
		int chrono_capt_et_mode_pb{0};
		int fire_bell_light1_pb{0};
		int MFD_ENG_pb{0};
		int MFD_SYS_pb{0};
		int ap_light_pilot{0};
		int at_light_pilot{0};
		int fms_light_pilot{0};

		int ap_discon_test1{0};
		std::vector<float> cover_position{.0,.0,.0,.0,.0,.0,.0,.0,.0,.0,.0};
		float guarded_covers{0.0f};

		int land_lights_ret_left_pos{0};
		int land_lights_ret_right_pos{0};
		int land_lights_left_pos{0};
		int land_lights_right_pos{0};
		int rwy_light_left{0};
		int rwy_light_right{0};
		int taxi_light_brightness_pos{0};
		int APU_start_pos{0};
		int hydro_pumps1_pos{0};
		int hydro_pumps2_pos{0};
		int electric_hydro_pumps1_pos{0};
		int electric_hydro_pumps2_pos{0};
		int eng_start_source{0};
		int fmc_source{0};
		int logo_light{0};
		int beacon_on{0};
		int wing_light{0};
		int wheel_light{0};
		int eq_cool_supply{0};
		int eq_cool_exhaust{0};
		int emer_exit_lights{0};
		int seatbelt_sign_pos{0};
		int no_smoking_pos{0};
		int attend{0};
		int grd_call{0};
		int crossfeed_on{0};
		int fuel_tank_pos_lft1{0};
		int fuel_tank_pos_lft2{0};
		int fuel_tank_pos_rgt2{0};
		int fuel_tank_pos_rgt1{0};
		int fuel_tank_pos_ctr1{0};
		int fuel_tank_pos_ctr2{0};
		int yaw_dumper_pos{0};
		int cab_util_pos{0};
		int standby_bat_pos{0};
		int gpu_pos{0};
		int gen1_pos{0};
		int gen2_pos{0};
		int apu_gen1_pos{0};
		int apu_gen2_pos{0};
		int window_heat_l_side_pos{0};
		int window_heat_l_fwd_pos{0};
		int window_heat_r_fwd_pos{0};
		int window_heat_r_side_pos{0};
		int window_ovht_test{0};
		int capt_probes_pos{0};
		int fo_probes_pos{0};
		int main_pnl_du_capt{0};
		int main_pnl_du_fo{0};
		int lower_du_capt{0};
		int lower_du_fo{0};
		int ap_discon_test2{0};
		int ap_light_fo{0};
		int at_light_fo{0};
		int fms_light_fo{0};
		int gpws_test{0};
		int gpws_test_pos{0};
		int gpws_flap{0};
		int gpws_flap_pos{0};
		int gpws_terr{0};
		int gpws_terr_pos{0}; 
		int gpws_gear{0};
		int gpws_gear_pos{0};
		int n1_set_source{0};

		float rudder_trim{0.0};
		float aileron_trim{0.0};
		std::vector<float>  instrument_brightness{1.0};
		float spd_ref_adjust{0.0};
		float n1_set_adjust{0.0};
	};


	struct SwitchCommands
	{
		int size{1};  // default to pushbutton

		// for up/dn or right/left command pairs
		// names[0] first entry == increasing values i.e. battery_up
		// names[1] second entry == decreasing values i.e. battery_dn
		std::vector<CommandRefName> names; 
						
	};

	struct Z737SwitchCmd
	{
		SwitchCommands battery_pos{2, std::vector<CommandRefName>{CommandRefName::battery_dn, CommandRefName::battery_up}};
;
		SwitchCommands bright_test{2, std::vector<CommandRefName>{CommandRefName::bright_test_up, CommandRefName::bright_test_dn}};		
		SwitchCommands spd_ref{2, std::vector<CommandRefName>{CommandRefName::spd_ref_right, CommandRefName::spd_ref_left}};			
		SwitchCommands autobrake_pos{6, std::vector<CommandRefName>{CommandRefName::autobrake_rto, CommandRefName::autobrake_off,
		CommandRefName::autobrake_1, CommandRefName::autobrake_2, CommandRefName::autobrake_3, CommandRefName::autobrake_max}};
		SwitchCommands landing_gear{3, std::vector<CommandRefName>{CommandRefName::gear_up, CommandRefName::gear_off, CommandRefName::gear_down }};
		SwitchCommands fuel_flow_pos{2, std::vector<CommandRefName>{CommandRefName::fuel_flow_dn, CommandRefName::fuel_flow_up }};


		// Capt
		SwitchCommands master_caution1_pb{1, std::vector<CommandRefName>{CommandRefName::master_caution1}};
		SwitchCommands capt_six_pack_pb{1, std::vector<CommandRefName>{CommandRefName::capt_six_pack}};
		SwitchCommands chrono_capt_et_mode_pb{1, std::vector<CommandRefName>{CommandRefName::chrono_capt_et_mode}};
		SwitchCommands fire_bell_light1_pb{1, std::vector<CommandRefName>{CommandRefName::fire_bell_light1}};
		// FO
		SwitchCommands master_caution2_pb{1, std::vector<CommandRefName>{CommandRefName::master_caution2}};
		SwitchCommands fo_six_pack_pb{1, std::vector<CommandRefName>{CommandRefName::fo_six_pack}};
		SwitchCommands chrono_fo_et_mode_pb{1, std::vector<CommandRefName>{CommandRefName::chrono_fo_et_mode}};
		SwitchCommands fire_bell_light2_pb{1, std::vector<CommandRefName>{CommandRefName::fire_bell_light2}};

		SwitchCommands MFD_ENG_pb{1, std::vector<CommandRefName>{CommandRefName::MFD_ENG}};
		SwitchCommands MFD_SYS_pb{1, std::vector<CommandRefName>{CommandRefName::MFD_SYS}};

		SwitchCommands ap_discon_test1{2, std::vector<CommandRefName>{CommandRefName::ap_disconnect_test1_up, CommandRefName::ap_disconnect_test1_dn}};
		SwitchCommands ap_discon_test2{2, std::vector<CommandRefName>{CommandRefName::ap_disconnect_test2_up, CommandRefName::ap_disconnect_test2_dn}};


		// need to be processed like spring loaded switches
		SwitchCommands ap_light_pilot{2, std::vector<CommandRefName>{CommandRefName::ap_light_pilot_pb, CommandRefName::ap_light_pilot_pb }};
		SwitchCommands at_light_pilot{2, std::vector<CommandRefName>{CommandRefName::at_light_pilot_pb, CommandRefName::at_light_pilot_pb }};
		SwitchCommands fms_light_pilot{2, std::vector<CommandRefName>{CommandRefName::fms_light_pilot_pb, CommandRefName::fms_light_pilot_pb }};
		// FO
		SwitchCommands ap_light_fo{2, std::vector<CommandRefName>{CommandRefName::ap_light_fo_pb, CommandRefName::ap_light_fo_pb }};
		SwitchCommands at_light_fo{2, std::vector<CommandRefName>{CommandRefName::at_light_fo_pb, CommandRefName::at_light_fo_pb }};
		SwitchCommands fms_light_fo{2, std::vector<CommandRefName>{CommandRefName::fms_light_fo_pb, CommandRefName::fms_light_fo_pb }};


		SwitchCommands land_lights_ret_left_pos{2, std::vector<CommandRefName>{CommandRefName::land_lights_ret_left_on, CommandRefName::land_lights_ret_left_off}};
		SwitchCommands land_lights_ret_right_pos{2, std::vector<CommandRefName>{CommandRefName::land_lights_ret_right_on, CommandRefName::land_lights_ret_right_off}};
		SwitchCommands land_lights_left_pos{2, std::vector<CommandRefName>{CommandRefName::land_lights_left_on, CommandRefName::land_lights_left_off}};
		SwitchCommands land_lights_right_pos{2, std::vector<CommandRefName>{CommandRefName::land_lights_right_on, CommandRefName::land_lights_right_off}};

		SwitchCommands rwy_light_left{2,std::vector<CommandRefName>{CommandRefName::rwy_light_left_on, CommandRefName::rwy_light_left_off}};
		SwitchCommands rwy_light_right{2,std::vector<CommandRefName>{CommandRefName::rwy_light_right_on, CommandRefName::rwy_light_right_off}};


		
		SwitchCommands taxi_light_brightness_pos{2,std::vector<CommandRefName>{CommandRefName::taxi_light_brightness_on, CommandRefName::taxi_light_brightness_off}};
		SwitchCommands APU_start_pos{2,std::vector<CommandRefName>{CommandRefName::APU_start_pos_dn, CommandRefName::APU_start_pos_up}};
		SwitchCommands hydro_pumps1_pos{2,	std::vector<CommandRefName>{CommandRefName::hydro_pumps1, CommandRefName::hydro_pumps1}};
		SwitchCommands hydro_pumps2_pos{2,	std::vector<CommandRefName>{CommandRefName::hydro_pumps2,CommandRefName::hydro_pumps2}};

		SwitchCommands electric_hydro_pumps1_pos{2,std::vector<CommandRefName>{CommandRefName::electric_hydro_pumps1, CommandRefName::electric_hydro_pumps1}};
		SwitchCommands electric_hydro_pumps2_pos{2,std::vector<CommandRefName>{CommandRefName::electric_hydro_pumps2, CommandRefName::electric_hydro_pumps2}};

		SwitchCommands eng_start_source{2,	std::vector<CommandRefName>{CommandRefName::eng_start_source_left, CommandRefName::eng_start_source_right}};
		SwitchCommands fmc_source{2,std::vector<CommandRefName>{CommandRefName::fmc_source_right, CommandRefName::fmc_source_left}};
		SwitchCommands logo_light{2,std::vector<CommandRefName>{CommandRefName::logo_light_on, CommandRefName::logo_light_off}};

		SwitchCommands beacon_on{ 2,std::vector<CommandRefName>{CommandRefName::beacon_lights_toggle, CommandRefName::beacon_lights_toggle} };


		SwitchCommands wing_light{2,std::vector<CommandRefName>{CommandRefName::wing_light_on, CommandRefName::wing_light_off}};
		SwitchCommands wheel_light{2,std::vector<CommandRefName>{CommandRefName::wheel_light_off, CommandRefName::wheel_light_on}};
		SwitchCommands eq_cool_supply{1,std::vector<CommandRefName>{CommandRefName::eq_cool_supply}};
		SwitchCommands eq_cool_exhaust{1,std::vector<CommandRefName>{CommandRefName::eq_cool_exhaust}};
		SwitchCommands emer_exit_lights{2,	std::vector<CommandRefName>{CommandRefName::emer_exit_lights_up, CommandRefName::emer_exit_lights_dn}};
		SwitchCommands seatbelt_sign_pos{2,std::vector<CommandRefName>{CommandRefName::seatbelt_sign_up, CommandRefName::seatbelt_sign_dn}};
		SwitchCommands no_smoking_pos{2,std::vector<CommandRefName>{CommandRefName::no_smoking_up, CommandRefName::no_smoking_dn}};

		SwitchCommands attend{1,std::vector<CommandRefName>{CommandRefName::attend}};
		SwitchCommands grd_call{ 2,std::vector<CommandRefName>{CommandRefName::grd_call, CommandRefName::grd_call} };

		SwitchCommands crossfeed_on{2,	std::vector<CommandRefName>{CommandRefName::crossfeed_valve_on, CommandRefName::crossfeed_valve_off}};
		SwitchCommands fuel_tank_pos_lft1{2,std::vector<CommandRefName>{CommandRefName::fuel_pump_lft1, CommandRefName::fuel_pump_lft1}};
		SwitchCommands fuel_tank_pos_lft2{2,std::vector<CommandRefName>{CommandRefName::fuel_pump_lft2, CommandRefName::fuel_pump_lft2}};
		SwitchCommands fuel_tank_pos_rgt2{2,std::vector<CommandRefName>{CommandRefName::fuel_pump_rgt2, CommandRefName::fuel_pump_rgt2}};
		SwitchCommands fuel_tank_pos_rgt1{2,std::vector<CommandRefName>{CommandRefName::fuel_pump_rgt1, CommandRefName::fuel_pump_rgt1}};
		SwitchCommands fuel_tank_pos_ctr1{2,std::vector<CommandRefName>{CommandRefName::fuel_pump_ctr1, CommandRefName::fuel_pump_ctr1}};
		SwitchCommands fuel_tank_pos_ctr2{2,std::vector<CommandRefName>{CommandRefName::fuel_pump_ctr2, CommandRefName::fuel_pump_ctr2}};
		SwitchCommands yaw_damper_on{2,std::vector<CommandRefName>{CommandRefName::yaw_dumper, CommandRefName::yaw_dumper}};
		SwitchCommands cab_util_pos{2,	std::vector<CommandRefName>{CommandRefName::cab_util_toggle, CommandRefName::cab_util_toggle}};
		SwitchCommands standby_bat_pos{2,std::vector<CommandRefName>{CommandRefName::standby_bat_right, CommandRefName::standby_bat_left}};
		SwitchCommands gpu_pos{2,std::vector<CommandRefName>{CommandRefName::gpu_up, CommandRefName::gpu_dn}};
		SwitchCommands gen1_pos{2,	std::vector<CommandRefName>{CommandRefName::gen1_up, CommandRefName::gen1_dn}};
		SwitchCommands gen2_pos{2,	std::vector<CommandRefName>{CommandRefName::gen2_up, CommandRefName::gen2_dn}};
		SwitchCommands apu_gen1_pos{2,std::vector<CommandRefName>{CommandRefName::apu_gen1_up, CommandRefName::apu_gen1_dn}};
		SwitchCommands apu_gen2_pos{2,std::vector<CommandRefName>{CommandRefName::apu_gen2_up, CommandRefName::apu_gen2_dn}};
		
		SwitchCommands window_heat_l_side_pos{2,std::vector<CommandRefName>{CommandRefName::window_heat_l_side, CommandRefName::window_heat_l_side}};
		SwitchCommands window_heat_l_fwd_pos{2,std::vector<CommandRefName>{CommandRefName::window_heat_l_fwd, CommandRefName::window_heat_l_fwd}};
		SwitchCommands window_heat_r_fwd_pos{2,std::vector<CommandRefName>{CommandRefName::window_heat_r_fwd, CommandRefName::window_heat_r_fwd}};
		SwitchCommands window_heat_r_side_pos{2,std::vector<CommandRefName>{CommandRefName::window_heat_r_side, CommandRefName::window_heat_r_side}};
		SwitchCommands window_ovht_test{2,	std::vector<CommandRefName>{CommandRefName::window_ovht_test_up, CommandRefName::window_ovht_test_dn}};
		SwitchCommands capt_probes_pos{2,std::vector<CommandRefName>{CommandRefName::capt_probes_pos, CommandRefName::capt_probes_pos}};
		SwitchCommands fo_probes_pos{2,std::vector<CommandRefName>{CommandRefName::fo_probes_pos, CommandRefName::fo_probes_pos}};
						  
		SwitchCommands main_pnl_du_capt{2, std::vector<CommandRefName>{CommandRefName::main_pnl_du_capt_right, CommandRefName::main_pnl_du_capt_left}};
		SwitchCommands main_pnl_du_fo{2, std::vector<CommandRefName>{CommandRefName::main_pnl_du_fo_right, CommandRefName::main_pnl_du_fo_left}};

		SwitchCommands lower_du_capt{2, std::vector<CommandRefName>{CommandRefName::lower_du_capt_right, CommandRefName::lower_du_capt_left}};
		SwitchCommands lower_du_fo{2, std::vector<CommandRefName>{CommandRefName::lower_du_fo_right, CommandRefName::lower_du_fo_left}};

		SwitchCommands gpws_test{1, std::vector<CommandRefName>{CommandRefName::gpws_test}};
		SwitchCommands gpws_flap{2, std::vector<CommandRefName>{CommandRefName::gpws_flap, CommandRefName::gpws_flap}};
		SwitchCommands gpws_terr{2, std::vector<CommandRefName>{CommandRefName::gpws_terr, CommandRefName::gpws_terr}};
		SwitchCommands gpws_gear{2, std::vector<CommandRefName>{CommandRefName::gpws_gear, CommandRefName::gpws_gear}};

		SwitchCommands rudder_trim{2, std::vector<CommandRefName>{CommandRefName::rudder_trim_right, CommandRefName::rudder_trim_left}};
		SwitchCommands aileron_trim{2, std::vector<CommandRefName>{CommandRefName::aileron_trim_right, CommandRefName::aileron_trim_left}};

		SwitchCommands n1_set_source{2, std::vector<CommandRefName>{CommandRefName::n1_set_source_right, CommandRefName::n1_set_source_left}};
		
	};


	struct SwitchValues
	{
		float max_value{1.0f};
		float min_value{-1.0f};
		int size{1};
	};
	struct Z737SwitchValue
	{
		SwitchValues guarded_covers{1.0f, 0.0, 1};
		SwitchValues instrument_brightness{1.0f, -1.0f, 6};
		SwitchValues spd_ref_adjust{1.0f, -1.0f, 1};
		SwitchValues n1_set_adjust{1.0f, -1.0f, 1};
	};
}
