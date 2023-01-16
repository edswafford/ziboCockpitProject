#ifndef __MainWindow__
#define __MainWindow__

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "zcockpitApp.h"
#include "zcockpitApp.h"
#include "wxfb/mainApp.h"


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
		auto msg = wxArrayString(1, message.c_str());
		std::ostream stream(m_textCtrlIocardMip);
		stream << message;
		stream.flush();
	}

	void set_iocard_fwd_overhead_addr(std::string message)
	{
		auto msg = wxArrayString(1, message.c_str());
		std::ostream stream(m_textCtrIocardFwdOverhead);
		stream << message;
		stream.flush();
	}
	void set_iocard_rear_overhead_addr(std::string message)
	{
		auto msg = wxArrayString(1, message.c_str());
		std::ostream stream(m_textCtrIocardRearOverhead);
		stream << message;
		stream.flush();
	}


private:
	static constexpr wchar_t connected[] = wxT("Connected");
	static constexpr wchar_t not_connected[] = wxT("Not Connected");

};

#endif // __MainWindow__
