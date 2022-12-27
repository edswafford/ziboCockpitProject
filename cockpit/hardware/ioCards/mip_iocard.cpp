#include "mip_iocard.hpp"
#include "../DeleteMeHotkey.h"
#include "../common/logger.hpp"
extern logger LOG;

namespace zcockpit::cockpit::hardware
{
	std::string MipIOCard::iocard_bus_addr;
	bool MipIOCard::running = false; 

	MipIOCard::MipIOCard(std::string deviceBusAddr)
		: IOCards(deviceBusAddr, "mip")
	{
		MipIOCard::iocard_bus_addr = deviceBusAddr;

		// Capt Main Panel DUs
		captOutboardPFD = 0;
		captNormal = 0;
		captEngPrimary = 0;
		captPFD = 0;

		// Capt Lower DU
		captEngPrimaryLowerDU = 0;
		captPFDLowerDU = 0;

		// Fo Main Panel DUs
		foOutboardPFD = 0;
		foNormal = 0;
		foEngPrimary = 0;
		foPFD = 0;
		foMFD = 0;

		// FO Lower DU
		foEngPrimaryLowerDU = 0;
		foPFDLowerDU = 0;
		foNDLowerDU = 0;

		foClock = 0;
		FoMasterFire = 0;
		FoMasteCaution = 0;
		warningAnn = 0;
		eng2N2Set = 0;
		eng1N2Set = 0;
		N1SetAuto = 0;
		N1SetBoth = 0;
		apWarning = 0;
		atWarning = 0;
		fmcWarning = 0;
		afdsTest2 = 0;
		afdsTest1 = 0;
		gpws = 0;
		flapsInhibit = 0;
		terrainInhibit = 0;
		gearInhibit = 0;
		rudderLeft = 0;
		rudderRight = 0;
		aileronLeft = 0;
		aileronRight = 0;


		powerIsOn = false;
	}
	std::unique_ptr<MipIOCard> MipIOCard::create_mip_iocard(const std::string& bus_address)
	{
		MipIOCard::running = false;

		LOG() << "IOCards: creating fwd overhead";


		return nullptr;
	}
	void MipIOCard::processMIP()
	{
		//if(powerIsOn && client->electrical_power_state == 0)
		//{
		//	powerIsOn = false;
		//	// turn off relays
		//	updateRelays(0);
		//}
		//else if(!powerIsOn && client->electrical_power_state != 0)
		//{
		//	powerIsOn = true;
		//	// turn on relays;
		//	updateRelays(1);
		//}

		processOutputs();

		//  Capt Main Panel DUs rotary switch only one can change --> use if--else if
		//if(mastercard_input(1, &captOutboardPFD) && captOutboardPFD)
		//{
		//	sendMessageInt(KEY_COMMAND_INSTRUMENT_CAPT_MAIN_PANEL_DISPLAY_UNIT_POS1, 0);
		//}
		//else if(mastercard_input(2, &captNormal) && captNormal)
		//{
		//	sendMessageInt(KEY_COMMAND_INSTRUMENT_CAPT_MAIN_PANEL_DISPLAY_UNIT_POS2, 0);
		//}
		//else if(mastercard_input(3, &captEngPrimary) && captEngPrimary)
		//{
		//	sendMessageInt(KEY_COMMAND_INSTRUMENT_CAPT_MAIN_PANEL_DISPLAY_UNIT_POS3, 0);
		//}
		//else if(mastercard_input(4, &captPFD) && captPFD)
		//{
		//	if(captPFD)
		//	{
		//		sendMessageInt(KEY_COMMAND_INSTRUMENT_CAPT_MAIN_PANEL_DISPLAY_UNIT_POS4, 0);
		//	}
		//}
		//else if(captOutboardPFD == 0 && captNormal == 0 && captEngPrimary == 0 && captPFD == 0)
		//{
		//	// MFD 
		//	sendMessageInt(KEY_COMMAND_INSTRUMENT_CAPT_MAIN_PANEL_DISPLAY_UNIT_POS5, 0);
		//}

		////  Capt Lower DUs rotary switch only one can change --> use if--else if
		//if(mastercard_input(5, &captEngPrimaryLowerDU) && captEngPrimaryLowerDU)
		//{
		//	sendMessageInt(KEY_COMMAND_INSTRUMENT_LOWER_DISPLAY_UNIT_POS3, 0);
		//}
		//else if(mastercard_input(6, &captPFDLowerDU))
		//{
		//	if(captPFDLowerDU)
		//	{
		//		sendMessageInt(KEY_COMMAND_INSTRUMENT_LOWER_DISPLAY_UNIT_POS2, 0);
		//	}
		//	else if(captPFDLowerDU == 0 && captEngPrimaryLowerDU == 0)
		//	{
		//		// ND
		//		sendMessageInt(KEY_COMMAND_INSTRUMENT_LOWER_DISPLAY_UNIT_POS1, 0);
		//	}
		//}


		////  FO Main Panel DUs rotary switch only one can change --> use if--else if
		//if(mastercard_input(13, &foOutboardPFD) && foOutboardPFD)
		//{
		//	sendMessageInt(KEY_COMMAND_INSTRUMENT_FO_MAIN_PANEL_DISPLAY_UNIT_POS5, 0);
		//}
		//else if(mastercard_input(9, &foNormal) && foNormal)
		//{
		//	sendMessageInt(KEY_COMMAND_INSTRUMENT_FO_MAIN_PANEL_DISPLAY_UNIT_POS4, 0);
		//}
		//else if(mastercard_input(10, &foEngPrimary) && foEngPrimary)
		//{
		//	sendMessageInt(KEY_COMMAND_INSTRUMENT_FO_MAIN_PANEL_DISPLAY_UNIT_POS3, 0);
		//}
		//else if(mastercard_input(11, &foPFD) && foPFD)
		//{
		//	sendMessageInt(KEY_COMMAND_INSTRUMENT_FO_MAIN_PANEL_DISPLAY_UNIT_POS2, 0);
		//}
		//else if(mastercard_input(12, &foMFD) && foMFD)
		//{
		//	sendMessageInt(KEY_COMMAND_INSTRUMENT_FO_MAIN_PANEL_DISPLAY_UNIT_POS1, 0);
		//}

		//// during init use Capt switch
		//auto delayIsOver = true;//  Sim737::HasRunForOneSecond();

		////  FO Lower DUs rotary switch only one can change --> use if--else if
		//if(mastercard_input(15, &foEngPrimaryLowerDU) && foEngPrimaryLowerDU)
		//{
		//	if(delayIsOver)
		//	{
		//		sendMessageInt(KEY_COMMAND_INSTRUMENT_LOWER_DISPLAY_UNIT_POS3, 0);
		//	}
		//}
		//else if(mastercard_input(16, &foPFDLowerDU))
		//{
		//	if(delayIsOver)
		//	{
		//		sendMessageInt(KEY_COMMAND_INSTRUMENT_LOWER_DISPLAY_UNIT_POS2, 0);
		//	}
		//}
		//else if(mastercard_input(14, &foNDLowerDU))
		//{
		//	if(delayIsOver)
		//	{
		//		sendMessageInt(KEY_COMMAND_INSTRUMENT_LOWER_DISPLAY_UNIT_POS1, 0);
		//	}
		//}


		//// FO Clock
		//if(mastercard_input(19, &foClock) && foClock)
		//{
		//	//    &iflyCmd = 792
		//	sendMessageInt(KEY_COMMAND_INSTRUMENT_CLOCK_FO_CHR, 0);
		//}

		//// FO Master Fire Warning
		//if(mastercard_input(20, &FoMasterFire) && FoMasterFire)
		//{
		//	//    &iflyCmd = 564
		//	sendMessageInt(KEY_COMMAND_FIRE_MASTER_FIRE_WARN_LIGHT, 0);
		//}
		//// FO Master Caution
		//if(mastercard_input(18, &FoMasteCaution) && FoMasteCaution)
		//{
		//	//    &iflyCmd = 1179
		//	sendMessageInt(KEY_COMMAND_WARNING_MASTER_CAUTION, 0);
		//}

		//// FO 6-pack
		//if(mastercard_input(21, &warningAnn) && warningAnn)
		//{
		//	//    &iflyCmd = 1180
		//	sendMessageInt(KEY_COMMAND_WARNING_SYSTEM_ANNUNCIATOR, 0);
		//}

		//// N1 set
		//if(mastercard_input(27, &eng2N2Set) && eng2N2Set)
		//{
		//	//    &iflyCmd = 476
		//	sendMessageInt(KEY_COMMAND_ENGAPU_N1_MODE_SET_1, 0);
		//}
		//if(mastercard_input(29, &eng1N2Set) && eng1N2Set)
		//{
		//	//    &iflyCmd = 477
		//	sendMessageInt(KEY_COMMAND_ENGAPU_N1_MODE_SET_2, 0);
		//}
		//if(mastercard_input(28, &N1SetAuto) && N1SetAuto)
		//{
		//	//    &iflyCmd = 478
		//	sendMessageInt(KEY_COMMAND_ENGAPU_N1_MODE_SET_AUTO, 0);
		//}
		//if(mastercard_input(30, &N1SetBoth) && N1SetBoth)
		//{
		//	//    &iflyCmd = 479
		//	sendMessageInt(KEY_COMMAND_ENGAPU_N1_MODE_SET_BOTH, 0);
		//}


		//// AFDS Warnings
		//if(mastercard_input(23, &apWarning) && apWarning)
		//{
		//	//    &iflyCmd = 235
		//	sendMessageInt(KEY_COMMAND_AUTOMATICFLIGHT_AUTOPILOT_DIS_LIGHT, 0);
		//}
		//if(mastercard_input(24, &atWarning) && atWarning)
		//{
		//	//    &iflyCmd = 236
		//	sendMessageInt(KEY_COMMAND_AUTOMATICFLIGHT_AUTOTHROTTLE_DIS_LIGHT, 0);
		//}
		//if(mastercard_input(25, &fmcWarning) && fmcWarning)
		//{
		//	//    &iflyCmd = 830
		//	sendMessageInt(KEY_COMMAND_FMS_FMC_ALERT_LIGHT, 0);
		//}

		//if(mastercard_input(26, &afdsTest2) && afdsTest2)
		//{
		//	//   &iflyCmd = 238
		//	sendMessageInt(KEY_COMMAND_AUTOMATICFLIGHT_DISENGAGE_LIGHT_TEST_2, 0);
		//}

		//if(mastercard_input(22, &afdsTest1) && afdsTest1)
		//{
		//	//    &iflyCmd = 237
		//	sendMessageInt(KEY_COMMAND_AUTOMATICFLIGHT_DISENGAGE_LIGHT_TEST_1, 0);
		//}

		////GPWS
		//if(mastercard_input(49, &gpws) && gpws)
		//{
		//	//    &iflyCmd = 1191
		//	sendMessageInt(KEY_COMMAND_WARNING_GPWS_SYS_TEST, 0);
		//}

		//if(mastercard_input(50, &flapsInhibit))
		//{
		//	if(flapsInhibit == 1)
		//	{
		//		//   &iflyCmd = 1183
		//		sendMessageInt(KEY_COMMAND_WARNING_GPWS_FLAP_INHIBIT_INHIBIT, 0);
		//	}
		//	else
		//	{
		//		//    &iflyCmd = 1184
		//		sendMessageInt(KEY_COMMAND_WARNING_GPWS_FLAP_INHIBIT_NORMAL, 0);
		//	}
		//}

		//if(mastercard_input(51, &terrainInhibit))
		//{
		//	if(terrainInhibit == 1)
		//	{
		//		//   &iflyCmd = 1189
		//		sendMessageInt(KEY_COMMAND_WARNING_GPWS_TERR_INHIBIT_INHIBIT, 0);
		//	}
		//	else
		//	{
		//		//   &iflyCmd = 1190
		//		sendMessageInt(KEY_COMMAND_WARNING_GPWS_TERR_INHIBIT_NORMAL, 0);
		//	}
		//}

		//if(mastercard_input(52, &gearInhibit))
		//{
		//	if(gearInhibit == 1)
		//	{
		//		//   &iflyCmd = 1186
		//		sendMessageInt(KEY_COMMAND_WARNING_GPWS_GEAR_INHIBIT_INHIBIT, 0);
		//	}
		//	else
		//	{
		//		//    &iflyCmd = 1187
		//		sendMessageInt(KEY_COMMAND_WARNING_GPWS_GEAR_INHIBIT_NORMAL, 0);
		//	}
		//}


		//// Rudder Trim
		//if(mastercard_input(54, &rudderLeft) && rudderLeft)
		//{ 
		//	//    &iflyCmd = 600
		//	sendMessageInt(KEY_COMMAND_FLTCTRL_RUDDER_TRIM_DEC, 0);
		//}

		//if(mastercard_input(57, &rudderRight) && rudderRight)
		//{
		//	//    &iflyCmd = 601
		//	sendMessageInt(KEY_COMMAND_FLTCTRL_RUDDER_TRIM_INC, 0);
		//}

		//// Aileron Trim
		//if(mastercard_input(55, &aileronLeft) && aileronLeft)
		//{
		//	//   &iflyCmd = 602
		//	sendMessageInt(KEY_COMMAND_FLTCTRL_AILERON_TRIM_DEC, 0);
		//}

		//if(mastercard_input(56, &aileronRight) && aileronRight)
		//{
		//	//    &iflyCmd = 603
		//	sendMessageInt(KEY_COMMAND_FLTCTRL_AILERON_TRIM_INC, 0);
		//}

		// Encoders
		double value = 1.0;
		//if(mastercard_encoder(31, &value, 1.0, 1.0) > 0)
		//{
		//	LOG() << "value " << value;
		//	if(value > 0)
		//	{
		//		sendMessageInt(KEY_COMMAND_INSTRUMENT_FO_INBD_DU_BRIGHTNESS_DEC, 0);
		//	}
		//	else
		//	{
		//		sendMessageInt(KEY_COMMAND_INSTRUMENT_FO_INBD_DU_BRIGHTNESS_INC, 0);
		//	}
		//}

		//value = 1.0;
		//if(mastercard_encoder(33, &value, 1.0, 1.0) > 0)
		//{
		//	LOG() << "value " << value;
		//	if(value > 0)
		//	{
		//		sendMessageInt(KEY_COMMAND_INSTRUMENT_FO_OUTBD_DU_BRIGHTNESS_DEC, 0);
		//	}
		//	else
		//	{
		//		sendMessageInt(KEY_COMMAND_INSTRUMENT_FO_OUTBD_DU_BRIGHTNESS_INC, 0);
		//	}
		//}

		//value = 1.0;
		//if(mastercard_encoder(36, &value, 1.0, 1.0) > 0)
		//{
		//	LOG() << "value " << value;
		//	if(value > 0)
		//	{
		//		sendMessageInt(KEY_COMMAND_INSTRUMENT_CAPT_INBD_DU_BRIGHTNESS_INC, 0);
		//	}
		//	else
		//	{
		//		sendMessageInt(KEY_COMMAND_INSTRUMENT_CAPT_INBD_DU_BRIGHTNESS_DEC, 0);
		//	}
		//}
		//value = 1.0;
		//if(mastercard_encoder(42, &value, 1.0, 1.0) > 0)
		//{
		//	LOG() << "value " << value;
		//	if(value > 0)
		//	{
		//		sendMessageInt(KEY_COMMAND_INSTRUMENT_CAPT_OUTBD_DU_BRIGHTNESS_DEC, 0);
		//	}
		//	else
		//	{
		//		sendMessageInt(KEY_COMMAND_INSTRUMENT_CAPT_OUTBD_DU_BRIGHTNESS_INC, 0);
		//	}
		//}

		//value = 1.0;
		//if(mastercard_encoder(40, &value, 1.0, 1.0) > 0)
		//{
		//	LOG() << "value " << value;
		//	if(value > 0)
		//	{
		//		sendMessageInt(KEY_COMMAND_INSTRUMENT_LOWER_DU_BRIGHTNESS_DEC, 0);
		//	}
		//	else
		//	{
		//		sendMessageInt(KEY_COMMAND_INSTRUMENT_LOWER_DU_BRIGHTNESS_INC, 0);
		//	}
		//}
		//value = 1.0;
		//if(mastercard_encoder(38, &value, 1.0, 1.0) > 0)
		//{
		//	LOG() << "value " << value;
		//	if(value > 0)
		//	{
		//		sendMessageInt(KEY_COMMAND_INSTRUMENT_UPPER_DU_BRIGHTNESS_INC, 0);
		//	}
		//	else
		//	{
		//		sendMessageInt(KEY_COMMAND_INSTRUMENT_UPPER_DU_BRIGHTNESS_DEC, 0);
		//	}
		//}

		//// Speed Ref  (auto, V1, Vr, WT, Vref ...
		//value = 1.0;
		//if(mastercard_encoder(45, &value, 1.0, 1.0) > 0)
		//{
		//	LOG() << "value " << value;
		//	if(value > 0)
		//	{
		//		sendMessageInt(KEY_COMMAND_INSTRUMENT_LOWER_SPD_REF_INC, 0);
		//	}
		//	else
		//	{
		//		sendMessageInt(KEY_COMMAND_INSTRUMENT_LOWER_SPD_REF_DEC, 0);
		//	}
		//}

		//// N1 Set
		//value = 1.0;
		//if(mastercard_encoder(47, &value, 1.0, 1.0) > 0)
		//{
		//	LOG() << "value " << value;
		//	if(value > 0)
		//	{
		//		sendMessageInt(KEY_COMMAND_ENGAPU_N1_SET_INC, 0);
		//	}
		//	else
		//	{
		//		sendMessageInt(KEY_COMMAND_ENGAPU_N1_SET_DEC, 0);
		//	}
		//}
	}



	void MipIOCard::updateRelays(int state)
	{
		//mastercard_output(49, &state);  // relay #1 Lights left MIP
		//mastercard_output(50, &state);  // relay #2 CP flight power to center console
		//mastercard_output(51, &state);  // relay #3 Lights Right MIP
		//mastercard_output(52, &state);  // relay #4 Throttle servo motors
		//mastercard_output(53, &state);  // relay #5 Throttle Lights
		////mastercard_output(54, &state);  // relay #6

		//mastercard_output(55, &state);  // relay #7 MCP lights
	}


	void MipIOCard::processOutputs()
	{


	}
}