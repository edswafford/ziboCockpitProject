#include "almost_equal.hpp"
#include "xp_data_ref.hpp"
#include <iterator>
#include <stdexcept>
#include <cassert>

#include "packet.hpp"
#include "XPLMDataAccess.h"

extern logger LOG;

namespace zcockpit::common {

	//
		// IntXPDataRef
		//
		void IntXPDataRef::update_new_value_from_xplane()
		{
			new_value_ = XPLMGetDatai(xplane_data_ref_);
			// LOG() << "New Value INT  id: " << xplane_data_ref_ << " new value " << new_value_;
		}
		std::optional<common::var_t> IntXPDataRef::get_changed_value()
		{
			if (value_ != new_value_)
			{
				value_ = new_value_;
				return value_;
			}
			return{};
		}

		//
		// FloatXPDataRef
		//
		void FloatXPDataRef::update_new_value_from_xplane()
		{
			new_value_ = XPLMGetDataf(xplane_data_ref_);
			//if(new_value_ != value_) {
			//	LOG() << "id " << xplane_data_ref_ << "new value " << new_value_  << " is bool " << is_bool_annum;
			//}
			if(is_bool_annum) {
				new_value_ = new_value_ == 0.0? 0.0 : 1.0;
			}
		}
		std::optional<common::var_t> FloatXPDataRef::get_changed_value()
		{
			
			if (!common::almost_equal<float>(value_, new_value_))
			{
				LOG() << "id " << xplane_data_ref_ << " new value " << new_value_ << " : " << value_ ;

				value_ = new_value_;
				return value_;
			}
			return{};
		}
		void FloatXPDataRef::set_new_xplane_value(common::var_t val)
		{
			if(std::holds_alternative<float>(val)) {
				auto float_val = std::get<float>(val);
				XPLMSetDataf(xplane_data_ref_, float_val);
				new_value_ = XPLMGetDataf(xplane_data_ref_);
				//LOG() << "Float Set value " << float_val << " xplane value " << new_value_;
			}
			else {
				LOG() << "ERROR: New DataRef value is NOT type float id " << xplane_data_ref_;
			}
		}


		//
		// DoubleXPDataRef
		//
		void DoubleXPDataRef::update_new_value_from_xplane()
		{
			new_value_ = XPLMGetDatad(xplane_data_ref_);
		}
		std::optional<common::var_t> DoubleXPDataRef::get_changed_value()
		{
			if (!common::almost_equal<double>(value_, new_value_))
			{
				value_ = new_value_;
				return value_;
			}
			return{};
		}

		//
		// Vector DataRef
		//
		void VectorIntXPDataRef::update_new_value_from_xplane()
		{
			if (size_ == 0)
			{
				size_ = XPLMGetDatavi(xplane_data_ref_, nullptr, 0, 0);
				if (size_ > 0) {
					new_value_ = std::vector<int>(size_);
					value_ = std::vector<int>(size_);
				}
			}
			if (size_ > 0) {
				const auto values_copied = XPLMGetDatavi(xplane_data_ref_, &new_value_[0], 0, size_);
				assert(size_ == values_copied);
			}
		}
		std::optional<common::var_t> VectorIntXPDataRef::get_changed_value()
		{
			auto values_changed = false;
			for (auto i = 0; i < size_; i++)
			{
				if (value_[i] != new_value_[i])
				{
					values_changed = true;
					break;
				}
			}
			if (values_changed)
			{
				auto changed_values = std::vector<int>();

				std::ranges::copy(new_value_, value_.begin());
				std::ranges::copy(value_, std::back_inserter(changed_values));
				common::var_t variant = changed_values;
				return variant;
			}
			return{};
		}

