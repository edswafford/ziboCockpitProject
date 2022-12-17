#ifndef XPLM200
#define XPLM200 1
#endif

#include "xp_command.hpp"

#include <stdexcept>



namespace zcockpit::common {

	void XpCommand::command_once() const
	{
		if(xplane_data_ref_) {
			LOG() << "XPLMCommandOnce CommandRef " << xplane_data_ref_;
				XPLMCommandOnce(xplane_data_ref_);
		}	
	}

	void XpCommand::command_begin() const
	{
		if(xplane_data_ref_) {
			LOG() << "XPLMCommandBegin CommandRef " << xplane_data_ref_;
				XPLMCommandBegin(xplane_data_ref_);
		}	
	}

	void XpCommand::command_end() const
	{
		if(xplane_data_ref_) {
			LOG() << "XPLMCommandEnd CommandRef " << xplane_data_ref_;
				XPLMCommandEnd(xplane_data_ref_);
		}	
	}

	int XpCommand::decrement_subscribers()
	{

		if (subscriber_count_ <= 0)
		{
			subscriber_count_ = 0;
		}
		if (subscriber_count_ == 1) {
			subscriber_count_ = 0;
		}
		else {
			subscriber_count_ -= 1;
		}
		return subscriber_count_;
	}
}
