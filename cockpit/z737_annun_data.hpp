#pragma once


namespace zcockpit::cockpit {


	struct Z737AnnunData
	{
		// NOT Defined
		bool FCTL_annunLOW_QUANTITY{false};
		bool FCTL_annunSPEED_TRIM_FAIL{false};
		bool FCTL_annunMACH_TRIM_FAIL{false};
		bool FCTL_annunFC_LOW_PRESSURE[2]{false};
		bool AIR_annunEquipCoolingSupplyOFF{false};
		bool AIR_annunEquipCoolingExhaustOFF{false};
		bool APU_annunMAINT{false};
		bool APU_annunOVERSPEED{false};
		bool AIR_annunZoneTemp[3]{false};
		bool ENG_annunALTN[2]{false};
		bool WARN_annunPSEU{false};

		//
		float ac_volt_value{0.0f};
		float dc_volt_value{0.0f};
		float ac_freq_value{0.0f};
		float dc_amp_value{0.0f};
		float ac_amp_value{0.0f};
		std::vector<float>  flap_indicator{0.0f};
		float brake_press{0.0f};
		float apu_temp{0.0f};
		float cabin_pressure_diff{0.0f};
		float cabin_alt{0.0f};
		float duct_press_L{0.0f};
		float duct_press_R{0.0f};
		float fuel_temp{0.0f};
		float cabin_temp{0.0f};
		float cabin_vvi{0.0f};

		float autothrottle_active;




		char tail_number[40];

		bool FUEL_annunXFEED_VALVE_OPEN{false};
		bool HYD_annunOVERHEAT_elec[2]{false};
		bool annun_cabin_alt{false};
		bool takeoff_config{false};
		bool IRS_annunGPS{false};
		bool OXY_annunPASS_OXY_ON{false};
		bool Left_Gear_Safe{false};
		bool Nose_Gear_Safe{false};
		bool Right_Gear_Safe{false};
		bool FCTL_annunYAW_DAMPER{false};
		bool FCTL_annunLOW_STBY_RUD_ON{false};
		bool FCTL_annunFEEL_DIFF_PRESS{false};
		bool FCTL_annunAUTO_SLAT_FAIL{false};
		bool ELEC_annunBAT_DISCHARGE{false};
		bool ELEC_annunTR_UNIT{false};
		bool ELEC_annunELEC{false};
		bool ELEC_annunSTANDBY_POWER_OFF{false};
		bool ELEC_annunGRD_POWER_AVAILABLE{false};
		bool ELEC_annunAPU_GEN_OFF_BUS{false};
		bool APU_annunLOW_OIL_PRESSURE{false};
		bool APU_annunFAULT{false};
		bool LTS_annunEmerNOT_ARMED{false};
		bool ICE_annun_CAPT_PITOT{false};
		bool ICE_annun_FO_PITOT{false};
		bool AIR_annunDualBleed{false};
		bool AIR_annunRamDoorL{false};
		bool AIR_annunRamDoorR{false};
		bool WARN_annunFLT_CONT{false};
		bool WARN_annunIRS{false};
		bool WARN_annunFUEL{false};
		bool WARN_annunELEC{false};
		bool WARN_annunAPU{false};
		bool WARN_annunOVHT_DET{false};
		bool WARN_annunANTI_ICE{false};
		bool WARN_annunHYD{false};
		bool WARN_annunDOORS{false};
		bool WARN_annunENG{false};
		bool WARN_annunOVERHEAD{false};
		bool WARN_annunAIR_COND{false};
		bool MAIN_annunSPEEDBRAKE_ARMED{false};
		bool MAIN_annunSPEEDBRAKE_DO_NOT_ARM{false};
		bool MAIN_annunSPEEDBRAKE_EXTENDED{false};
		bool MAIN_annunSTAB_OUT_OF_TRIM{false};
		bool MAIN_annunANTI_SKID_INOP{false};
		bool MAIN_annunAUTO_BRAKE_DISARM{false};
		bool MAIN_annunLE_FLAPS_TRANSIT{false};
		bool MAIN_annunLE_FLAPS_EXT{false};
		bool GPWS_annunINOP{false};
		bool PED_annunParkingBrake{false};
		bool FIRE_annunWHEEL_WELL{false};
		bool FIRE_annunFAULT{false};
		bool FIRE_annunAPU_DET_INOP{false};
		bool FIRE_annunAPU_BOTTLE_DISCHARGE{false};
		bool CARGO_annunDETECTOR_FAULT{false};
		bool CARGO_annunDISCH{false};
		bool XPDR_annunFAIL{false};
		bool DOOR_annunFWD_ENTRY{false};
		bool DOOR_annunFWD_SERVICE{false};
		bool DOOR_annunAIRSTAIR{false};
		bool DOOR_annunLEFT_FWD_OVERWING{false};
		bool DOOR_annunRIGHT_FWD_OVERWING{false};
		bool DOOR_annunFWD_CARGO{false};
		bool DOOR_annunEQUIP{false};
		bool DOOR_annunLEFT_AFT_OVERWING{false};
		bool DOOR_annunRIGHT_AFT_OVERWING{false};
		bool DOOR_annunAFT_CARGO{false};
		bool DOOR_annunAFT_ENTRY{false};
		bool DOOR_annunAFT_SERVICE{false};
		bool PED_annunLOCK_FAIL{false};
		bool PED_annunAUTO_UNLK{false};
		bool AIR_annunAUTO_FAIL{false};
		bool AIR_annunOFFSCHED_DESCENT{false};
		bool AIR_annunALTN{false};
		bool AIR_annunMANUAL{false};
		bool IRS_annunALIGN[2]{false, false};