		//
		// VectorFloatXPDataRef
		//
		void VectorFloatXPDataRef::update_new_value_from_xplane()
		{
			if (size_ == 0)
			{
				size_ = XPLMGetDatavf(xplane_data_ref_, nullptr, 0, 0);
				if (size_ > 0) {
					new_value_ = std::vector<float>(size_);
					value_ = std::vector<float>(size_);
				}
			}
			if (size_ > 0) {
				const auto values_copied = XPLMGetDatavf(xplane_data_ref_, &new_value_[0], 0, size_);
				assert(size_ == values_copied);
				if (is_bool_annum) {
					for (auto i = 0; i < size_; i++) {
						if(is_bool_annum){
							new_value_[i] = new_value_[i] == 0.0 ? 0.0 : 1.0;
						}
					}
				}
			}
		}

		void VectorFloatXPDataRef::set_new_xplane_value(common::var_t val)
		{
			if (size_ == 0)
			{
				size_ = XPLMGetDatavf(xplane_data_ref_, nullptr, 0, 0);
				if (size_ > 0) {
					new_value_ = std::vector<float>(size_);
					value_ = std::vector<float>(size_);
				}
			}
			if(std::holds_alternative<std::vector<float>>(val)) {
				std::vector<float> float_vals = std::get<std::vector<float>>(val);

				auto count = float_vals.size();
				count = count >= size_? size_ : count;
				XPLMSetDatavf(xplane_data_ref_, float_vals.data(), 0, count);

				const auto values_copied = XPLMGetDatavf(xplane_data_ref_, &new_value_[0], 0, size_);
				assert(size_ == values_copied);
				if (is_bool_annum) {
					for (auto i = 0; i < size_; i++) {
						if(is_bool_annum){
							new_value_[i] = new_value_[i] == 0.0 ? 0.0 : 1.0;
						}
					}
				}
			}
			else {
				LOG() << "ERROR: New DataRef value is NOT type std::vector<float> id " << xplane_data_ref_;
			}			
		}

		std::optional<common::var_t> VectorFloatXPDataRef::get_changed_value()
		{
			auto values_changed = false;
			for (auto i = 0; i < size_; i++)
			{
				if (!common::almost_equal<float>(value_[i], new_value_[i]))
				{
					values_changed = true;
					break;
				}
			}
			if (values_changed)
			{
				auto changed_values = std::vector<float>();

				std::ranges::copy(new_value_, value_.begin());
				std::ranges::copy(value_, std::back_inserter(changed_values));
				common::var_t variant = changed_values;
				return variant;
			}
			return{};
		}

		//
		// VectorCharXPDataRef
		//
		void VectorCharXPDataRef::update_new_value_from_xplane()
		{
			// first read allocate vectors 
			if (size_ == 0)
			{
				size_ = XPLMGetDatab(xplane_data_ref_, nullptr, 0, 0);
				if (size_ > 0) {
					new_value_ = std::vector<char>(size_);
					value_ = std::vector<char>(size_);
				}
			}
			if (size_ > 0) {
				const auto current_size = XPLMGetDatab(xplane_data_ref_, nullptr, 0, 0);
				if(size_ != current_size) {
					new_value_ = std::vector<char>(size_);
					value_ = std::vector<char>(size_);
				}
				const auto values_copied = XPLMGetDatab(xplane_data_ref_, &new_value_[0], 0, size_);
				assert(size_ == values_copied);
			}
		}
		std::optional<common::var_t> VectorCharXPDataRef::get_changed_value()
		{
			auto values_changed = false;
			for (auto i = 0; i < size_; i++)
			{
				if (value_[i] != new_value_[i])
				{
					values_changed = true;
					break;
				}
			}
			if (values_changed)
			{
				auto changed_values = std::vector<char>();
				std::ranges::copy(new_value_, value_.begin());
				std::ranges::copy(value_, std::back_inserter(changed_values));
				common::var_t variant = changed_values;
				return variant;
			}
			return{};
		}

		// ***********************************************************************************************
		//
		//                                   D A T A R E F S
		//
		// ***********************************************************************************************

