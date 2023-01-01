#pragma once

#include <cassert>
#include <optional>
#include <string>

#include <map>
#include <unordered_map>
#include <variant>
#include <vector>
#include "../common/shared_types.hpp"
#include "aircraft_parameters.hpp"
#include "z737_switch_data.hpp"
#include "z737_annun_data.hpp"
#include "packet.hpp"
#include "queue.hpp"
#include "hardware/zcockpit_switch.hpp"

using namespace zcockpit::common;

namespace zcockpit::cockpit {
	struct ZCockpitSwitchData
	{
		common::ZCockpitType hw_type;
		common::OperationType operation_type;
		void* xplane_data;
		void* operation;
		ZCockpitSwitchData(ZCockpitType hw_type_, void* xplane_data_, void* operation_, OperationType operation_type_ = OperationType::Command) :
			hw_type(hw_type_), operation_type(operation_type_), xplane_data(xplane_data_), operation(operation_)
		{}
	};
	struct ZCockpitInData
	{
		void* in_data;
		common::ZCockpitType hw_type;
	};

	using z_class_t = std::variant<ZCockpitInData, ZCockpitSwitchData>;

	struct ZCockpitData
	{
		ZCockpitData(z_class_t xp_data) : zcockpit_data(xp_data) {}
		z_class_t zcockpit_data;

	};


	struct DataRefString
	{
		const char* dataref_name{ "" };
		common::XplaneType type{ common::XplaneType::type_Unknown };
		bool is_rounded{ false };
	};

	struct CommandRefString
	{
		const char* cmdref_name;
	};

	class AircraftModel
	{
	public:

		AircraftModel();
		//	void open_covers();


		[[nodiscard]] std::vector<common::packet_data_t> build_xplane_commands(const hardware::ZcockpitSwitch switch_data, const SwitchCommands* commands, int xplane_value,
		                                                                       const int hw_value);
		std::vector<common::packet_data_t> process_hw_switch_values(hardware::ZcockpitSwitch sw_data, ZCockpitSwitchData switch_data);
		std::vector<common::packet_data_t> process_hw_switch_commands(hardware::ZcockpitSwitch sw_data, ZCockpitSwitchData switch_data);


		[[nodiscard]] std::vector<common::packet_data_t> process_hw_switch(const hardware::ZcockpitSwitch switch_data);


		void process_data_packet(const common::DataPacket& data_packet);
		[[nodiscard]] int process_dataref_conformation_packet(const std::vector<common::DataRefConfirmation>& confirmations, std::unordered_map<std::string, DataRefParameter>& requested_subscriptions);
		[[nodiscard]] int  process_command_conformation_packet(const std::vector<common::CommandConfirmation>& confirmations, std::unordered_map<std::string, CommandRefParameter>& requested_subscriptions);

		[[nodiscard]] const char* get_data_ref_string(const DataRefName param) const {
			if (data_ref_strings.size() > static_cast<int>(param) && data_ref_strings[param].dataref_name != nullptr) {
				return data_ref_strings[param].dataref_name;
			}

			return "Unknown DataRefName";
		}
		[[nodiscard]] const char* get_cmd_ref_string(const CommandRefName param) const {
			if (command_ref_strings.size() > static_cast<int>(param) && command_ref_strings[param].cmdref_name != nullptr) {
				return command_ref_strings[param].cmdref_name;
			}

			return "Unknown DataRefName";
		}


		bool z738_is_available() const { return z738_available; }
		bool z738_ac_power_is_on() const { return (z737InData.ac_tnsbus1_status || z737InData.ac_tnsbus2_status);}
		void clear_ref_id_lists();

		void push_switch_change(hardware::ZcockpitSwitch switch_data);

		[[nodiscard]] std::vector<common::packet_data_t> update_switch_values();
		std::vector<common::packet_data_t> open_guards();


		std::unordered_map<int, DataRefParameter> ref_id_to_dataref;
		std::unordered_map<DataRefName, int>dataref_to_ref_id;
		std::unordered_map<CommandRefName, int>cmdref_to_ref_id;


		std::unordered_map<std::string, DataRefParameter> dataref_subscriptions;
		std::unordered_map<std::string, CommandRefParameter> commandref_subscriptions;

		Z737AnnunData z737InData{};
		Z737SwitchData hw_switch_data{};
		Z737SwitchData xplane_switch_data{};
		Z737SwitchCmd z737SwitchCmd{};
		Z737SwitchValue z737SwitchValue{};

	private:
		bool z738_available{ false };


		common::EnumArray<DataRefName, z_class_t> z_cockpit_data;
		common::EnumArray<DataRefName, DataRefString> data_ref_strings;
		common::EnumArray<CommandRefName, CommandRefString> command_ref_strings;

		common::ThreadSafeQueue<hardware::ZcockpitSwitch> pending_switch_changes;

		std::unordered_map<DataRefName, int>spring_loaded_switch_cmd_ref_id{};
		std::unordered_map<DataRefName, int>rotary_switch_value{};

		void initialize_zcockpit_data();
		void initialize_ref_strings();



	};



}
