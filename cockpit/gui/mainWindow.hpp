#pragma once

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <xlocmon>

#include "zcockpitApp.hpp"
#include "wxfb/mainApp.h"
#include "../hardware/throttle/throttle_joystick.hpp"

class wxWindow;

/** Implementing frameMain */
class MainWindow : public frameMain
{
	public:
	
	MainWindow( wxWindow* parent ): frameMain( parent )
	{
		sbsizerStatusBox->GetStaticBox()->SetWindowStyleFlag(wxBORDER_THEME);
		sbsizerStatusBox->GetStaticBox()->Refresh();
	}

	void onCalibrate( wxCommandEvent& event ) override
	{
		const wxString text = m_buttonStartCalibrate->GetLabelText();
		if(text.Cmp(start) == 0) {
			m_buttonStartCalibrate->SetLabel(running);
			m_buttonStartCalibrate->SetBackgroundColour( wxColor(*wxGREEN));
		
			m_buttonCancelCalibration->Enable(true);
			m_buttonSaveCalibration->Enable(true);
			m_buttonStartCalibrate->Enable(false);
			m_buttonStartTest->Enable(false);
			zcockpit::cockpit::hardware::ThrottleAndJoystick::set_enable_realtime_display(true);
			zcockpit::cockpit::hardware::ThrottleAndJoystick::set_enable_calibration(true);
			zcockpit::cockpit::hardware::ThrottleAndJoystick::set_cancel_calibration(false);
		}
	}

	void onSaveCalibration(wxCommandEvent& event) override
	{
		const wxString text = m_buttonStartCalibrate->GetLabelText();
		if(text.Cmp(running) == 0) {
			m_buttonStartCalibrate->SetLabel(start);
			m_buttonStartCalibrate->SetBackgroundColour( wxColor(*wxLIGHT_GREY));

			m_buttonCancelCalibration->Enable(false);
			m_buttonSaveCalibration->Enable(false);
			m_buttonStartCalibrate->Enable(true);
			m_buttonStartTest->Enable(true);
			zcockpit::cockpit::hardware::ThrottleAndJoystick::set_save_calibration(true);		
			zcockpit::cockpit::hardware::ThrottleAndJoystick::set_enable_realtime_display(false);
			zcockpit::cockpit::hardware::ThrottleAndJoystick::set_enable_calibration(false);
		}
	}

	void onCancelCalibration(wxCommandEvent& event) override
	{
		const wxString text = m_buttonStartCalibrate->GetLabelText();
		if(text.Cmp(running) == 0) {
			m_buttonStartCalibrate->SetLabel(start);
			m_buttonStartCalibrate->SetBackgroundColour( wxColor(*wxLIGHT_GREY));

			m_buttonCancelCalibration->Enable(false);
			m_buttonSaveCalibration->Enable(false);
			m_buttonStartCalibrate->Enable(true);
			m_buttonStartTest->Enable(true);
			zcockpit::cockpit::hardware::ThrottleAndJoystick::set_enable_realtime_display(false);
			zcockpit::cockpit::hardware::ThrottleAndJoystick::set_enable_calibration(false);
			zcockpit::cockpit::hardware::ThrottleAndJoystick::set_cancel_calibration(true);
		}
	}

	void onTest(wxCommandEvent& event) override
	{
		const wxString text = m_buttonStartTest->GetLabelText();
		if(text.Cmp(start) == 0) {
			m_buttonStartTest->SetLabel(running);
			m_buttonStartTest->SetBackgroundColour( wxColor(*wxGREEN));

			m_buttonStopTest->Enable(true);
			m_buttonStartTest->Enable(false);
			m_buttonStartCalibrate->Enable(false);
			zcockpit::cockpit::hardware::ThrottleAndJoystick::set_enable_realtime_display(true);
		}
	}
	void onStopTest(wxCommandEvent& event) override
	{
		const wxString text = m_buttonStartTest->GetLabelText();
		if(text.Cmp(running) == 0) {
			m_buttonStartTest->SetLabel(start);
			m_buttonStartTest->SetBackgroundColour( wxColor(*wxLIGHT_GREY));

			m_buttonStopTest->Enable(false);
			m_buttonStartTest->Enable(true);
			m_buttonStartCalibrate->Enable(true);
			zcockpit::cockpit::hardware::ThrottleAndJoystick::set_enable_realtime_display(false);
		}		
	}

	void set_zibo_status(bool active)
	{
		const wxString text = m_staticTextZiboStatus->GetLabelText();

		if(active){
			if(text.CmpNoCase(connected) != 0) {
				m_staticTextZiboStatus->SetBackgroundColour( wxColor(*wxGREEN));
				m_staticTextZiboStatus->SetLabel (connected);
			}
		}
		else {
			if(text.CmpNoCase(not_connected) != 0) {
				m_staticTextZiboStatus->SetBackgroundColour( wxColor(*wxRED));
				m_staticTextZiboStatus->SetLabel (not_connected);
			}
		}
	}