		XPDataRef::XPDataRef(std::string name, common::XplaneType requested_type, bool is_rounded) : name_(std::move(name)), type_(requested_type), is_rounded_(is_rounded)
		{
			create_data_ref();
		}

		std::unordered_map<int, common::var_t> XPDataRef::changed_values(std::unordered_map<int, std::unique_ptr<XPDataRef>>& data_refs)
		{
			auto changed_values = std::unordered_map<int, common::var_t>();
			for (auto& [id, data_ref] : data_refs)
			{
				auto call_fetch_xplane_value = [](auto& value) {return value.get_changed_value(); };
				std::optional<common::var_t> changed_value = std::visit(call_fetch_xplane_value, data_ref->xp_dataref_);
				if (changed_value)
				{
					changed_values[id] = *changed_value;
					try{
						if (std::holds_alternative<float>(*changed_value)) {
							float value = std::get<float>(*changed_value);
							LOG() << "dataref idx " << data_ref->name_ << " new value " << value;
						}
						else if (std::holds_alternative<std::vector<float>>(*changed_value)) {
							for (auto& value : std::get<std::vector<float>>(*changed_value)) {
								LOG() << "dataref idx " << data_ref->name_ << " new value " << value;
							}
						}
					}
				    catch (const std::bad_variant_access& ex) {
				    	LOG() << "Exception bad variant access dataref idx " << id << " message " <<  ex.what() << '\n';
				    }
				}
			}
			return changed_values;
		}

		std::unordered_map<int, common::var_t> XPDataRef::values_for_requested_ids(const std::vector<int>& requested_id, std::unordered_map<int, std::unique_ptr<XPDataRef>>& data_refs)
		{
			auto values = std::unordered_map<int, common::var_t>();
			for (auto id : requested_id)
			{
				if (data_refs.contains(id)) {
					const auto& data_ref = data_refs.at(id);

					// update value with new_value so it doesn't get sent twice
					auto call_fetch_xplane_value = [](auto& value) {return value.get_changed_value(); };
					std::optional<common::var_t> changed_value = std::visit(call_fetch_xplane_value, data_ref->xp_dataref_);

					// the new value may be the same as the old value so it wouldn't show up as a chaged value
					// so the other choice is to just get the new value
					auto call_fetch_xplane_new_value = [](auto& value) {return value.get_new_value(); };
					common::var_t value = std::visit(call_fetch_xplane_new_value, data_ref->xp_dataref_);
					values[id] = value;
				}
			}
			return values;
		}

		std::unordered_map<int, var_t> XPDataRef::get_new_values(std::unordered_map<int, std::unique_ptr<XPDataRef>>& data_refs)
		{
			auto values = std::unordered_map<int, common::var_t>();
			for (auto& [id, data_ref] : data_refs)
			{

				auto call_fetch_xplane_value = [](auto& value) {return value.get_new_value(); };
				const common::var_t value = std::visit(call_fetch_xplane_value, data_ref->xp_dataref_);
				values[id] = value;
			}
			return values;
		}
		std::unordered_map<int, var_t> XPDataRef::get_new_values_for_requested(const std::vector<int>& requested_id, std::unordered_map<int, std::unique_ptr<XPDataRef>>& data_refs)
		{
			auto changed_values = std::unordered_map<int, common::var_t>();
			for (auto id : requested_id)
			{
				if (data_refs.contains(id)) {
					const auto& data_ref = data_refs.at(id);
					auto call_fetch_xplane_value = [](auto& value) {return value.get_new_value(); };
					const common::var_t value = std::visit(call_fetch_xplane_value, data_ref->xp_dataref_);
					changed_values[id] = value;
				}
			}
			return changed_values;
		}

		void XPDataRef::update_values_for_requested_ids(const std::vector<int>& requested_id, std::unordered_map<int, std::unique_ptr<XPDataRef>>& data_refs)
		{
			// get new values for requested id's
			for (auto id : requested_id)
			{
				if (data_refs.contains(id)) {
					const auto& data_ref = data_refs.at(id);
					auto call_fetch_xplane_value = [](auto& value) {return value.update_new_value_from_xplane(); };
					std::visit(call_fetch_xplane_value, data_ref->xp_dataref_);
				}
			}
		}

