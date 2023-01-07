#pragma once



namespace zcockpit::cockpit {

	enum class DataRefName : unsigned
	{
		acf_tailnum,


		//
		// Annunciators
		//

		engine1_ovht,
		engine2_ovht,
		cabin_alt,
		gps,
		engine1_fire,
		engine2_fire,
		tr_unit,
		elec,
		hyd_A_rud,
		hyd_B_rud,
		hyd_stdby_rud,
		feel_diff_press,
		auto_slat_fail,
		apu_low_oil,
		ram_door_open1,
		ram_door_open2,
		wing_body_ovht_left,
		wing_body_ovht_right,
		pack_left,
		pack_right,
		spd_brk_not_arm,
		stab_out_of_trim,
		anti_skid_inop,
		apu_bottle_discharge,
		extinguisher_circuit_annun_apu,
		cargo_fault_detector,
		airstair,
		autofail,
		door_auto_unlk,
		door_lock_fail,
		off_sched_descent,
		altn_press,
		manual_press,
		spar1_valve_closed,
		spar2_valve_closed,
		at_fms_warn1,
		at_fms_warn2,
		fms_exec_light_copilot,
		ap_warn1,
		ap_warn2,
		parking_brake,
		window_heat_l_side,
		window_heat_l_fwd,
		window_heat_r_side,
		window_heat_r_fwd,
		fadec_fail_0,
		fadec_fail_1,
		reverser_fail_0,
		reverser_fail_1,
		capt_pitot_off,
		fo_pitot_off,
		window_heat_ovht_rf,
		window_heat_ovht_rs,
		window_heat_ovht_lf,
		window_heat_ovht_ls,
		cowl_ice_0,
		cowl_ice_1,
		cowl_ice_on_0,
		cowl_ice_on_1,
		wing_ice_on_L,
		wing_ice_on_R,
		apu_fault,
		nose_gear_transit,
		nose_gear_safe,
		left_gear_transit,
		left_gear_safe,
		right_gear_transit,
		right_gear_safe,
		low_fuel_press_l1,
		low_fuel_press_l2,
		low_fuel_press_c1,
		low_fuel_press_c2,
		low_fuel_press_r1,
		low_fuel_press_r2,
		eng1_valve_closed,
		eng2_valve_closed,
		drive1,
		drive2,
		standby_pwr_off,
		bypass_filter_1,
		bypass_filter_2,
		bat_discharge,
		hyd_press_a,
		hyd_press_b,
		hyd_el_press_a,
		hyd_el_press_b,
		apu_gen_off_bus,
		gen_off_bus1,
		gen_off_bus2,
		source_off1,
		source_off2,
		trans_bus_off1,
		trans_bus_off2,
		fwd_entry,
		left_fwd_overwing,
		left_aft_overwing,
		aft_entry,
		fwd_service,
		right_fwd_overwing,
		right_aft_overwing,
		aft_service,
		fwd_cargo,
		aft_cargo,
		equip_door,
		pax_oxy,
		bleed_trip_1,
		bleed_trip_2,
		ground_power_avail,
		yaw_damp,
		emer_exit,
		speedbrake_armed,
		speedbrake_extend,
		gpws,
		slats_transit,
		slats_extend,
		cargo_fire,
		fire_fault_inop,
		wheel_well_fire,
		six_pack_fuel,
		six_pack_fire,
		six_pack_apu,
		six_pack_flt_cont,
		six_pack_elec,
		six_pack_irs,
		six_pack_ice,
		six_pack_doors,
		six_pack_eng,
		six_pack_hyd,
		six_pack_air_cond,
		six_pack_overhead,
		xpond_fail,
		auto_brake_disarm,
		dual_bleed,
		irs_align_fail_right,
		irs_align_fail_left,
		irs_align_right,
		irs_align_left,
		irs_dc_fail_left,
		irs_on_dc_left,
		irs_dc_fail_right,
		irs_on_dc_right,
		takeoff_config,
		below_gs,
		crossfeed,
		master_caution_light,
		ap_disconnect1,
		ap_disconnect2,
		at_disconnect1,
		at_disconnect2,
		at_fms_disconnect1,
		at_fms_disconnect2,
		fms_exec_light_pilot,
		ac_tnsbus1_status,
		ac_tnsbus2_status,


		//
		// end of DataRef names
		//

