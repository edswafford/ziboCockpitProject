#pragma once

#include <optional>
#include <string>
#include <map>
#include <ranges>
#include <variant>
#include <tuple>
#include "shared_types.hpp"
#include "xp_data_ref.hpp"

#include "logger.hpp"
#include "packet.hpp"


extern logger LOG;

namespace zcockpit::common
{

	class XpDataRefRepository
	{
		int next_id_{ 0 };

		std::unordered_map<std::string, int> subscribed_dataref_id_by_name;
		std::unordered_map<int, std::unique_ptr<XPDataRef>> subscribed_dataref_by_id;

	public:
		std::optional<int> subscribe_dataref(const SubscribeData& xplane_dataref)
		{
			// reset next_id
			if(next_id_ > 0 && subscribed_dataref_by_id.empty()) {
				next_id_ = 0;
			}

			auto id = next_id_;
			auto& name = xplane_dataref.dataref_string;

			if (subscribed_dataref_id_by_name.contains(name)) {
				id = subscribed_dataref_id_by_name[name];
				const auto& data_ref = subscribed_dataref_by_id.at(id);
				data_ref->increment_subscribers();
				LOG() << "DataRef  id " << next_id_  << ", " << name << " Already Subscribed";
			}
			else {
				try {
					auto xp_data_ref = std::make_unique<XPDataRef>( name, xplane_dataref.requested_type, xplane_dataref.is_boolean_annun);
					subscribed_dataref_by_id.emplace(std::make_pair(next_id_, std::move(xp_data_ref)));
					subscribed_dataref_id_by_name.emplace(std::make_pair(name, id));
					//LOG() << next_id_ << " DataRef " << name << " Subscribed is_annun " << xplane_dataref.is_boolean_annun;

					next_id_ += 1;

				}
				catch(std::exception& ex) {
					LOG() << "Cannot create DataRef for " << name << " " << ex.what();
					return {};
				}
			}
			return id;
		}

		void unsubscribe(const int id)
		{
			if(subscribed_dataref_by_id.contains(id)) {
				const auto& dataref = subscribed_dataref_by_id.at(id);
				const auto count = dataref->decrement_subscribers();
				if(count == 0) {
					subscribed_dataref_id_by_name.erase(dataref->name());
					subscribed_dataref_by_id.erase(id);
				}
			}
		}


		void update_values()
		{
			XPDataRef::update_new_values(subscribed_dataref_by_id);
		}
		void update_values_for_requested_ids(const std::vector<int>& requested_id)
		{
			XPDataRef::update_values_for_requested_ids(requested_id, subscribed_dataref_by_id);
		}
		void set_xplane_dataref_values(const std::vector<std::tuple<int, var_t>>& values)
		{
			XPDataRef::set_values_for_datarefs(values, subscribed_dataref_by_id);
		}

		[[nodiscard]] std::unordered_map<int, common::var_t> changed_values()
		{
			return XPDataRef::changed_values(subscribed_dataref_by_id);
		}
		[[nodiscard]] std::unordered_map<int, common::var_t>  values_for_requested_ids(const std::vector<int>& requested_id)
		{
			return XPDataRef::values_for_requested_ids(requested_id, subscribed_dataref_by_id);
		}

		[[nodiscard]] confirmation_t build_dataref_confirmation_data(const std::unordered_map<int, common::var_t>& values) const
		{
			std::vector<DataRefConfirmation> confirmation_data;
			for (auto& [id, value] : values) {

				const auto& data_ref = subscribed_dataref_by_id.at(id);
				
				confirmation_data.push_back(DataRefConfirmation(data_ref->name(), id, data_ref->type(), value));

			}
			return confirmation_data;
		}


		[[nodiscard]] static std::vector<SetDataRef> build_set_dataref_data(std::vector<std::tuple<int, common::var_t>> datarefs_values)
		{
			std::vector<SetDataRef> dataref_data;
			for (auto& [id, value] : datarefs_values) {
				dataref_data.push_back(SetDataRef(id, value));
			}
			return dataref_data;
		}

	};
}