		void XPDataRef::set_values_for_datarefs(const std::vector<std::tuple<int, var_t>>& values, std::unordered_map<int, std::unique_ptr<XPDataRef>>& data_refs)
		{
			// get new values for requested id's
			for (auto& param : values)
			{
				auto id = std::get<0>(param);
				var_t new_val = std::get<1>(param);
				if (data_refs.contains(id)) {
					const auto& data_ref = data_refs.at(id);
					LOG() << "Setting dataref id " << id;
					try{
						//if (std::holds_alternative<float>(std::get<1>(param))) {
						//	auto float_val = std::get<float>(std::get<1>(param));
						//	LOG() << "XPDataREf: Setting DataRef id " << std::get<0>(param) << " new float value " << float_val;
						//}
						//else if (std::holds_alternative<int>(std::get<1>(param))) {
						//	auto int_val = std::get<int>(std::get<1>(param));
						//	LOG() << "XPDataREf: Setting DataRef id " << std::get<0>(param) << " new int value " << int_val;
						//}
						//else if (std::holds_alternative<double>(std::get<1>(param))) {
						//	auto double_val = std::get<double>(std::get<1>(param));
						//	LOG() << "XPDataREf: Setting DataRef id " << std::get<0>(param) << " new int value " << double_val;
						//}
						//else {
						//	LOG() << "XPDataREf: Setting DataRef id " << std::get<0>(param) << " VECTOR TYPE ";
						//}
						//if(std::holds_alternative<FloatXPDataRef>(data_ref->xp_dataref_)) {
						//	LOG() << "DataRef is type FloatXPDataRef";
						//}
						//else if(std::holds_alternative<IntXPDataRef>(data_ref->xp_dataref_)) {
						//	LOG() << "DataRef is type IntXPDataRef";
						//}
						auto call_set_xplane_value = [new_val](auto& value) {value.set_new_xplane_value(new_val); };
						std::visit(call_set_xplane_value, data_ref->xp_dataref_);
					}
					catch(...) {
						LOG() << "Failed std::visit";
					}
				}
				else {
					LOG() << "Cannot set dataRef " << id << " is invalid";
				}
			}
		}


		void XPDataRef::update_new_values(std::unordered_map<int, std::unique_ptr<XPDataRef>>& data_refs) {
			// get the latest variant from Xplane so they can be process outside of Main thread
			for (auto& [id, data_ref] : data_refs)
			{
				auto call_fetch_xplane_value = [](auto& value) {return value.update_new_value_from_xplane(); };
				std::visit(call_fetch_xplane_value, data_ref->xp_dataref_);
			}
		}

		int XPDataRef::decrement_subscribers()
		{
			
			if (subscriber_count_ <= 0)
			{
				subscriber_count_ = 0;
			}
			if(subscriber_count_ == 1) {
				subscriber_count_ = 0;
			}
			else {
				subscriber_count_ -= 1;
			}
			return subscriber_count_;
		}



