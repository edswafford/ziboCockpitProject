///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.10.1-0-g8feb16b3)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "mainApp.h"

///////////////////////////////////////////////////////////////////////////

frameMain::frameMain( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizerFrameMain;
	bSizerFrameMain = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* bSizerMainFrame;
	bSizerMainFrame = new wxBoxSizer( wxVERTICAL );

	m_panelMain = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizerMainPanel;
	bSizerMainPanel = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizerPanelMain;
	bSizerPanelMain = new wxBoxSizer( wxVERTICAL );

	sbsizerStatusBox = new wxStaticBoxSizer( new wxStaticBox( m_panelMain, wxID_ANY, wxT("STATUS") ), wxVERTICAL );

	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText1011 = new wxStaticText( sbsizerStatusBox->GetStaticBox(), wxID_ANY, wxT("B737-800 ZIBO Status"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1011->Wrap( -1 );
	fgSizer1->Add( m_staticText1011, 0, wxALL, 5 );

	m_staticTextZiboStatus = new wxStaticText( sbsizerStatusBox->GetStaticBox(), wxID_ANY, wxT("Not Connected"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextZiboStatus->Wrap( -1 );
	m_staticTextZiboStatus->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );
	m_staticTextZiboStatus->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BACKGROUND ) );
	m_staticTextZiboStatus->SetBackgroundColour( wxColour( 255, 0, 0 ) );

	fgSizer1->Add( m_staticTextZiboStatus, 0, wxALL, 5 );

	m_staticText10111 = new wxStaticText( sbsizerStatusBox->GetStaticBox(), wxID_ANY, wxT("Throttle Status"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText10111->Wrap( -1 );
	fgSizer1->Add( m_staticText10111, 0, wxALL, 5 );

	m_staticTextThrottleStatus = new wxStaticText( sbsizerStatusBox->GetStaticBox(), wxID_ANY, wxT("Not Connected"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextThrottleStatus->Wrap( -1 );
	m_staticTextThrottleStatus->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );
	m_staticTextThrottleStatus->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BACKGROUND ) );
	m_staticTextThrottleStatus->SetBackgroundColour( wxColour( 255, 0, 0 ) );

	fgSizer1->Add( m_staticTextThrottleStatus, 0, wxALL, 5 );

	m_staticText10112 = new wxStaticText( sbsizerStatusBox->GetStaticBox(), wxID_ANY, wxT("Transponder Status"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText10112->Wrap( -1 );
	fgSizer1->Add( m_staticText10112, 0, wxALL, 5 );

	m_staticTextTransponderStatus = new wxStaticText( sbsizerStatusBox->GetStaticBox(), wxID_ANY, wxT("Not Connected"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextTransponderStatus->Wrap( -1 );
	m_staticTextTransponderStatus->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );
	m_staticTextTransponderStatus->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BACKGROUND ) );
	m_staticTextTransponderStatus->SetBackgroundColour( wxColour( 255, 0, 0 ) );

	fgSizer1->Add( m_staticTextTransponderStatus, 0, wxALL, 5 );

	m_staticText10113 = new wxStaticText( sbsizerStatusBox->GetStaticBox(), wxID_ANY, wxT("InterfaceIT Main Panel Status"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText10113->Wrap( -1 );
	fgSizer1->Add( m_staticText10113, 0, wxALL, 5 );

	m_staticTextInterfaceitMipStatus = new wxStaticText( sbsizerStatusBox->GetStaticBox(), wxID_ANY, wxT("Not Connected"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextInterfaceitMipStatus->Wrap( -1 );
	m_staticTextInterfaceitMipStatus->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );
	m_staticTextInterfaceitMipStatus->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BACKGROUND ) );
	m_staticTextInterfaceitMipStatus->SetBackgroundColour( wxColour( 255, 0, 0 ) );

	fgSizer1->Add( m_staticTextInterfaceitMipStatus, 0, wxALL, 5 );

	m_staticText10114 = new wxStaticText( sbsizerStatusBox->GetStaticBox(), wxID_ANY, wxT("Interfaceit Overhead Status"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText10114->Wrap( -1 );
	fgSizer1->Add( m_staticText10114, 0, wxALL, 5 );

	m_staticTextInterfaceitOverheadStatus = new wxStaticText( sbsizerStatusBox->GetStaticBox(), wxID_ANY, wxT("Not Connected"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextInterfaceitOverheadStatus->Wrap( -1 );
	m_staticTextInterfaceitOverheadStatus->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );
	m_staticTextInterfaceitOverheadStatus->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BACKGROUND ) );
	m_staticTextInterfaceitOverheadStatus->SetBackgroundColour( wxColour( 255, 0, 0 ) );

	fgSizer1->Add( m_staticTextInterfaceitOverheadStatus, 0, wxALL, 5 );

	m_staticText10115 = new wxStaticText( sbsizerStatusBox->GetStaticBox(), wxID_ANY, wxT("Flight Illusions Main Panel Status"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText10115->Wrap( -1 );
	fgSizer1->Add( m_staticText10115, 0, wxALL, 5 );

	m_staticTextFlightIllusionsMipStatus = new wxStaticText( sbsizerStatusBox->GetStaticBox(), wxID_ANY, wxT("Not Connected"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextFlightIllusionsMipStatus->Wrap( -1 );
	m_staticTextFlightIllusionsMipStatus->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );
	m_staticTextFlightIllusionsMipStatus->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BACKGROUND ) );
	m_staticTextFlightIllusionsMipStatus->SetBackgroundColour( wxColour( 255, 0, 0 ) );

	fgSizer1->Add( m_staticTextFlightIllusionsMipStatus, 0, wxALL, 5 );

	m_staticText101151 = new wxStaticText( sbsizerStatusBox->GetStaticBox(), wxID_ANY, wxT("Flight Illusions Overhead Panel Status\t\t"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText101151->Wrap( -1 );
	fgSizer1->Add( m_staticText101151, 0, wxALL, 5 );

	m_staticTextFlightIllusionsOveheadStatus = new wxStaticText( sbsizerStatusBox->GetStaticBox(), wxID_ANY, wxT("Not Connected"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextFlightIllusionsOveheadStatus->Wrap( -1 );
	m_staticTextFlightIllusionsOveheadStatus->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );
	m_staticTextFlightIllusionsOveheadStatus->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BACKGROUND ) );
	m_staticTextFlightIllusionsOveheadStatus->SetBackgroundColour( wxColour( 255, 0, 0 ) );

	fgSizer1->Add( m_staticTextFlightIllusionsOveheadStatus, 0, wxALL, 5 );

	m_staticText10116 = new wxStaticText( sbsizerStatusBox->GetStaticBox(), wxID_ANY, wxT("CDU Keyboards Status"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText10116->Wrap( -1 );
	fgSizer1->Add( m_staticText10116, 0, wxALL, 5 );

	m_staticTextCduStatus = new wxStaticText( sbsizerStatusBox->GetStaticBox(), wxID_ANY, wxT("Not Connected"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextCduStatus->Wrap( -1 );
	m_staticTextCduStatus->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );
	m_staticTextCduStatus->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BACKGROUND ) );
	m_staticTextCduStatus->SetBackgroundColour( wxColour( 255, 0, 0 ) );

	fgSizer1->Add( m_staticTextCduStatus, 0, wxALL, 5 );


	bSizer12->Add( fgSizer1, 1, 0, 5 );


	sbsizerStatusBox->Add( bSizer12, 1, wxBOTTOM|wxEXPAND, 5 );


	bSizerPanelMain->Add( sbsizerStatusBox, 1, wxALL|wxEXPAND, 10 );

	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( m_panelMain, wxID_ANY, wxT("IOCards Devices") ), wxVERTICAL );

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );

	m_panel2 = new wxPanel( sbSizer2->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );

	m_listBoxIocard = new wxListBox( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	m_listBoxIocard->Enable( false );

	bSizer8->Add( m_listBoxIocard, 0, wxALL|wxEXPAND, 5 );

	m_buttonIocardGetAvailable = new wxButton( m_panel2, wxID_ANY, wxT("Get Available IOCard Devices"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_buttonIocardGetAvailable, 0, wxALL, 5 );

	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxHORIZONTAL );

	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText20 = new wxStaticText( m_panel2, wxID_ANY, wxT("MIP"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText20->Wrap( -1 );
	fgSizer3->Add( m_staticText20, 0, wxALL, 5 );

	m_textCtrlIocardMip = new wxTextCtrl( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_textCtrlIocardMip, 0, wxALL, 5 );

	m_staticTextIocardMipStatus = new wxStaticText( m_panel2, wxID_ANY, wxT("Not Connected"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextIocardMipStatus->Wrap( -1 );
	m_staticTextIocardMipStatus->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BACKGROUND ) );
	m_staticTextIocardMipStatus->SetBackgroundColour( wxColour( 255, 0, 0 ) );

	fgSizer3->Add( m_staticTextIocardMipStatus, 0, wxALL, 5 );

	m_staticText21 = new wxStaticText( m_panel2, wxID_ANY, wxT("FWD OVERHEAD"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText21->Wrap( -1 );
	fgSizer3->Add( m_staticText21, 0, wxALL, 5 );

	m_textCtrIocardFwdOverhead = new wxTextCtrl( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_textCtrIocardFwdOverhead, 0, wxALL, 5 );

	m_staticTextIocardFwdOverheadStatus = new wxStaticText( m_panel2, wxID_ANY, wxT("Not Connected"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextIocardFwdOverheadStatus->Wrap( -1 );
	m_staticTextIocardFwdOverheadStatus->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BACKGROUND ) );
	m_staticTextIocardFwdOverheadStatus->SetBackgroundColour( wxColour( 255, 0, 0 ) );

	fgSizer3->Add( m_staticTextIocardFwdOverheadStatus, 0, wxALL, 5 );

	m_staticText22 = new wxStaticText( m_panel2, wxID_ANY, wxT("REAR OVERHEAD"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText22->Wrap( -1 );
	fgSizer3->Add( m_staticText22, 0, wxALL, 5 );

	m_textCtrIocardRearOverhead = new wxTextCtrl( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_textCtrIocardRearOverhead, 0, wxALL, 5 );

	m_staticTextIocardRearOverheadStatus = new wxStaticText( m_panel2, wxID_ANY, wxT("Not Connected"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextIocardRearOverheadStatus->Wrap( -1 );
	m_staticTextIocardRearOverheadStatus->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BACKGROUND ) );
	m_staticTextIocardRearOverheadStatus->SetBackgroundColour( wxColour( 255, 0, 0 ) );

	fgSizer3->Add( m_staticTextIocardRearOverheadStatus, 0, wxALL, 5 );


	bSizer10->Add( fgSizer3, 1, wxEXPAND, 5 );


	bSizer8->Add( bSizer10, 1, wxALL|wxEXPAND, 5 );


	m_panel2->SetSizer( bSizer8 );
	m_panel2->Layout();
	bSizer8->Fit( m_panel2 );
	bSizer6->Add( m_panel2, 1, wxEXPAND | wxALL, 5 );


	sbSizer2->Add( bSizer6, 1, wxBOTTOM|wxEXPAND, 5 );


	bSizerPanelMain->Add( sbSizer2, 1, wxALL|wxEXPAND, 5 );


	bSizerMainPanel->Add( bSizerPanelMain, 1, wxALL|wxEXPAND, 0 );


	m_panelMain->SetSizer( bSizerMainPanel );
	m_panelMain->Layout();
	bSizerMainPanel->Fit( m_panelMain );
	bSizerMainFrame->Add( m_panelMain, 1, wxALL|wxEXPAND, 0 );


	bSizer9->Add( bSizerMainFrame, 1, wxALL, 0 );

	wxBoxSizer* bSizer101;
	bSizer101 = new wxBoxSizer( wxVERTICAL );

	m_panelThrottle = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( m_panelThrottle, wxID_ANY, wxT("THROTTLE") ), wxVERTICAL );

	wxFlexGridSizer* fgSizer31;
	fgSizer31 = new wxFlexGridSizer( 0, 4, 0, 0 );
	fgSizer31->SetFlexibleDirection( wxBOTH );
	fgSizer31->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );


	fgSizer31->Add( 0, 0, 1, wxEXPAND, 5 );

	m_staticText24 = new wxStaticText( sbSizer3->GetStaticBox(), wxID_ANY, wxT("VALUE"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText24->Wrap( -1 );
	m_staticText24->SetFont( wxFont( 12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Arial") ) );

	fgSizer31->Add( m_staticText24, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 5 );

	m_staticText241 = new wxStaticText( sbSizer3->GetStaticBox(), wxID_ANY, wxT("MIN"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText241->Wrap( -1 );
	m_staticText241->SetFont( wxFont( 12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Arial") ) );

	fgSizer31->Add( m_staticText241, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

	m_staticText242 = new wxStaticText( sbSizer3->GetStaticBox(), wxID_ANY, wxT("MAX"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText242->Wrap( -1 );
	m_staticText242->SetFont( wxFont( 12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Arial") ) );

	fgSizer31->Add( m_staticText242, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

	m_staticText243 = new wxStaticText( sbSizer3->GetStaticBox(), wxID_ANY, wxT("ENG 1"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText243->Wrap( -1 );
	m_staticText243->SetFont( wxFont( 12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Arial") ) );

	fgSizer31->Add( m_staticText243, 0, wxALL, 5 );

	m_textCtrlEng1Value = new wxTextCtrl( sbSizer3->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer31->Add( m_textCtrlEng1Value, 0, wxALL, 5 );

	m_textCtrlEng1Min = new wxTextCtrl( sbSizer3->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer31->Add( m_textCtrlEng1Min, 0, wxALL, 5 );

	m_textCtrlEng1Max = new wxTextCtrl( sbSizer3->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer31->Add( m_textCtrlEng1Max, 0, wxALL, 5 );

	m_staticText2431 = new wxStaticText( sbSizer3->GetStaticBox(), wxID_ANY, wxT("ENG 2"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2431->Wrap( -1 );
	m_staticText2431->SetFont( wxFont( 12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Arial") ) );

	fgSizer31->Add( m_staticText2431, 0, wxALL, 5 );

	m_textCtrlEng2Value = new wxTextCtrl( sbSizer3->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer31->Add( m_textCtrlEng2Value, 0, wxALL, 5 );

	m_textCtrlEng2Min = new wxTextCtrl( sbSizer3->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer31->Add( m_textCtrlEng2Min, 0, wxALL, 5 );

	m_textCtrlEng2Max = new wxTextCtrl( sbSizer3->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer31->Add( m_textCtrlEng2Max, 0, wxALL, 5 );

	m_staticText24311 = new wxStaticText( sbSizer3->GetStaticBox(), wxID_ANY, wxT("SPD BRK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText24311->Wrap( -1 );
	m_staticText24311->SetFont( wxFont( 12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Arial") ) );

	fgSizer31->Add( m_staticText24311, 0, wxALL, 5 );

	m_textCtrlSpdBrkValue = new wxTextCtrl( sbSizer3->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer31->Add( m_textCtrlSpdBrkValue, 0, wxALL, 5 );

	m_textCtrlSpdfBrkMin = new wxTextCtrl( sbSizer3->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer31->Add( m_textCtrlSpdfBrkMin, 0, wxALL, 5 );

	m_textCtrlSpdBrkMax = new wxTextCtrl( sbSizer3->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer31->Add( m_textCtrlSpdBrkMax, 0, wxALL, 5 );

	m_staticText243111 = new wxStaticText( sbSizer3->GetStaticBox(), wxID_ANY, wxT("REV 1"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText243111->Wrap( -1 );
	m_staticText243111->SetFont( wxFont( 12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Arial") ) );

	fgSizer31->Add( m_staticText243111, 0, wxALL, 5 );

	m_textCtrlRev1Value = new wxTextCtrl( sbSizer3->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer31->Add( m_textCtrlRev1Value, 0, wxALL, 5 );

	m_textCtrlRev1Min = new wxTextCtrl( sbSizer3->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer31->Add( m_textCtrlRev1Min, 0, wxALL, 5 );

	m_textCtrlRev1Max = new wxTextCtrl( sbSizer3->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer31->Add( m_textCtrlRev1Max, 0, wxALL, 5 );

	m_staticText243112 = new wxStaticText( sbSizer3->GetStaticBox(), wxID_ANY, wxT("REV 2"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText243112->Wrap( -1 );
	m_staticText243112->SetFont( wxFont( 12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Arial") ) );

	fgSizer31->Add( m_staticText243112, 0, wxALL, 5 );

	m_textCtrlRev2Value = new wxTextCtrl( sbSizer3->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer31->Add( m_textCtrlRev2Value, 0, wxALL, 5 );

	m_textCtrlRev2Min = new wxTextCtrl( sbSizer3->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer31->Add( m_textCtrlRev2Min, 0, wxALL, 5 );

	m_textCtrlRev2Max = new wxTextCtrl( sbSizer3->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer31->Add( m_textCtrlRev2Max, 0, wxALL, 5 );


	sbSizer3->Add( fgSizer31, 1, wxALL|wxEXPAND, 5 );

	wxFlexGridSizer* fgSizer5;
	fgSizer5 = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizer5->SetFlexibleDirection( wxBOTH );
	fgSizer5->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( sbSizer3->GetStaticBox(), wxID_ANY, wxT("CALIBRATE") ), wxVERTICAL );

	wxBoxSizer* bSizer131;
	bSizer131 = new wxBoxSizer( wxVERTICAL );

	m_buttonStartCalibrate = new wxButton( sbSizer4->GetStaticBox(), wxID_ANY, wxT("START"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonStartCalibrate->SetBackgroundColour( wxColour( 192, 192, 192 ) );

	bSizer131->Add( m_buttonStartCalibrate, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

	m_buttonCancelCalibration = new wxButton( sbSizer4->GetStaticBox(), wxID_ANY, wxT("CANCEL"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonCancelCalibration->Enable( false );

	bSizer131->Add( m_buttonCancelCalibration, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

	m_buttonSaveCalibration = new wxButton( sbSizer4->GetStaticBox(), wxID_ANY, wxT("SAVE"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonSaveCalibration->Enable( false );

	bSizer131->Add( m_buttonSaveCalibration, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );


	sbSizer4->Add( bSizer131, 1, wxEXPAND, 5 );


	fgSizer5->Add( sbSizer4, 1, wxEXPAND|wxLEFT, 20 );

	wxStaticBoxSizer* sbSizer5;
	sbSizer5 = new wxStaticBoxSizer( new wxStaticBox( sbSizer3->GetStaticBox(), wxID_ANY, wxT("TEST") ), wxVERTICAL );

	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxVERTICAL );

	m_buttonStartTest = new wxButton( sbSizer5->GetStaticBox(), wxID_ANY, wxT("START"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonStartTest->SetBackgroundColour( wxColour( 192, 192, 192 ) );

	bSizer14->Add( m_buttonStartTest, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

	m_buttonStopTest = new wxButton( sbSizer5->GetStaticBox(), wxID_ANY, wxT("STOP"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonStopTest->Enable( false );

	bSizer14->Add( m_buttonStopTest, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );


	sbSizer5->Add( bSizer14, 1, wxEXPAND, 5 );


	fgSizer5->Add( sbSizer5, 1, wxEXPAND|wxLEFT|wxRIGHT, 20 );

	wxStaticBoxSizer* sbSizer6;
	sbSizer6 = new wxStaticBoxSizer( new wxStaticBox( sbSizer3->GetStaticBox(), wxID_ANY, wxT("STEPPER TEST") ), wxVERTICAL );

	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxVERTICAL );

	m_buttonIncrementStepper = new wxButton( sbSizer6->GetStaticBox(), wxID_ANY, wxT("Increment"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer15->Add( m_buttonIncrementStepper, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxLEFT|wxRIGHT, 5 );

	m_buttonDecrementSteppper = new wxButton( sbSizer6->GetStaticBox(), wxID_ANY, wxT("Decrement"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer15->Add( m_buttonDecrementSteppper, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );


	sbSizer6->Add( bSizer15, 1, wxEXPAND, 5 );


	fgSizer5->Add( sbSizer6, 1, wxALIGN_RIGHT|wxEXPAND|wxLEFT, 20 );


	sbSizer3->Add( fgSizer5, 1, wxEXPAND, 5 );


	bSizer11->Add( sbSizer3, 1, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxVERTICAL );


	bSizer13->Add( 0, 0, 1, wxEXPAND, 5 );


	bSizer11->Add( bSizer13, 1, wxEXPAND, 5 );


	m_panelThrottle->SetSizer( bSizer11 );
	m_panelThrottle->Layout();
	bSizer11->Fit( m_panelThrottle );
	bSizer101->Add( m_panelThrottle, 1, wxALL|wxEXPAND|wxLEFT, 8 );


	bSizer9->Add( bSizer101, 1, wxEXPAND, 5 );


	bSizerFrameMain->Add( bSizer9, 1, wxEXPAND, 5 );


	this->SetSizer( bSizerFrameMain );
	this->Layout();
	m_menubarMain = new wxMenuBar( 0 );
	m_menuFile = new wxMenu();
	m_menuFileNew = new wxMenu();
	wxMenuItem* m_menuFileNewItem = new wxMenuItem( m_menuFile, wxID_ANY, wxT("New"), wxEmptyString, wxITEM_NORMAL, m_menuFileNew );
	m_menuFile->Append( m_menuFileNewItem );

	m_menuFileOpen = new wxMenu();
	wxMenuItem* m_menuFileOpenItem = new wxMenuItem( m_menuFile, wxID_ANY, wxT("Open"), wxEmptyString, wxITEM_NORMAL, m_menuFileOpen );
	m_menuFileOpen->AppendSeparator();

	m_menuFile->Append( m_menuFileOpenItem );

	m_menuFileSave = new wxMenu();
	wxMenuItem* m_menuFileSaveItem = new wxMenuItem( m_menuFile, wxID_ANY, wxT("Save"), wxEmptyString, wxITEM_NORMAL, m_menuFileSave );
	m_menuFile->Append( m_menuFileSaveItem );

	m_menuFileExcit = new wxMenu();
	wxMenuItem* m_menuFileExcitItem = new wxMenuItem( m_menuFile, wxID_ANY, wxT("Exit"), wxEmptyString, wxITEM_NORMAL, m_menuFileExcit );
	m_menuFile->Append( m_menuFileExcitItem );

	m_menubarMain->Append( m_menuFile, wxT("File") );

	m_menuEdit = new wxMenu();
	m_menubarMain->Append( m_menuEdit, wxT("Edit") );

	this->SetMenuBar( m_menubarMain );


	this->Centre( wxBOTH );

	// Connect Events
	m_buttonStartCalibrate->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frameMain::onCalibrate ), NULL, this );
	m_buttonCancelCalibration->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frameMain::onCancelCalibration ), NULL, this );
	m_buttonSaveCalibration->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frameMain::onSaveCalibration ), NULL, this );
	m_buttonStartTest->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frameMain::onTest ), NULL, this );
	m_buttonStopTest->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frameMain::onStopTest ), NULL, this );
	m_buttonIncrementStepper->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frameMain::onIncrementStepper ), NULL, this );
	m_buttonDecrementSteppper->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frameMain::onDecrementStepper ), NULL, this );
}

frameMain::~frameMain()
{
	// Disconnect Events
	m_buttonStartCalibrate->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frameMain::onCalibrate ), NULL, this );
	m_buttonCancelCalibration->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frameMain::onCancelCalibration ), NULL, this );
	m_buttonSaveCalibration->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frameMain::onSaveCalibration ), NULL, this );
	m_buttonStartTest->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frameMain::onTest ), NULL, this );
	m_buttonStopTest->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frameMain::onStopTest ), NULL, this );
	m_buttonIncrementStepper->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frameMain::onIncrementStepper ), NULL, this );
	m_buttonDecrementSteppper->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frameMain::onDecrementStepper ), NULL, this );

}