		//
		// HARDWARE SWITCHES
		//
		battery_pos,
		bright_test,			//    laminar/B738/toggle_switch/bright_test [1, 0, -1]  up==test, center==brt, dn==dim
		spd_ref,				//	  laminar/B738/toggle_switch/spd_ref [0,1,2,3,4,5,6] auto v1,vr,wt,vref,rto,off
		autobrake_pos,			//    laminar/B738/autobrake/autobrake_pos [0,1,2,3,4,5] rto, off, 1,2,3,max
		landing_gear,			//    laminar/B738/switches/landing_gear  0-up, 1-off, 2-down	
		fuel_flow_pos,

		// Capt
		ap_discon_test1,		//	laminar/B738/toggle_switch/ap_discon_test1  "capt TEST 1/2" up=1, 0=off, -1=dn
		ap_light_pilot,
		at_light_pilot,
		fms_light_pilot,

		// FO
		ap_discon_test2,
		ap_light_fo,
		at_light_fo,
		fms_light_fo,

		// guards [11]
		// [0] EEC #1			open = 1.0
		// [1] EEC #2			open = 1.0
		// [2] Battery_pos,		open = 1.0
		// [3] StandBy Power	open = 1.0
		// [4] Eng 1 Disconnect open = 1.0
		// [5] Eng 2 Disconnect open = 1.0
		// [6] Bus Transfer		open = 1.0
		// [7] Pass Oxygen		open = 1.0
		// [8] ELT				open = 1.0
		// [9] Emer Exit Lights	open = 1.0
		// [10] Nose Wheel Steering open = 1.0
		//NOT USED cover_position,		// array of guards[11]
		guarded_covers,		// set to 1.0 opens all guards and prevents closing
							// set to 0.0 allows guards to close, but does not close guards


		land_lights_ret_left_pos,
		land_lights_ret_right_pos,
		land_lights_left_pos,
		land_lights_right_pos,
		rwy_light_left,
		rwy_light_right,

		taxi_light_brightness_pos,
		APU_start_pos,
		hydro_pumps1_pos,
		hydro_pumps2_pos,
		electric_hydro_pumps1_pos,
		electric_hydro_pumps2_pos,
		eng_start_source,
		fmc_source,
		logo_light,
		beacon_on,
		wing_light,
		wheel_light,
		eq_cool_supply,
		eq_cool_exhaust,
		emer_exit_lights,
		seatbelt_sign_pos,
		no_smoking_pos,
		attend,
		grd_call,
		cross_feed_pos,
		fuel_tank_pos_lft1,
		fuel_tank_pos_lft2,
		fuel_tank_pos_rgt2,
		fuel_tank_pos_rgt1,
		fuel_tank_pos_ctr1,
		fuel_tank_pos_ctr2,
		yaw_dumper_pos,
		cab_util_pos,
		standby_bat_pos,
		gpu_pos,
		gen1_pos,
		gen2_pos,
		apu_gen1_pos,
		apu_gen2_pos,
		window_heat_l_side_pos,
		window_heat_l_fwd_pos,
		window_heat_r_fwd_pos,
		window_heat_r_side_pos,
		window_ovht_test,
		capt_probes_pos,
		fo_probes_pos,
		main_pnl_du_capt,
		main_pnl_du_fo,
		lower_du_capt,
		lower_du_fo,

		
		//
		// These are made up Datarefs used so we can look up the associated Command Datarefs
		// There are NO Xplane dataRef i.e. "laminar/B738/..." associated with these
		//
		master_caution1_pb,
		capt_six_pack_pb,
		chrono_capt_et_mode_pb,
		fire_bell_light1_pb,
		MFD_ENG_pb,
		MFD_SYS_pb,

		// FO
		master_caution2_pb,
		fo_six_pack_pb,
		chrono_fo_et_mode_pb,
		fire_bell_light2_pb,

		gpws_test,
		gpws_test_pos,
		gpws_flap_pos,
		gpws_terr_pos, 
		gpws_gear_pos,

		gpws_flap,
		gpws_terr,
		gpws_gear,

		rudder_trim,
		aileron_trim,
		instrument_brightness,
		spd_ref_adjust,
		n1_set_source,
		n1_set_adjust,

		air_valve_ctrl,
		dc_power,
		ac_power,
		l_pack_pos,
		r_pack_pos,
		isolation_valve_pos,
		position_light_pos,
		irs_left,
		irs_right,
		irs_dspl_sel,
		starter1_pos,
		starter2_pos,
		air_valve_manual,
		flt_ctr_B_pos,
		flt_ctr_A_pos,
		alt_flaps_ctrl,
		dspl_source,
		irs_source,
		vhf_nav_source,



