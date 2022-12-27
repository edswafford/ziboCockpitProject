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


	bSizerFrameMain->Add( bSizerMainFrame, 1, wxALL, 0 );


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
}

frameMain::~frameMain()
{
}
