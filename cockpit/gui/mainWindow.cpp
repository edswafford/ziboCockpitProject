#include "mainWindow.hpp"

MainWindow::MainWindow(wxWindow* parent): frameMain( parent )
{
	sbsizerStatusBox->GetStaticBox()->SetWindowStyleFlag(wxBORDER_THEME);
	sbsizerStatusBox->GetStaticBox()->Refresh();
}

void MainWindow::onClose(wxCloseEvent& event)
{
	if(callbacks.contains(CallbackTypes::Close)) {
		callbacks[CallbackTypes::Close]();
	}
	event.Skip();
}

void MainWindow::m_menuItemFileExitOnMenuSelection(wxCommandEvent& event)
{
	Close(true);		
}

void MainWindow::add_callback(const CallbackTypes cb_type, const TCallback cb)
{
	callbacks[cb_type] = cb;	
}

void MainWindow::onCalibrate(wxCommandEvent& event)
{
	const wxString text = m_buttonStartCalibrate->GetLabelText();
	if(text.Cmp(start) == 0) {
		m_buttonStartCalibrate->SetLabel(running);
		m_buttonStartCalibrate->SetBackgroundColour( wxColor(*wxGREEN));
		
		m_buttonCancelCalibration->Enable(true);
		m_buttonSaveCalibration->Enable(true);
		m_buttonStartCalibrate->Enable(false);
		m_buttonStartTest->Enable(false);
		if(callbacks.contains(CallbackTypes::Calibrate)) {
			callbacks[CallbackTypes::Calibrate]();
		}
	}
}

void MainWindow::onSaveCalibration(wxCommandEvent& event)
{
	const wxString text = m_buttonStartCalibrate->GetLabelText();
	if(text.Cmp(running) == 0) {
		m_buttonStartCalibrate->SetLabel(start);
		m_buttonStartCalibrate->SetBackgroundColour( wxColor(*wxLIGHT_GREY));

		m_buttonCancelCalibration->Enable(false);
		m_buttonSaveCalibration->Enable(false);
		m_buttonStartCalibrate->Enable(true);
		m_buttonStartTest->Enable(true);
		if(callbacks.contains(CallbackTypes::SaveCalibration)) {
			callbacks[CallbackTypes::Calibrate]();
		}
	}
}

void MainWindow::onCancelCalibration(wxCommandEvent& event)
{
	const wxString text = m_buttonStartCalibrate->GetLabelText();
	if(text.Cmp(running) == 0) {
		m_buttonStartCalibrate->SetLabel(start);
		m_buttonStartCalibrate->SetBackgroundColour( wxColor(*wxLIGHT_GREY));

		m_buttonCancelCalibration->Enable(false);
		m_buttonSaveCalibration->Enable(false);
		m_buttonStartCalibrate->Enable(true);
		m_buttonStartTest->Enable(true);
		if(callbacks.contains(CallbackTypes::CancelCalibration)) {
			callbacks[CallbackTypes::Calibrate]();
		}
	}
}

void MainWindow::onTest(wxCommandEvent& event)
{
	const wxString text = m_buttonStartTest->GetLabelText();
	if(text.Cmp(start) == 0) {
		m_buttonStartTest->SetLabel(running);
		m_buttonStartTest->SetBackgroundColour( wxColor(*wxGREEN));

		m_buttonStopTest->Enable(true);
		m_buttonStartTest->Enable(false);
		m_buttonStartCalibrate->Enable(false);
		if(callbacks.contains(CallbackTypes::ThrottleTest)) {
			callbacks[CallbackTypes::Calibrate]();
		}
	}
}

void MainWindow::onStopTest(wxCommandEvent& event)
{
	const wxString text = m_buttonStartTest->GetLabelText();
	if(text.Cmp(running) == 0) {
		m_buttonStartTest->SetLabel(start);
		m_buttonStartTest->SetBackgroundColour( wxColor(*wxLIGHT_GREY));

		m_buttonStopTest->Enable(false);
		m_buttonStartTest->Enable(true);
		m_buttonStartCalibrate->Enable(true);
		if(callbacks.contains(CallbackTypes::StopThrottleTest)) {
			callbacks[CallbackTypes::Calibrate]();
		}
	}		
}

void MainWindow::set_zibo_status(bool active)
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

void MainWindow::set_throttle_status(bool active)
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

void MainWindow::set_interfaceit_mip_status(bool active)
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

void MainWindow::set_interfaceit_overhead_status(bool active)
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

void MainWindow::set_iocard_status(std::string message)
{
	auto msg = wxArrayString(1, message.c_str());
	m_listBoxIocard->Insert(msg, 0);
}

void MainWindow::set_iocard_mip_status(bool active)
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

void MainWindow::set_iocard_fwd_overhead_status(bool active)
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