		//
		// end of HARDWARE SWITCHES
		//

		DataRefName_unused,

		kMaxValue = DataRefName_unused,
	};




	// Used to iterate over the DataRef Switch Names
	static constexpr DataRefName data_ref_switch_list[] = {
		// HARDWARE SWITCHES
		DataRefName::battery_pos,
		DataRefName::bright_test,
		DataRefName::spd_ref,	
		DataRefName::autobrake_pos,
		DataRefName::landing_gear,	
		DataRefName::fuel_flow_pos,
		// Capt
		DataRefName::ap_discon_test1,
		DataRefName::ap_light_pilot,
		DataRefName::at_light_pilot,
		DataRefName::fms_light_pilot,
		//FO
		DataRefName::ap_discon_test2,
		DataRefName::ap_light_fo,
		DataRefName::at_light_fo,
		DataRefName::fms_light_fo,

		DataRefName::land_lights_ret_left_pos,
		DataRefName::land_lights_ret_right_pos,
		DataRefName::land_lights_left_pos,
		DataRefName::land_lights_right_pos,
		DataRefName::rwy_light_left,
		DataRefName::rwy_light_right,

		DataRefName::taxi_light_brightness_pos,
		DataRefName::APU_start_pos,
		DataRefName::hydro_pumps1_pos,
		DataRefName::hydro_pumps2_pos,
		DataRefName::electric_hydro_pumps1_pos,
		DataRefName::electric_hydro_pumps2_pos,
		DataRefName::eng_start_source,
		DataRefName::fmc_source,
		DataRefName::logo_light,
		DataRefName::beacon_on,
		DataRefName::wing_light,
		DataRefName::wheel_light,
		DataRefName::eq_cool_supply,
		DataRefName::eq_cool_exhaust,
		DataRefName::emer_exit_lights,
		DataRefName::seatbelt_sign_pos,
		DataRefName::no_smoking_pos,
		DataRefName::attend,
		DataRefName::grd_call,
		DataRefName::cross_feed_pos,
		DataRefName::fuel_tank_pos_lft1,
		DataRefName::fuel_tank_pos_lft2,
		DataRefName::fuel_tank_pos_rgt2,
		DataRefName::fuel_tank_pos_rgt1,
		DataRefName::fuel_tank_pos_ctr1,
		DataRefName::fuel_tank_pos_ctr2,
		DataRefName::yaw_dumper_pos,
		DataRefName::cab_util_pos,
		DataRefName::standby_bat_pos,
		DataRefName::gpu_pos,
		DataRefName::gen1_pos,
		DataRefName::gen2_pos,
		DataRefName::apu_gen1_pos,
		DataRefName::apu_gen2_pos,
		DataRefName::window_heat_l_side_pos,
		DataRefName::window_heat_l_fwd_pos,
		DataRefName::window_heat_r_fwd_pos,
		DataRefName::window_heat_r_side_pos,
		DataRefName::window_ovht_test,
		DataRefName::capt_probes_pos,
		DataRefName::fo_probes_pos,
		DataRefName::main_pnl_du_capt,
		DataRefName::main_pnl_du_fo,
		DataRefName::lower_du_capt,
		DataRefName::lower_du_fo,


		DataRefName::gpws_test,
		DataRefName::gpws_flap,
		DataRefName::gpws_terr,
		DataRefName::gpws_gear,

		DataRefName::rudder_trim,
		DataRefName::aileron_trim,
		DataRefName::instrument_brightness,
		DataRefName::spd_ref_adjust,
		DataRefName::n1_set_source,
		DataRefName::n1_set_adjust,

		DataRefName::air_valve_ctrl,
		DataRefName::dc_power,
		DataRefName::ac_power,
		DataRefName::l_pack_pos,
		DataRefName::r_pack_pos,
		DataRefName::isolation_valve_pos,
		DataRefName::position_light_pos,
		DataRefName::irs_left,
		DataRefName::irs_right,
		DataRefName::irs_dspl_sel,
		DataRefName::starter1_pos,
		DataRefName::starter2_pos,
		DataRefName::air_valve_manual,
		DataRefName::flt_ctr_A_pos,
		DataRefName::flt_ctr_B_pos,
		DataRefName::alt_flaps_ctrl,
		DataRefName::dspl_source,
		DataRefName::irs_source,
		DataRefName::vhf_nav_source,

	};




}