		bool IRS_annunON_DC[2]{false, false};

		bool IRS_annunFAULT[2]{false, false};

		bool IRS_annunDC_FAIL[2]{false, false};

		bool ENG_annunREVERSER[2]{false, false};

		bool ENG_annunENGINE_CONTROL[2]{false, false};

		bool FUEL_annunENG_VALVE_CLOSED[2]{false, false};

		bool FUEL_annunSPAR_VALVE_CLOSED[2]{false, false};

		bool FUEL_annunFILTER_BYPASS[2]{false, false};

		bool FUEL_annunLOWPRESS_Fwd[2]{false, false};

		bool FUEL_annunLOWPRESS_Aft[2]{false, false};

		bool FUEL_annunLOWPRESS_Ctr[2]{false, false};

		bool ELEC_annunTRANSFER_BUS_OFF[2]{false, false};

		bool ELEC_annunSOURCE_OFF[2]{false, false};

		bool ELEC_annunGEN_BUS_OFF[2]{false, false};

		bool ELEC_annunDRIVE[2]{false, false};

		bool ICE_annunOVERHEAT[4]{false, false, false, false };

		bool ICE_annunON[4]{false, false, false, false };

		bool ICE_annunVALVE_OPEN[2]{false, false};

		bool ICE_annunCOWL_ANTI_ICE[2]{false, false};

		bool ICE_annunCOWL_VALVE_OPEN[2]{false, false};

		bool HYD_annunLOW_PRESS_eng[2]{false, false};

		bool HYD_annunLOW_PRESS_elec[2]{false, false};

		bool AIR_annunPackTripOff[2]{false, false};

		bool AIR_annunWingBodyOverheat[2]{false, false};
	
		bool AIR_annunBleedTripOff[2]{false, false};

		bool WARN_annunFIRE_WARN[2]{false, false};

		bool WARN_annunMASTER_CAUTION{false};

		bool MAIN_annunBELOW_GS{false};

		bool MAIN_annun_AP_RED[2]{false, false};

		bool MAIN_annun_AT_RED[2]{false, false};

		bool MAIN_annun_FMC_AMBER[2]{false, false};

		bool MAIN_annunGEAR_transit[3]{false, false, false};


		bool CDU_annunEXEC[2]{false, false};

		bool MAIN_annun_AP_AMBER[2]{false, false};

		bool MAIN_annun_AT_AMBER[2]{false, false};

		bool ac_tnsbus1_status{false};
		bool ac_tnsbus2_status{false};

	};
}
