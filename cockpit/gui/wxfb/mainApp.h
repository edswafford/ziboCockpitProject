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
		wxButton* m_buttonIocardMip;
		wxTextCtrl* m_textCtrlIocardMip;
		wxStaticText* m_staticTextIocardMipStatus;
		wxButton* m_buttonIocardFwdOverhead;
		wxTextCtrl* m_textCtrIocardFwdOverhead;
		wxStaticText* m_staticTextIocardFwdOverheadStatus;
		wxButton* m_buttonIocardRearOverhead;
		wxTextCtrl* m_textCtrIocardRearOverhead;
		wxStaticText* m_staticTextIocardRearOverheadStatus;
		wxMenuBar* m_menubarMain;
		wxMenu* m_menuFile;
		wxMenu* m_menuFileNew;
		wxMenu* m_menuFileOpen;
		wxMenu* m_menuFileSave;
		wxMenu* m_menuFileExcit;
		wxMenu* m_menuEdit;

	public:

		frameMain( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Zcockpit "), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 810,582 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

		~frameMain();

};

