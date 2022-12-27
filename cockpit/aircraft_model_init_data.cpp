#include "aircraft_model.hpp"

#include "xp_data_ref_repository.hpp"
#include "hardware/zcockpit_switch.hpp"



namespace zcockpit::cockpit {

	using namespace common;


	void AircraftModel::initialize_zcockpit_data()
	{
		//
		// SWITCHES HW State
		z_cockpit_data[DataRefName::battery_pos] = ZCockpitSwitchData(ZCockpitType::ZInt, &xplane_switch_data.battery_pos, &z737SwitchCmd.battery_pos);
		z_cockpit_data[DataRefName::bright_test] = ZCockpitSwitchData(ZCockpitType::ZInt, &xplane_switch_data.bright_test, &z737SwitchCmd.bright_test);
		z_cockpit_data[DataRefName::spd_ref] = ZCockpitSwitchData(ZCockpitType::ZInt, &xplane_switch_data.spd_ref, &z737SwitchCmd.spd_ref);
		z_cockpit_data[DataRefName::autobrake_pos] = ZCockpitSwitchData(ZCockpitType::ZInt, &xplane_switch_data.autobrake_pos, &z737SwitchCmd.autobrake_pos);
		z_cockpit_data[DataRefName::landing_gear] = ZCockpitSwitchData(ZCockpitType::ZFloat, &xplane_switch_data.landing_gear, &z737SwitchCmd.landing_gear);
		z_cockpit_data[DataRefName::fuel_flow_pos] = ZCockpitSwitchData(ZCockpitType::ZInt, &xplane_switch_data.fuel_flow_pos, &z737SwitchCmd.fuel_flow_pos);

		z_cockpit_data[DataRefName::land_lights_ret_left_pos] = ZCockpitSwitchData(ZCockpitType::ZInt, &xplane_switch_data.land_lights_ret_left_pos, &z737SwitchCmd.land_lights_ret_left_pos );
		z_cockpit_data[DataRefName::land_lights_ret_right_pos] = ZCockpitSwitchData(ZCockpitType::ZInt, &xplane_switch_data.land_lights_ret_right_pos, &z737SwitchCmd.land_lights_ret_right_pos );
		z_cockpit_data[DataRefName::land_lights_left_pos] = ZCockpitSwitchData(ZCockpitType::ZInt, &xplane_switch_data.land_lights_left_pos, &z737SwitchCmd.land_lights_left_pos );
		z_cockpit_data[DataRefName::land_lights_right_pos] = ZCockpitSwitchData(ZCockpitType::ZInt, &xplane_switch_data.land_lights_right_pos, &z737SwitchCmd.land_lights_right_pos );

		z_cockpit_data[DataRefName::rwy_light_left] = ZCockpitSwitchData(ZCockpitType::ZInt, &xplane_switch_data.rwy_light_left, &z737SwitchCmd.rwy_light_left);
		z_cockpit_data[DataRefName::rwy_light_right] = ZCockpitSwitchData(ZCockpitType::ZInt, &xplane_switch_data.rwy_light_right, &z737SwitchCmd.rwy_light_right);
		z_cockpit_data[DataRefName::taxi_light_brightness_pos] = ZCockpitSwitchData(ZCockpitType::ZInt,	&xplane_switch_data.taxi_light_brightness_pos,	&z737SwitchCmd.taxi_light_brightness_pos);
		z_cockpit_data[DataRefName::APU_start_pos] = ZCockpitSwitchData(ZCockpitType::ZInt,				&xplane_switch_data.APU_start_pos,				&z737SwitchCmd.APU_start_pos);
		z_cockpit_data[DataRefName::hydro_pumps1_pos] = ZCockpitSwitchData(ZCockpitType::ZInt,			&xplane_switch_data.hydro_pumps1_pos,			&z737SwitchCmd.hydro_pumps1_pos);
		z_cockpit_data[DataRefName::hydro_pumps2_pos] = ZCockpitSwitchData(ZCockpitType::ZInt,			&xplane_switch_data.hydro_pumps2_pos,			&z737SwitchCmd.hydro_pumps2_pos);
		z_cockpit_data[DataRefName::electric_hydro_pumps1_pos] = ZCockpitSwitchData(ZCockpitType::ZInt,	&xplane_switch_data.electric_hydro_pumps1_pos,	&z737SwitchCmd.electric_hydro_pumps1_pos);
		z_cockpit_data[DataRefName::electric_hydro_pumps2_pos] = ZCockpitSwitchData(ZCockpitType::ZInt,	&xplane_switch_data.electric_hydro_pumps2_pos,	&z737SwitchCmd.electric_hydro_pumps2_pos);
		z_cockpit_data[DataRefName::eng_start_source] = ZCockpitSwitchData(ZCockpitType::ZInt,			&xplane_switch_data.eng_start_source,			&z737SwitchCmd.eng_start_source);
		z_cockpit_data[DataRefName::fmc_source] = ZCockpitSwitchData(ZCockpitType::ZInt,					&xplane_switch_data.fmc_source,					&z737SwitchCmd.fmc_source);
		z_cockpit_data[DataRefName::logo_light] = ZCockpitSwitchData(ZCockpitType::ZInt,					&xplane_switch_data.logo_light,					&z737SwitchCmd.logo_light);

		z_cockpit_data[DataRefName::beacon_on] = ZCockpitSwitchData(ZCockpitType::ZInt, &xplane_switch_data.beacon_on, &z737SwitchCmd.beacon_on);

		z_cockpit_data[DataRefName::wing_light] = ZCockpitSwitchData(ZCockpitType::ZInt,					&xplane_switch_data.wing_light,					&z737SwitchCmd.wing_light);
		z_cockpit_data[DataRefName::wheel_light] = ZCockpitSwitchData(ZCockpitType::ZInt,					&xplane_switch_data.wheel_light,				&z737SwitchCmd.wheel_light);
		z_cockpit_data[DataRefName::eq_cool_supply] = ZCockpitSwitchData(ZCockpitType::ZInt,				&xplane_switch_data.eq_cool_supply,				&z737SwitchCmd.eq_cool_supply);
		z_cockpit_data[DataRefName::eq_cool_exhaust] = ZCockpitSwitchData(ZCockpitType::ZInt,				&xplane_switch_data.eq_cool_exhaust,			&z737SwitchCmd.eq_cool_exhaust);
		z_cockpit_data[DataRefName::emer_exit_lights] = ZCockpitSwitchData(ZCockpitType::ZInt,			&xplane_switch_data.emer_exit_lights,			&z737SwitchCmd.emer_exit_lights);
		z_cockpit_data[DataRefName::seatbelt_sign_pos] = ZCockpitSwitchData(ZCockpitType::ZInt,			&xplane_switch_data.seatbelt_sign_pos,			&z737SwitchCmd.seatbelt_sign_pos);
		z_cockpit_data[DataRefName::no_smoking_pos] = ZCockpitSwitchData(ZCockpitType::ZInt,				&xplane_switch_data.no_smoking_pos,				&z737SwitchCmd.no_smoking_pos);

		z_cockpit_data[DataRefName::attend] = ZCockpitSwitchData(ZCockpitType::ZInt,				&xplane_switch_data.attend,				&z737SwitchCmd.attend);
		z_cockpit_data[DataRefName::grd_call] = ZCockpitSwitchData(ZCockpitType::ZInt,				&xplane_switch_data.grd_call,				&z737SwitchCmd.grd_call);


		z_cockpit_data[DataRefName::cross_feed_pos] = ZCockpitSwitchData(ZCockpitType::ZInt,				&xplane_switch_data.crossfeed_on,				&z737SwitchCmd.crossfeed_on);
		z_cockpit_data[DataRefName::fuel_tank_pos_lft1] = ZCockpitSwitchData(ZCockpitType::ZInt,			&xplane_switch_data.fuel_tank_pos_lft1,			&z737SwitchCmd.fuel_tank_pos_lft1);
		z_cockpit_data[DataRefName::fuel_tank_pos_lft2] = ZCockpitSwitchData(ZCockpitType::ZInt,			&xplane_switch_data.fuel_tank_pos_lft2,			&z737SwitchCmd.fuel_tank_pos_lft2);
		z_cockpit_data[DataRefName::fuel_tank_pos_rgt2] = ZCockpitSwitchData(ZCockpitType::ZInt,			&xplane_switch_data.fuel_tank_pos_rgt2,			&z737SwitchCmd.fuel_tank_pos_rgt2);
		z_cockpit_data[DataRefName::fuel_tank_pos_rgt1] = ZCockpitSwitchData(ZCockpitType::ZInt,			&xplane_switch_data.fuel_tank_pos_rgt1,			&z737SwitchCmd.fuel_tank_pos_rgt1);
		z_cockpit_data[DataRefName::fuel_tank_pos_ctr1] = ZCockpitSwitchData(ZCockpitType::ZInt,			&xplane_switch_data.fuel_tank_pos_ctr1,			&z737SwitchCmd.fuel_tank_pos_ctr1);
		z_cockpit_data[DataRefName::fuel_tank_pos_ctr2] = ZCockpitSwitchData(ZCockpitType::ZInt,			&xplane_switch_data.fuel_tank_pos_ctr2,			&z737SwitchCmd.fuel_tank_pos_ctr2);
		z_cockpit_data[DataRefName::yaw_dumper_pos] = ZCockpitSwitchData(ZCockpitType::ZInt,				&xplane_switch_data.yaw_dumper_pos,				&z737SwitchCmd.yaw_damper_on);
		z_cockpit_data[DataRefName::cab_util_pos] = ZCockpitSwitchData(ZCockpitType::ZInt,				&xplane_switch_data.cab_util_pos,				&z737SwitchCmd.cab_util_pos);
		z_cockpit_data[DataRefName::standby_bat_pos] = ZCockpitSwitchData(ZCockpitType::ZInt,				&xplane_switch_data.standby_bat_pos,			&z737SwitchCmd.standby_bat_pos);
		z_cockpit_data[DataRefName::gpu_pos] = ZCockpitSwitchData(ZCockpitType::ZInt,						&xplane_switch_data.gpu_pos,					&z737SwitchCmd.gpu_pos);
		z_cockpit_data[DataRefName::gen1_pos] = ZCockpitSwitchData(ZCockpitType::ZInt,					&xplane_switch_data.gen1_pos,					&z737SwitchCmd.gen1_pos);
		z_cockpit_data[DataRefName::gen2_pos] = ZCockpitSwitchData(ZCockpitType::ZInt,					&xplane_switch_data.gen2_pos,					&z737SwitchCmd.gen2_pos);
		z_cockpit_data[DataRefName::apu_gen1_pos] = ZCockpitSwitchData(ZCockpitType::ZInt,				&xplane_switch_data.apu_gen1_pos,				&z737SwitchCmd.apu_gen1_pos);
		z_cockpit_data[DataRefName::apu_gen2_pos] = ZCockpitSwitchData(ZCockpitType::ZInt,				&xplane_switch_data.apu_gen2_pos,				&z737SwitchCmd.apu_gen2_pos);
		z_cockpit_data[DataRefName::window_heat_l_side_pos] = ZCockpitSwitchData(ZCockpitType::ZInt,		&xplane_switch_data.window_heat_l_side_pos,		&z737SwitchCmd.window_heat_l_side_pos);
		z_cockpit_data[DataRefName::window_heat_l_fwd_pos] = ZCockpitSwitchData(ZCockpitType::ZInt,		&xplane_switch_data.window_heat_l_fwd_pos,		&z737SwitchCmd.window_heat_l_fwd_pos);
		z_cockpit_data[DataRefName::window_heat_r_fwd_pos] = ZCockpitSwitchData(ZCockpitType::ZInt,		&xplane_switch_data.window_heat_r_fwd_pos,		&z737SwitchCmd.window_heat_r_fwd_pos);
		z_cockpit_data[DataRefName::window_heat_r_side_pos] = ZCockpitSwitchData(ZCockpitType::ZInt,		&xplane_switch_data.window_heat_r_side_pos,		&z737SwitchCmd.window_heat_r_side_pos);
		z_cockpit_data[DataRefName::window_ovht_test] = ZCockpitSwitchData(ZCockpitType::ZInt,			&xplane_switch_data.window_ovht_test,			&z737SwitchCmd.window_ovht_test);
		z_cockpit_data[DataRefName::capt_probes_pos] = ZCockpitSwitchData(ZCockpitType::ZInt,				&xplane_switch_data.capt_probes_pos,			&z737SwitchCmd.capt_probes_pos);
		z_cockpit_data[DataRefName::fo_probes_pos] = ZCockpitSwitchData(ZCockpitType::ZInt,				&xplane_switch_data.fo_probes_pos,				&z737SwitchCmd.fo_probes_pos);




		// PUSHBUTTONS	 -- we don't need a switch value to compare
		// master caution
		z_cockpit_data[DataRefName::master_caution1_pb] = ZCockpitSwitchData(ZCockpitType::ZBool, nullptr, &z737SwitchCmd.master_caution1_pb );
		z_cockpit_data[DataRefName::capt_six_pack_pb] = ZCockpitSwitchData(ZCockpitType::ZBool, nullptr, &z737SwitchCmd.capt_six_pack_pb );
		z_cockpit_data[DataRefName::chrono_capt_et_mode_pb] = ZCockpitSwitchData(ZCockpitType::ZBool, nullptr, &z737SwitchCmd.chrono_capt_et_mode_pb);
		
		z_cockpit_data[DataRefName::fire_bell_light1_pb] = ZCockpitSwitchData(ZCockpitType::ZBool, nullptr, &z737SwitchCmd.fire_bell_light1_pb );
		z_cockpit_data[DataRefName::MFD_ENG_pb] = ZCockpitSwitchData(ZCockpitType::ZBool, nullptr, &z737SwitchCmd.MFD_ENG_pb);
		z_cockpit_data[DataRefName::MFD_SYS_pb] = ZCockpitSwitchData(ZCockpitType::ZBool, nullptr, &z737SwitchCmd.MFD_SYS_pb);

		z_cockpit_data[DataRefName::ap_light_pilot] = ZCockpitSwitchData(ZCockpitType::ZBool, &xplane_switch_data.ap_light_pilot, &z737SwitchCmd.ap_light_pilot);
		z_cockpit_data[DataRefName::at_light_pilot] = ZCockpitSwitchData(ZCockpitType::ZBool, &xplane_switch_data.at_light_pilot, &z737SwitchCmd.at_light_pilot);
		z_cockpit_data[DataRefName::fms_light_pilot] = ZCockpitSwitchData(ZCockpitType::ZBool, &xplane_switch_data.fms_light_pilot, &z737SwitchCmd.fms_light_pilot);

		z_cockpit_data[DataRefName::ap_discon_test1] = ZCockpitSwitchData(ZCockpitType::ZInt, &xplane_switch_data.ap_discon_test1, &z737SwitchCmd.ap_discon_test1 );

//NOT USED		z_cockpit_data[DataRefName::cover_position] = ZCockpitSwitchData(ZCockpitType::ZVectorFloat, &xplane_switch_data.cover_position, nullptr);

		z_cockpit_data[DataRefName::guarded_covers] = ZCockpitSwitchData(ZCockpitType::ZInt, &xplane_switch_data.guarded_covers, nullptr );

		//z_cockpit_data[DataRefName::] = ZCockpitSwitchData(ZCockpitType::ZBool, nullptr, &z737SwitchCmd. );


		//
		// IN Data
		z_cockpit_data[DataRefName::acf_tailnum] = ZCockpitInData(&z737InData.tail_number, ZCockpitType::ZBool);

		z_cockpit_data[DataRefName::crossfeed] = ZCockpitInData(&z737InData.FUEL_annunXFEED_VALVE_OPEN, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::engine1_ovht] = ZCockpitInData(&z737InData.HYD_annunOVERHEAT_elec[0], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::engine2_ovht] = ZCockpitInData(&z737InData.HYD_annunOVERHEAT_elec[1], ZCockpitType::ZBool);

		z_cockpit_data[DataRefName::cabin_alt] = ZCockpitInData(&z737InData.cabin_alt, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::hyd_A_rud] = ZCockpitInData(&z737InData.FCTL_annunFC_LOW_PRESSURE[0], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::hyd_B_rud] = ZCockpitInData(&z737InData.FCTL_annunFC_LOW_PRESSURE[0], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::takeoff_config] = ZCockpitInData(&z737InData.takeoff_config, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::gps] = ZCockpitInData(&z737InData.IRS_annunGPS, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::pax_oxy] = ZCockpitInData(&z737InData.OXY_annunPASS_OXY_ON, ZCockpitType::ZBool);

		z_cockpit_data[DataRefName::left_gear_safe] = ZCockpitInData(&z737InData.Left_Gear_Safe, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::nose_gear_safe] = ZCockpitInData(&z737InData.Nose_Gear_Safe, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::right_gear_safe] = ZCockpitInData(&z737InData.Right_Gear_Safe, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::left_gear_transit] = ZCockpitInData(&z737InData.MAIN_annunGEAR_transit[0], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::nose_gear_transit] = ZCockpitInData(&z737InData.MAIN_annunGEAR_transit[1], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::right_gear_transit] = ZCockpitInData(&z737InData.MAIN_annunGEAR_transit[2], ZCockpitType::ZBool);

		z_cockpit_data[DataRefName::yaw_damp] = ZCockpitInData(&z737InData.FCTL_annunYAW_DAMPER, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::hyd_stdby_rud] = ZCockpitInData(&z737InData.FCTL_annunLOW_STBY_RUD_ON, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::feel_diff_press] = ZCockpitInData(&z737InData.FCTL_annunFEEL_DIFF_PRESS, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::auto_slat_fail] = ZCockpitInData(&z737InData.FCTL_annunAUTO_SLAT_FAIL, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::bat_discharge] = ZCockpitInData(&z737InData.ELEC_annunBAT_DISCHARGE, ZCockpitType::ZBool);

		z_cockpit_data[DataRefName::tr_unit] = ZCockpitInData(&z737InData.ELEC_annunTR_UNIT, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::elec] = ZCockpitInData(&z737InData.ELEC_annunELEC, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::standby_pwr_off] = ZCockpitInData(&z737InData.ELEC_annunSTANDBY_POWER_OFF, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::ground_power_avail] = ZCockpitInData(&z737InData.ELEC_annunGRD_POWER_AVAILABLE, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::apu_gen_off_bus] = ZCockpitInData(&z737InData.ELEC_annunAPU_GEN_OFF_BUS, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::apu_low_oil] = ZCockpitInData(&z737InData.APU_annunLOW_OIL_PRESSURE, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::apu_fault] = ZCockpitInData(&z737InData.APU_annunFAULT, ZCockpitType::ZBool);

		z_cockpit_data[DataRefName::emer_exit] = ZCockpitInData(&z737InData.LTS_annunEmerNOT_ARMED, ZCockpitType::ZBool);
		// Capt/FO PITOT controls all 4 lights
		z_cockpit_data[DataRefName::capt_pitot_off] = ZCockpitInData(&z737InData.ICE_annun_CAPT_PITOT, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::fo_pitot_off] = ZCockpitInData(&z737InData.ICE_annun_FO_PITOT, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::dual_bleed] = ZCockpitInData(&z737InData.AIR_annunDualBleed, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::ram_door_open1] = ZCockpitInData(&z737InData.AIR_annunRamDoorL, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::ram_door_open2] = ZCockpitInData(&z737InData.AIR_annunRamDoorR, ZCockpitType::ZBool);

		z_cockpit_data[DataRefName::six_pack_flt_cont] = ZCockpitInData(&z737InData.WARN_annunFLT_CONT, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::six_pack_irs] = ZCockpitInData(&z737InData.WARN_annunIRS, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::six_pack_fuel] = ZCockpitInData(&z737InData.WARN_annunFUEL, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::six_pack_elec] = ZCockpitInData(&z737InData.WARN_annunELEC, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::six_pack_apu] = ZCockpitInData(&z737InData.WARN_annunAPU, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::six_pack_fire] = ZCockpitInData(&z737InData.WARN_annunOVHT_DET, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::six_pack_ice] = ZCockpitInData(&z737InData.WARN_annunANTI_ICE, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::six_pack_hyd] = ZCockpitInData(&z737InData.WARN_annunHYD, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::six_pack_doors] = ZCockpitInData(&z737InData.WARN_annunDOORS, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::six_pack_eng] = ZCockpitInData(&z737InData.WARN_annunENG, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::six_pack_overhead] = ZCockpitInData(&z737InData.WARN_annunOVERHEAD, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::six_pack_air_cond] = ZCockpitInData(&z737InData.WARN_annunAIR_COND, ZCockpitType::ZBool);

		z_cockpit_data[DataRefName::speedbrake_armed] = ZCockpitInData(&z737InData.MAIN_annunSPEEDBRAKE_ARMED, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::spd_brk_not_arm] = ZCockpitInData(&z737InData.MAIN_annunSPEEDBRAKE_DO_NOT_ARM, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::speedbrake_extend] = ZCockpitInData(&z737InData.MAIN_annunSPEEDBRAKE_EXTENDED, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::stab_out_of_trim] = ZCockpitInData(&z737InData.MAIN_annunSTAB_OUT_OF_TRIM, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::anti_skid_inop] = ZCockpitInData(&z737InData.MAIN_annunANTI_SKID_INOP, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::auto_brake_disarm] = ZCockpitInData(&z737InData.MAIN_annunAUTO_BRAKE_DISARM, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::slats_transit] = ZCockpitInData(&z737InData.MAIN_annunLE_FLAPS_TRANSIT, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::slats_extend] = ZCockpitInData(&z737InData.MAIN_annunLE_FLAPS_EXT, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::gpws] = ZCockpitInData(&z737InData.GPWS_annunINOP, ZCockpitType::ZBool);

		z_cockpit_data[DataRefName::parking_brake] = ZCockpitInData(&z737InData.PED_annunParkingBrake, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::wheel_well_fire] = ZCockpitInData(&z737InData.FIRE_annunWHEEL_WELL, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::fire_fault_inop] = ZCockpitInData(&z737InData.FIRE_annunFAULT, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::extinguisher_circuit_annun_apu] = ZCockpitInData(&z737InData.FIRE_annunAPU_DET_INOP, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::apu_bottle_discharge] = ZCockpitInData(&z737InData.FIRE_annunAPU_BOTTLE_DISCHARGE, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::cargo_fault_detector] = ZCockpitInData(&z737InData.CARGO_annunDETECTOR_FAULT, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::cargo_fire] = ZCockpitInData(&z737InData.CARGO_annunDISCH, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::xpond_fail] = ZCockpitInData(&z737InData.XPDR_annunFAIL, ZCockpitType::ZBool);

		z_cockpit_data[DataRefName::fwd_entry] = ZCockpitInData(&z737InData.DOOR_annunFWD_ENTRY, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::aft_entry] = ZCockpitInData(&z737InData.DOOR_annunAFT_ENTRY, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::fwd_service] = ZCockpitInData(&z737InData.DOOR_annunFWD_SERVICE, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::aft_service] = ZCockpitInData(&z737InData.DOOR_annunAFT_SERVICE, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::airstair] = ZCockpitInData(&z737InData.DOOR_annunAIRSTAIR, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::left_fwd_overwing] = ZCockpitInData(&z737InData.DOOR_annunLEFT_FWD_OVERWING, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::right_fwd_overwing] = ZCockpitInData(&z737InData.DOOR_annunRIGHT_FWD_OVERWING, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::left_aft_overwing] = ZCockpitInData(&z737InData.DOOR_annunLEFT_AFT_OVERWING, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::right_aft_overwing] = ZCockpitInData(&z737InData.DOOR_annunRIGHT_AFT_OVERWING, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::fwd_cargo] = ZCockpitInData(&z737InData.DOOR_annunFWD_CARGO, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::aft_cargo] = ZCockpitInData(&z737InData.DOOR_annunAFT_CARGO, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::equip_door] = ZCockpitInData(&z737InData.DOOR_annunEQUIP, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::door_lock_fail] = ZCockpitInData(&z737InData.PED_annunLOCK_FAIL, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::door_auto_unlk] = ZCockpitInData(&z737InData.PED_annunAUTO_UNLK, ZCockpitType::ZBool);

		z_cockpit_data[DataRefName::autofail] = ZCockpitInData(&z737InData.AIR_annunAUTO_FAIL, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::off_sched_descent] = ZCockpitInData(&z737InData.AIR_annunOFFSCHED_DESCENT, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::altn_press] = ZCockpitInData(&z737InData.AIR_annunALTN, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::manual_press] = ZCockpitInData(&z737InData.AIR_annunMANUAL, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::irs_align_left] = ZCockpitInData(&z737InData.IRS_annunALIGN[0], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::irs_align_right] = ZCockpitInData(&z737InData.IRS_annunALIGN[1], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::irs_on_dc_left] = ZCockpitInData(&z737InData.IRS_annunON_DC[0], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::irs_on_dc_right] = ZCockpitInData(&z737InData.IRS_annunON_DC[1], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::irs_align_fail_left] = ZCockpitInData(&z737InData.IRS_annunFAULT[0], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::irs_align_fail_right] = ZCockpitInData(&z737InData.IRS_annunFAULT[1], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::irs_dc_fail_left] = ZCockpitInData(&z737InData.IRS_annunDC_FAIL[0], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::irs_dc_fail_right] = ZCockpitInData(&z737InData.IRS_annunDC_FAIL[1], ZCockpitType::ZBool);

		z_cockpit_data[DataRefName::reverser_fail_0] = ZCockpitInData(&z737InData.ENG_annunREVERSER[0], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::reverser_fail_1] = ZCockpitInData(&z737InData.ENG_annunREVERSER[1], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::fadec_fail_0] = ZCockpitInData(&z737InData.ENG_annunENGINE_CONTROL[0], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::fadec_fail_1] = ZCockpitInData(&z737InData.ENG_annunENGINE_CONTROL[1], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::eng1_valve_closed] = ZCockpitInData(&z737InData.FUEL_annunENG_VALVE_CLOSED[0], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::eng2_valve_closed] = ZCockpitInData(&z737InData.FUEL_annunENG_VALVE_CLOSED[1], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::spar1_valve_closed] = ZCockpitInData(&z737InData.FUEL_annunSPAR_VALVE_CLOSED[0], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::spar2_valve_closed] = ZCockpitInData(&z737InData.FUEL_annunSPAR_VALVE_CLOSED[1], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::bypass_filter_1] = ZCockpitInData(&z737InData.FUEL_annunFILTER_BYPASS[0], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::bypass_filter_2] = ZCockpitInData(&z737InData.FUEL_annunFILTER_BYPASS[1], ZCockpitType::ZBool);

		z_cockpit_data[DataRefName::low_fuel_press_l1] = ZCockpitInData(&z737InData.FUEL_annunLOWPRESS_Aft[0], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::low_fuel_press_r1] = ZCockpitInData(&z737InData.FUEL_annunLOWPRESS_Fwd[1], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::low_fuel_press_l2] = ZCockpitInData(&z737InData.FUEL_annunLOWPRESS_Fwd[0], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::low_fuel_press_r2] = ZCockpitInData(&z737InData.FUEL_annunLOWPRESS_Aft[1], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::low_fuel_press_c1] = ZCockpitInData(&z737InData.FUEL_annunLOWPRESS_Ctr[0], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::low_fuel_press_c2] = ZCockpitInData(&z737InData.FUEL_annunLOWPRESS_Ctr[1], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::trans_bus_off1] = ZCockpitInData(&z737InData.ELEC_annunTRANSFER_BUS_OFF[0], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::trans_bus_off2] = ZCockpitInData(&z737InData.ELEC_annunTRANSFER_BUS_OFF[1], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::source_off1] = ZCockpitInData(&z737InData.ELEC_annunSOURCE_OFF[0], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::source_off2] = ZCockpitInData(&z737InData.ELEC_annunSOURCE_OFF[1], ZCockpitType::ZBool);

		z_cockpit_data[DataRefName::gen_off_bus1] = ZCockpitInData(&z737InData.ELEC_annunGEN_BUS_OFF[0], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::gen_off_bus2] = ZCockpitInData(&z737InData.ELEC_annunGEN_BUS_OFF[1], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::drive1] = ZCockpitInData(&z737InData.ELEC_annunDRIVE[0], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::drive2] = ZCockpitInData(&z737InData.ELEC_annunDRIVE[1], ZCockpitType::ZBool);

		z_cockpit_data[DataRefName::window_heat_ovht_ls] = ZCockpitInData(&z737InData.ICE_annunOVERHEAT[0], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::window_heat_ovht_lf] = ZCockpitInData(&z737InData.ICE_annunOVERHEAT[1], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::window_heat_ovht_rf] = ZCockpitInData(&z737InData.ICE_annunOVERHEAT[2], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::window_heat_ovht_rs] = ZCockpitInData(&z737InData.ICE_annunOVERHEAT[3], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::window_heat_l_fwd] = ZCockpitInData(&z737InData.ICE_annunON[1], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::window_heat_r_fwd] = ZCockpitInData(&z737InData.ICE_annunON[2], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::window_heat_l_side] = ZCockpitInData(&z737InData.ICE_annunON[0], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::window_heat_r_side] = ZCockpitInData(&z737InData.ICE_annunON[3], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::wing_ice_on_L] = ZCockpitInData(&z737InData.ICE_annunVALVE_OPEN[0], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::wing_ice_on_R] = ZCockpitInData(&z737InData.ICE_annunVALVE_OPEN[1], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::cowl_ice_0] = ZCockpitInData(&z737InData.ICE_annunCOWL_ANTI_ICE[0], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::cowl_ice_1] = ZCockpitInData(&z737InData.ICE_annunCOWL_ANTI_ICE[1], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::cowl_ice_on_0] = ZCockpitInData(&z737InData.ICE_annunCOWL_VALVE_OPEN[0], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::cowl_ice_on_1] = ZCockpitInData(&z737InData.ICE_annunCOWL_VALVE_OPEN[1], ZCockpitType::ZBool);

		z_cockpit_data[DataRefName::hyd_press_a] = ZCockpitInData(&z737InData.HYD_annunLOW_PRESS_eng[0], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::hyd_press_b] = ZCockpitInData(&z737InData.HYD_annunLOW_PRESS_eng[1], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::hyd_el_press_a] = ZCockpitInData(&z737InData.HYD_annunLOW_PRESS_elec[0], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::hyd_el_press_b] = ZCockpitInData(&z737InData.HYD_annunLOW_PRESS_elec[1], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::pack_left] = ZCockpitInData(&z737InData.AIR_annunPackTripOff[0], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::pack_right] = ZCockpitInData(&z737InData.AIR_annunPackTripOff[1], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::wing_body_ovht_left] = ZCockpitInData(&z737InData.AIR_annunWingBodyOverheat[0], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::wing_body_ovht_right] = ZCockpitInData(&z737InData.AIR_annunWingBodyOverheat[1], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::bleed_trip_1] = ZCockpitInData(&z737InData.AIR_annunBleedTripOff[0], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::bleed_trip_2] = ZCockpitInData(&z737InData.AIR_annunBleedTripOff[1], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::engine1_fire] = ZCockpitInData(&z737InData.WARN_annunFIRE_WARN[0], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::engine2_fire] = ZCockpitInData(&z737InData.WARN_annunFIRE_WARN[1], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::master_caution_light] = ZCockpitInData(&z737InData.WARN_annunMASTER_CAUTION, ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::below_gs] = ZCockpitInData(&z737InData.MAIN_annunBELOW_GS, ZCockpitType::ZBool);

		z_cockpit_data[DataRefName::ap_disconnect1] = ZCockpitInData(&z737InData.MAIN_annun_AP_RED[0], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::ap_disconnect2] = ZCockpitInData(&z737InData.MAIN_annun_AP_RED[1], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::at_disconnect1] = ZCockpitInData(&z737InData.MAIN_annun_AT_RED[0], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::at_disconnect2] = ZCockpitInData(&z737InData.MAIN_annun_AT_RED[1], ZCockpitType::ZBool);

		z_cockpit_data[DataRefName::ap_warn1] = ZCockpitInData(&z737InData.MAIN_annun_AP_AMBER[0], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::ap_warn2] = ZCockpitInData(&z737InData.MAIN_annun_AP_AMBER[1], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::at_fms_warn1] = ZCockpitInData(&z737InData.MAIN_annun_AT_AMBER[0], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::at_fms_warn2] = ZCockpitInData(&z737InData.MAIN_annun_AT_AMBER[1], ZCockpitType::ZBool);

		z_cockpit_data[DataRefName::at_fms_disconnect1] = ZCockpitInData(&z737InData.MAIN_annun_FMC_AMBER[0], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::at_fms_disconnect2] = ZCockpitInData(&z737InData.MAIN_annun_FMC_AMBER[1], ZCockpitType::ZBool);


		z_cockpit_data[DataRefName::fms_exec_light_pilot] = ZCockpitInData(&z737InData.CDU_annunEXEC[0], ZCockpitType::ZBool);
		z_cockpit_data[DataRefName::fms_exec_light_copilot] = ZCockpitInData(&z737InData.CDU_annunEXEC[1], ZCockpitType::ZBool);

		z_cockpit_data[DataRefName::main_bus] = ZCockpitInData(&z737InData.main_bus, ZCockpitType::ZBool);
	}


}