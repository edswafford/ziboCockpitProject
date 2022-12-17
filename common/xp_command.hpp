#pragma once
#include <string>
#include <utility>

#include "shared_types.hpp"
#include "XPLMDataAccess.h"
#include "XPLMUtilities.h"
#include "xp_data_ref.hpp"


namespace zcockpit::common {


	class XpCommand
	{
	public:
		XpCommand(const std::string name): name_(name)
		{
			const XPLMDataRef ref =	XPLMFindCommand(name_.c_str());
			if(ref) {
				xplane_data_ref_ = ref;
				LOG() << "Command " << name << " ref " << ref;
			}
			else
			{
				LOG() << "Cannot find Xplane Command " << std::string(name_);
				throw std::invalid_argument(std::string("Cannot find Xplane Command " + std::string(name_)));
			}
		}

		void command_once() const;
		void command_begin() const;
		void command_end() const;

		int decrement_subscribers();
		void increment_subscribers() { subscriber_count_ += 1; }
		std::string name() { return name_; };
		bool valid_ref() const {return xplane_data_ref_ != nullptr;}
	private:
		void* xplane_data_ref_{ nullptr };


		std::string name_;
		int subscriber_count_{ 1 };
	};

}
