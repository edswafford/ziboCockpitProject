#pragma once

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <xlocmon>

#include "wxfb/mainApp.h"
#include "../hardware/throttle/throttle_joystick.hpp"

class wxWindow;

/** Implementing frameMain */
class MainWindow : public frameMain
{
	public:
	
	MainWindow( wxWindow* parent );

	void onCalibrate( wxCommandEvent& event ) override;

	void onSaveCalibration(wxCommandEvent& event) override;

	void onCancelCalibration(wxCommandEvent& event) override;

	void onTest(wxCommandEvent& event) override;

	void onStopTest(wxCommandEvent& event) override;

	void set_zibo_status(bool active);

	void set_throttle_status(bool active);

	void set_interfaceit_mip_status(bool active);

	void set_interfaceit_overhead_status(bool active);

	void set_iocard_status(std::string message);

	void set_iocard_mip_status(bool active);

	void set_iocard_fwd_overhead_status(bool active);

	void set_iocard_rear_overhead_status(bool active);

	void set_flight_illusion_mip_status(bool active);

	void set_flight_illusion_overhead_status(bool active);

	void set_transponder_status(bool active);


	void set_iocard_mip_addr(std::string message);

	void set_iocard_fwd_overhead_addr(std::string message);

	void set_iocard_rear_overhead_addr(std::string message);

	void set_eng1_min(unsigned long min);

	void set_eng1_max(unsigned long max);

	void set_eng1_value(unsigned long value);

	void set_eng2_min(unsigned long min);

	void set_eng2_max(unsigned long max);

	void set_eng2_value(unsigned long value);


	void set_spd_brk_min(unsigned long min);

	void set_spd_brk_max(unsigned long max);

	void set_spd_brk_value(unsigned long value);

	void set_rev1_min(unsigned long min);

	void set_rev1_max(unsigned long max);

	void set_rev1_value(unsigned long value);

	void set_rev2_min(unsigned long min);

	void set_rev2_max(unsigned long max);

	void set_rev2_value(unsigned long value);


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