void MainWindow::set_iocard_rear_overhead_status(bool active)
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

void MainWindow::set_flight_illusion_mip_status(bool active)
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

void MainWindow::set_flight_illusion_overhead_status(bool active)
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

void MainWindow::set_transponder_status(bool active)
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

void MainWindow::set_iocard_mip_addr(std::string message)
{
	//auto msg = wxArrayString(1, message.c_str());
	std::ostream stream(m_textCtrlIocardMip);
	stream << message;
	stream.flush();
}

void MainWindow::set_iocard_fwd_overhead_addr(std::string message)
{
	//auto msg = wxArrayString(1, message.c_str());
	std::ostream stream(m_textCtrIocardFwdOverhead);
	stream << message;
	stream.flush();
}

void MainWindow::set_iocard_rear_overhead_addr(std::string message)
{
	//auto msg = wxArrayString(1, message.c_str());
	std::ostream stream(m_textCtrIocardRearOverhead);
	stream << message;
	stream.flush();
}

void MainWindow::set_eng1_min(unsigned long min)
{
	auto message = std::to_string(min);
	m_textCtrlEng1Min->Clear();
	std::ostream stream(m_textCtrlEng1Min);
	stream << message;
	stream.flush();
}

void MainWindow::set_eng1_max(unsigned long max)
{
	auto message = std::to_string(max);
	m_textCtrlEng1Max->Clear();
	std::ostream stream(m_textCtrlEng1Max);
	stream << message;
	stream.flush();
}

void MainWindow::set_eng1_value(unsigned long value)
{
	auto message = std::to_string(value);
	m_textCtrlEng1Value->Clear();
	std::ostream stream(m_textCtrlEng1Value);
	stream << message;
	stream.flush();
}

void MainWindow::set_eng2_min(unsigned long min)
{
	auto message = std::to_string(min);
	m_textCtrlEng2Min->Clear();
	std::ostream stream(m_textCtrlEng2Min);
	stream << message;
	stream.flush();
}

void MainWindow::set_eng2_max(unsigned long max)
{
	auto message = std::to_string(max);
	m_textCtrlEng2Max->Clear();
	std::ostream stream(m_textCtrlEng2Max);
	stream << message;
	stream.flush();
}

void MainWindow::set_eng2_value(unsigned long value)
{
	auto message = std::to_string(value);
	m_textCtrlEng2Value->Clear();
	std::ostream stream(m_textCtrlEng2Value);
	stream << message;
	stream.flush();
}

void MainWindow::set_spd_brk_min(unsigned long min)
{
	auto message = std::to_string(min);
	m_textCtrlSpdfBrkMin->Clear();
	std::ostream stream(m_textCtrlSpdfBrkMin);
	stream << message;
	stream.flush();
}

void MainWindow::set_spd_brk_max(unsigned long max)
{
	auto message = std::to_string(max);
	m_textCtrlSpdBrkMax->Clear();
	std::ostream stream(m_textCtrlSpdBrkMax);
	stream << message;
	stream.flush();
}

void MainWindow::set_spd_brk_value(unsigned long value)
{
	auto message = std::to_string(value);
	m_textCtrlSpdBrkValue->Clear();
	std::ostream stream(m_textCtrlSpdBrkValue);
	stream << message;
	stream.flush();
}

void MainWindow::set_rev1_min(unsigned long min)
{
	auto message = std::to_string(min);
	m_textCtrlRev1Min->Clear();
	std::ostream stream(m_textCtrlRev1Min);
	stream << message;
	stream.flush();
}

void MainWindow::set_rev1_max(unsigned long max)
{
	auto message = std::to_string(max);
	m_textCtrlRev1Max->Clear();
	std::ostream stream(m_textCtrlRev1Max);
	stream << message;
	stream.flush();
}

void MainWindow::set_rev1_value(unsigned long value)
{
	auto message = std::to_string(value);
	m_textCtrlRev1Value->Clear();
	std::ostream stream(m_textCtrlRev1Value);
	stream << message;
	stream.flush();
}

void MainWindow::set_rev2_min(unsigned long min)
{
	auto message = std::to_string(min);
	m_textCtrlRev2Min->Clear();
	std::ostream stream(m_textCtrlRev2Min);
	stream << message;
	stream.flush();
}

void MainWindow::set_rev2_max(unsigned long max)
{
	auto message = std::to_string(max);
	m_textCtrlRev2Max->Clear();
	std::ostream stream(m_textCtrlRev2Max);
	stream << message;
	stream.flush();
}

void MainWindow::set_rev2_value(unsigned long value)
{
	auto message = std::to_string(value);
	m_textCtrlRev2Value->Clear();
	std::ostream stream(m_textCtrlRev2Value);
	stream << message;
	stream.flush();
}