		void  XPDataRef::create_data_ref()
		{
			const XPLMDataRef ref = XPLMFindDataRef(name_.c_str());
			if (ref) {
				auto refType = XPLMGetDataRefTypes(ref);
				switch (type_)
				{
					case common::XplaneType::type_Unknown:
					if (refType & xplmType_Float) {
						xp_dataref_ = FloatXPDataRef(ref, refType, is_rounded_);
						type_ = XplaneType::type_Float;
						LOG() << "Created " << name_ << " Unknown FloatXPDataRef ref " << ref << " is annun " << is_rounded_;
					}
					else if (refType & xplmType_Int) {
						xp_dataref_ = IntXPDataRef(ref, refType, is_rounded_);
						type_ = XplaneType::type_Int;
						LOG() << "Created " << name_ << " Unknown IntXPDataRef ref " << ref << " is annun " << is_rounded_;
					}
					else if (refType & xplmType_Double) {
						xp_dataref_ = DoubleXPDataRef(ref, refType, is_rounded_);
						type_ = XplaneType::type_Double;
						LOG() << "Created " << name_ << " Unknown DoubleXPDataRef ref " << ref << " is annun " << is_rounded_;
					}

					else if (refType & xplmType_FloatArray) {
						xp_dataref_ = VectorFloatXPDataRef(ref, refType, is_rounded_);
						type_ = XplaneType::type_FloatArray;
						LOG() << "Created " << name_ << " Unknown VectorFloatXPDataRef ref " << ref << " is annun " << is_rounded_;
					}
					else if (refType & xplmType_IntArray) {
						xp_dataref_ = VectorIntXPDataRef(ref, refType, is_rounded_);
						type_ = XplaneType::type_IntArray;
						LOG() << "Created " << name_ << " Unknown VectorIntXPDataRef ref " << ref << " is annun " << is_rounded_;
					}
					else if (refType & xplmType_Data) {
						xp_dataref_ = VectorCharXPDataRef(ref, refType, is_rounded_);
						type_ = XplaneType::type_String;
						LOG() << "Created " << name_ << " Unknown VectorCharXPDataRef ref " << ref << " is annun " << is_rounded_;
					}
					break;
				case common::XplaneType::type_Int:
					if (refType & xplmType_Int)
					{
						xp_dataref_ = IntXPDataRef(ref, refType, is_rounded_);
						type_ = XplaneType::type_Int;
						LOG() << "Created " << name_ << " IntXPDataRef ref " << ref << " is annun " << is_rounded_;
					}
					break;


				case common::XplaneType::type_Float:
					if (refType & xplmType_Float)
					{
						xp_dataref_ = FloatXPDataRef(ref, refType, is_rounded_);
						type_ = XplaneType::type_Float;
						LOG() << "Created " << name_ << " FloatXPDataRef ref " << ref << " is annun " << is_rounded_;

					}
					break;

				case common::XplaneType::type_Double:
					if (refType & xplmType_Double)
					{
						xp_dataref_ = DoubleXPDataRef(ref, refType, is_rounded_);
						type_ = XplaneType::type_Double;
						LOG() << "Created " << name_ << " DoubleXPDataRef ref " << ref << " is annun " << is_rounded_;
					}
					break;

				case common::XplaneType::type_FloatArray:
					if (refType & xplmType_FloatArray)
					{
						xp_dataref_ = VectorFloatXPDataRef(ref, refType, is_rounded_);
						type_ = XplaneType::type_FloatArray;
						LOG() << "Created " << name_ << " VectorFlaotXPDataRef ref " << ref << " is annun " << is_rounded_;
					}
					break;
				case common::XplaneType::type_IntArray:
					if (refType & xplmType_IntArray)
					{
						xp_dataref_ = VectorIntXPDataRef(ref, refType, is_rounded_);
						type_ = XplaneType::type_IntArray;
						LOG() << "Created " << name_ << " VectorIntXPDataRef ref " << ref << " is annun " << is_rounded_;
					}
					break;
				case common::XplaneType::type_String:
					if (refType & xplmType_Data)
					{
						xp_dataref_ = VectorCharXPDataRef(ref, refType, is_rounded_);
						type_ = XplaneType::type_String;
						LOG() << "Created " << name_ << " VectorCharXPDataRef ref " << ref << " is annun " << is_rounded_;
					}
					break;
				}
				if (std::holds_alternative<InvalidXPDataRef>(xp_dataref_))
				{
					throw std::invalid_argument(std::string("Cannot find data type for " + name_));
				}
			}
			else
			{
				throw std::invalid_argument(std::string("Cannot find dataref " + name_));
			}
		}
}