	void set_throttle_status(bool active)
	{
		const wxString text = m_staticTextThrottleStatus->GetLabelText();

		if(active){
			if(text.CmpNoCase(connected) != 0) {
				m_staticTextThrottleStatus->SetBackgroundColour( wxColor(*wxGREEN));
				m_staticTextThrottleStatus->SetLabel (connected);
			}
		}
		else {
			if(text.CmpNoCase(not_connected) != 0) {
				m_staticTextThrottleStatus->SetBackgroundColour( wxColor(*wxRED));
				m_staticTextThrottleStatus->SetLabel (not_connected);
			}
		}
	}

	void set_interfaceit_mip_status(bool active)
	{
		const wxString text = m_staticTextInterfaceitMipStatus->GetLabelText();
		if(active){
			if(text.CmpNoCase(connected) != 0) {
				m_staticTextInterfaceitMipStatus->SetBackgroundColour( wxColor(*wxGREEN));
				m_staticTextInterfaceitMipStatus->SetLabel (connected);
			}
		}
		else {
			if(text.CmpNoCase(not_connected) != 0) {
				m_staticTextInterfaceitMipStatus->SetBackgroundColour( wxColor(*wxRED));
				m_staticTextInterfaceitMipStatus->SetLabel (not_connected);
			}
		}
	}

	void set_interfaceit_overhead_status(bool active)
	{
		const wxString text = m_staticTextInterfaceitOverheadStatus->GetLabelText();
		if(active){
			if(text.CmpNoCase(connected) != 0) {
				m_staticTextInterfaceitOverheadStatus->SetBackgroundColour( wxColor(*wxGREEN));
				m_staticTextInterfaceitOverheadStatus->SetLabel (connected);
			}
		}
		else {
			if(text.CmpNoCase(not_connected) != 0) {
				m_staticTextInterfaceitOverheadStatus->SetBackgroundColour( wxColor(*wxRED));
				m_staticTextInterfaceitOverheadStatus->SetLabel (not_connected);
			}
		}
	}

	void set_iocard_status(std::string message)
	{
		auto msg = wxArrayString(1, message.c_str());
		m_listBoxIocard->Insert(msg, 0);
	}

	void set_iocard_mip_status(bool active)
	{
		const wxString text = m_staticTextIocardMipStatus->GetLabelText();
		if(active){
			if(text.CmpNoCase(connected) != 0) {
				m_staticTextIocardMipStatus->SetBackgroundColour( wxColor(*wxGREEN));
				m_staticTextIocardMipStatus->SetLabel (connected);
			}
		}
		else {
			if(text.CmpNoCase(not_connected) != 0) {
				m_staticTextIocardMipStatus->SetBackgroundColour( wxColor(*wxRED));
				m_staticTextIocardMipStatus->SetLabel (not_connected);
			}
		}
	}
	void set_iocard_fwd_overhead_status(bool active)
	{
		const wxString text = m_staticTextIocardFwdOverheadStatus->GetLabelText();
		if(active){
			if(text.CmpNoCase(connected) != 0) {
				m_staticTextIocardFwdOverheadStatus->SetBackgroundColour( wxColor(*wxGREEN));
				m_staticTextIocardFwdOverheadStatus->SetLabel (connected);
			}
		}
		else {
			if(text.CmpNoCase(not_connected) != 0) {
				m_staticTextIocardFwdOverheadStatus->SetBackgroundColour( wxColor(*wxRED));
				m_staticTextIocardFwdOverheadStatus->SetLabel (not_connected);
			}
		}
	}
	void set_iocard_rear_overhead_status(bool active)
	{
		const wxString text = m_staticTextIocardRearOverheadStatus->GetLabelText();
		if(active){
			if(text.CmpNoCase(connected) != 0) {
				m_staticTextIocardRearOverheadStatus->SetBackgroundColour( wxColor(*wxGREEN));
				m_staticTextIocardRearOverheadStatus->SetLabel (connected);
			}
		}
		else {
			if(text.CmpNoCase(not_connected) != 0) {
				m_staticTextIocardRearOverheadStatus->SetBackgroundColour( wxColor(*wxRED));
				m_staticTextIocardRearOverheadStatus->SetLabel (not_connected);
			}
		}
	}

