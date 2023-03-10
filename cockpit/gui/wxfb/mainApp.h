///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.10.1-0-g8feb16b3)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/listbox.h>
#include <wx/button.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/textctrl.h>
#include <wx/panel.h>
#include <wx/menu.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class frameMain
///////////////////////////////////////////////////////////////////////////////
class frameMain : public wxFrame
{
	private:

	protected:
		wxPanel* m_panelMain;
		wxStaticBoxSizer* sbsizerStatusBox;
		wxStaticText* m_staticText1011;
		wxStaticText* m_staticTextZiboStatus;
		wxStaticText* m_staticText10111;
		wxStaticText* m_staticTextThrottleStatus;
		wxStaticText* m_staticText10112;
		wxStaticText* m_staticTextTransponderStatus;
		wxStaticText* m_staticText10113;
		wxStaticText* m_staticTextInterfaceitMipStatus;
		wxStaticText* m_staticText10114;
		wxStaticText* m_staticTextInterfaceitOverheadStatus;
		wxStaticText* m_staticText10115;
		wxStaticText* m_staticTextFlightIllusionsMipStatus;
		wxStaticText* m_staticText101151;
		wxStaticText* m_staticTextFlightIllusionsOveheadStatus;
		wxStaticText* m_staticText10116;
		wxStaticText* m_staticTextCduStatus;
		wxPanel* m_panel2;
		wxListBox* m_listBoxIocard;
		wxButton* m_buttonIocardGetAvailable;
		wxStaticText* m_staticText20;
		wxTextCtrl* m_textCtrlIocardMip;
		wxStaticText* m_staticTextIocardMipStatus;
		wxStaticText* m_staticText21;
		wxTextCtrl* m_textCtrIocardFwdOverhead;
		wxStaticText* m_staticTextIocardFwdOverheadStatus;
		wxStaticText* m_staticText22;
		wxTextCtrl* m_textCtrIocardRearOverhead;
		wxStaticText* m_staticTextIocardRearOverheadStatus;
		wxPanel* m_panelThrottle;
		wxStaticText* m_staticText24;
		wxStaticText* m_staticText241;
		wxStaticText* m_staticText242;
		wxStaticText* m_staticText243;
		wxTextCtrl* m_textCtrlEng1Value;
		wxTextCtrl* m_textCtrlEng1Min;
		wxTextCtrl* m_textCtrlEng1Max;
		wxStaticText* m_staticText2431;
		wxTextCtrl* m_textCtrlEng2Value;
		wxTextCtrl* m_textCtrlEng2Min;
		wxTextCtrl* m_textCtrlEng2Max;
		wxStaticText* m_staticText24311;
		wxTextCtrl* m_textCtrlSpdBrkValue;
		wxTextCtrl* m_textCtrlSpdfBrkMin;
		wxTextCtrl* m_textCtrlSpdBrkMax;
		wxStaticText* m_staticText243111;
		wxTextCtrl* m_textCtrlRev1Value;
		wxTextCtrl* m_textCtrlRev1Min;
		wxTextCtrl* m_textCtrlRev1Max;
		wxStaticText* m_staticText243112;
		wxTextCtrl* m_textCtrlRev2Value;
		wxTextCtrl* m_textCtrlRev2Min;
		wxTextCtrl* m_textCtrlRev2Max;
		wxButton* m_buttonStartCalibrate;
		wxButton* m_buttonCancelCalibration;
		wxButton* m_buttonSaveCalibration;
		wxButton* m_buttonStartTest;
		wxButton* m_buttonStopTest;
		wxButton* m_buttonIncrementStepper;
		wxButton* m_buttonDecrementSteppper;
		wxMenuBar* m_menubarMain;
		wxMenu* m_menuFile;
		wxMenu* m_menuHelp;

		// Virtual event handlers, override them in your derived class
		virtual void onClose( wxCloseEvent& event ) { event.Skip(); }
		virtual void onCalibrate( wxCommandEvent& event ) { event.Skip(); }
		virtual void onCancelCalibration( wxCommandEvent& event ) { event.Skip(); }
		virtual void onSaveCalibration( wxCommandEvent& event ) { event.Skip(); }
		virtual void onTest( wxCommandEvent& event ) { event.Skip(); }
		virtual void onStopTest( wxCommandEvent& event ) { event.Skip(); }
		virtual void onIncrementStepper( wxCommandEvent& event ) { event.Skip(); }
		virtual void onDecrementStepper( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_menuItemFileExitOnMenuSelection( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_menuItemHelpAboutOnMenuSelection( wxCommandEvent& event ) { event.Skip(); }


	public:

		frameMain( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Zcockpit "), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 880,585 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

		~frameMain();

};