	void set_flight_illusion_mip_status(bool active)
	{
		const wxString text = m_staticTextFlightIllusionsMipStatus->GetLabelText();
		if(active){
			if(text.CmpNoCase(connected) != 0) {
				m_staticTextFlightIllusionsMipStatus->SetBackgroundColour( wxColor(*wxGREEN));
				m_staticTextFlightIllusionsMipStatus->SetLabel (connected);
			}
		}
		else {
			if(text.CmpNoCase(not_connected) != 0) {
				m_staticTextFlightIllusionsMipStatus->SetBackgroundColour( wxColor(*wxRED));
				m_staticTextFlightIllusionsMipStatus->SetLabel (not_connected);
			}
		}
	}
	void set_flight_illusion_overhead_status(bool active)
	{
		const wxString text = m_staticTextFlightIllusionsOveheadStatus->GetLabelText();
		if(active){
			if(text.CmpNoCase(connected) != 0) {
				m_staticTextFlightIllusionsOveheadStatus->SetBackgroundColour( wxColor(*wxGREEN));
				m_staticTextFlightIllusionsOveheadStatus->SetLabel (connected);
			}
		}
		else {
			if(text.CmpNoCase(not_connected) != 0) {
				m_staticTextFlightIllusionsOveheadStatus->SetBackgroundColour( wxColor(*wxRED));
				m_staticTextFlightIllusionsOveheadStatus->SetLabel (not_connected);
			}
		}
	}
	void set_transponder_status(bool active)
	{
		const wxString text = m_staticTextTransponderStatus->GetLabelText();
		if(active){
			if(text.CmpNoCase(connected) != 0) {
				m_staticTextTransponderStatus->SetBackgroundColour( wxColor(*wxGREEN));
				m_staticTextTransponderStatus->SetLabel (connected);
			}
		}
		else {
			if(text.CmpNoCase(not_connected) != 0) {
				m_staticTextTransponderStatus->SetBackgroundColour( wxColor(*wxRED));
				m_staticTextTransponderStatus->SetLabel (not_connected);
			}
		}
	}


	
	void set_iocard_mip_addr(std::string message)
	{
		//auto msg = wxArrayString(1, message.c_str());
		std::ostream stream(m_textCtrlIocardMip);
		stream << message;
		stream.flush();
	}

	void set_iocard_fwd_overhead_addr(std::string message)
	{
		//auto msg = wxArrayString(1, message.c_str());
		std::ostream stream(m_textCtrIocardFwdOverhead);
		stream << message;
		stream.flush();
	}
	void set_iocard_rear_overhead_addr(std::string message)
	{
		//auto msg = wxArrayString(1, message.c_str());
		std::ostream stream(m_textCtrIocardRearOverhead);
		stream << message;
		stream.flush();
	}

	void set_eng1_min(unsigned long min)
	{
		auto message = std::to_string(min);
		std::ostream stream(m_textCtrlEng1Min);
		stream << message;
		stream.flush();
	}
	void set_eng1_max(unsigned long max)
	{
		auto message = std::to_string(max);
		std::ostream stream(m_textCtrlEng1Max);
		stream << message;
		stream.flush();
	}
	void set_eng1_value(unsigned long value)
	{
		auto message = std::to_string(value);
		std::ostream stream(m_textCtrlEng1Value);
		stream << message;
		stream.flush();
	}
	void set_eng2_min(unsigned long min)
	{
		auto message = std::to_string(min);
		std::ostream stream(m_textCtrlEng2Min);
		stream << message;
		stream.flush();
	}
	void set_eng2_max(unsigned long max)
	{
		auto message = std::to_string(max);
		std::ostream stream(m_textCtrlEng2Max);
		stream << message;
		stream.flush();
	}
	void set_eng2_value(unsigned long value)
	{
		auto message = std::to_string(value);
		std::ostream stream(m_textCtrlEng2Value);
		stream << message;
		stream.flush();
	}


	void set_spd_brk_min(unsigned long min)
	{
		auto message = std::to_string(min);
		std::ostream stream(m_textCtrlSpdfBrkMin);
		stream << message;
		stream.flush();
	}
	void set_spd_brk_max(unsigned long max)
	{
		auto message = std::to_string(max);
		std::ostream stream(m_textCtrlSpdBrkMax);
		stream << message;
		stream.flush();
	}
	void set_spd_brk_value(unsigned long value)
	{
		auto message = std::to_string(value);
		std::ostream stream(m_textCtrlSpdBrkValue);
		stream << message;
		stream.flush();
	}

	void set_rev1_min(unsigned long min)
	{
		auto message = std::to_string(min);
		std::ostream stream(m_textCtrlRev1Min);
		stream << message;
		stream.flush();
	}
	void set_rev1_max(unsigned long max)
	{
		auto message = std::to_string(max);
		std::ostream stream(m_textCtrlRev1Max);
		stream << message;
		stream.flush();
	}
	void set_rev1_value(unsigned long value)
	{
		auto message = std::to_string(value);
		std::ostream stream(m_textCtrlRev1Value);
		stream << message;
		stream.flush();
	}
	void set_rev2_min(unsigned long min)
	{
		auto message = std::to_string(min);
		std::ostream stream(m_textCtrlRev2Min);
		stream << message;
		stream.flush();
	}
	void set_rev2_max(unsigned long max)
	{
		auto message = std::to_string(max);
		std::ostream stream(m_textCtrlRev2Max);
		stream << message;
		stream.flush();
	}
	void set_rev2_value(unsigned long value)
	{
		auto message = std::to_string(std::money_base::value);
		std::ostream stream(m_textCtrlRev2Value);
		stream << message;
		stream.flush();
	}



private:
	static constexpr wchar_t connected[] = wxT("Connected");
	static constexpr wchar_t not_connected[] = wxT("Not Connected");

	static constexpr wchar_t start[] = wxT("START");
	static constexpr wchar_t running[] = wxT("RUNNING");

//	static constexpr wchar_t [] = wxT("");
//	static constexpr wchar_t [] = wxT("");
//	static constexpr wchar_t [] = wxT("");
//	static constexpr wchar_t [] = wxT("");
//	static constexpr wchar_t [] = wxT("");

};