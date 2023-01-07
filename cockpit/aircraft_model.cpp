#include "aircraft_model.hpp"

#include "xp_data_ref_repository.hpp"
#include "../common/logger.hpp"
#include "hardware/zcockpit_switch.hpp"
#include "network/connection.hpp"
#include "network/udp.hpp"

#include <ranges>
#include <utility>

extern logger LOG;

namespace zcockpit::cockpit {

	template<typename ... Ts>
	struct Overload : Ts ... {
		using Ts::operator() ...;
	};
	template<class... Ts> Overload(Ts...) -> Overload<Ts...>;

	using namespace common;



	AircraftModel::AircraftModel()
	{
		initialize_ref_strings();
		initialize_zcockpit_data();

		constexpr auto enum_range = [](auto front, auto back) {
		  return std::views::iota(std::to_underlying(front), std::to_underlying(back)) 
		       | std::views::transform([](auto e) { return decltype(front)(e); }); 
		};

		for(const auto data_ref_name : enum_range(DataRefName::acf_tailnum, DataRefName::DataRefName_unused)  ) {
			const DataRefString data_ref_def = data_ref_strings[data_ref_name];
			if(data_ref_def.dataref_name[0] == 0) {
				LOG() << "DataRef Defination Xplane string is empty, DataRefName Index " << static_cast<int>(data_ref_name);
				continue;
			}
			dataref_subscriptions.emplace(data_ref_strings[data_ref_name].dataref_name, 
				DataRefParameter(data_ref_name, data_ref_strings[data_ref_name].type, data_ref_strings[data_ref_name].is_rounded));

			LOG() << "DATAREF " << static_cast<int>(data_ref_name) << " :: " << data_ref_strings[data_ref_name].dataref_name;
		}

		//for(auto& data_ref_name : data_ref_name_list) {
		//	const DataRefString data_ref_def = data_ref_strings[data_ref_name];
		//	if(data_ref_def.dataref_name[0] == 0) {
		//		LOG() << "DataRef Defination Xplane string is empty, DataRefName Index " << static_cast<int>(data_ref_name);
		//		continue;
		//	}
		//	dataref_subscriptions.emplace(data_ref_strings[data_ref_name].dataref_name, 
		//		DataRefParameter(data_ref_name, data_ref_strings[data_ref_name].type, data_ref_strings[data_ref_name].is_rounded));
		//}

		for(auto& command_ref_name : command_ref_name_list) {
			auto cmdref_name = command_ref_strings[command_ref_name].cmdref_name;
			if(cmdref_name != nullptr){
				commandref_subscriptions.emplace(cmdref_name, CommandRefParameter(command_ref_name));
			}
			else {
				LOG() << "ERROR: Subscription String Array 'command_ref_strings' has no entry for enum " << static_cast<int>(command_ref_name); 
			}
		}
	}

	// xplane[1, 0, -1]  up==test, center==brt, dn==dim  ==> 0,1,2
	//DataRefName::bright_test:

	// xplane[0,1,2,3,4,5] rto, off, 1,2,3,max  ==> // 0: RTO  1: OFF ... 5: MAX
	//DataRefName::autobrake_pos:

	// xplane[0.0, 0.5, 1.0] 0-up, 0.5-off, 1-down		==> // 0: UP  1: OFF  2: DOWN
	//DataRefName::landing_gear:

	// xplane[0,1,2,3,4,5,6] auto v1,vr,wt,vref,bug,set ==>  0: SET  1: AUTO  2: V1  3: VR  4: WT  5: VREF  6: Bug  
	//DataRefName::spd_ref:

	// xplane[-1, 0, 1] off, bat, on  
	//DataRefName::battery_pos:
	std::vector<packet_data_t> AircraftModel::build_xplane_commands(const hardware::ZcockpitSwitch switch_data, const SwitchCommands* commands, int xplane_value,
	                                                                const int hw_value)
	{
		std::vector<packet_data_t> xplane_commands;
		if (commands != nullptr) {
			const auto dataref_name = switch_data.dataref_name;

			if(switch_data.switch_type == common::SwitchType::multiposition) {
				
			}
			else if(commands->size >= 2 || switch_data.switch_type == common::SwitchType::spring_loaded) {
				int direction = 1;
				CommandRefName command_ref_name = commands->names[0];
				const auto cmd_ref_string = get_cmd_ref_string(command_ref_name);

				if (hw_value <= xplane_value && commands->size == 2) {
					// send decrease command
					command_ref_name = commands->names[1];
					direction = -1;
				}

				if (cmdref_to_ref_id.contains(command_ref_name)) {
					auto cmd_ref_id = cmdref_to_ref_id[command_ref_name];
					//
					// Toggle
					if (switch_data.switch_type == common::SwitchType::toggle) {
						xplane_commands.emplace_back(XplaneCommand(cmd_ref_id, XplaneCommand_enum::once));
						LOG() << "Toggle Command once " << cmd_ref_string << " id " << cmd_ref_id;
					}
					//
					// Spring Loaded
					else if (switch_data.switch_type == common::SwitchType::spring_loaded) {
						if (hw_value == switch_data.spring_loaded_return_value) {
							// end command
							if (spring_loaded_switch_cmd_ref_id.contains(dataref_name) && spring_loaded_switch_cmd_ref_id[dataref_name] >= 0)
							{
								cmd_ref_id = spring_loaded_switch_cmd_ref_id[dataref_name];
								spring_loaded_switch_cmd_ref_id[dataref_name] = -1;
								xplane_commands.emplace_back(XplaneCommand(cmd_ref_id, XplaneCommand_enum::end));
								LOG() << "Command end  id " << cmd_ref_id;
							}
						}
						else {
							// begin command
							if (!spring_loaded_switch_cmd_ref_id.contains(dataref_name) || spring_loaded_switch_cmd_ref_id[dataref_name] == -1)
							{
								spring_loaded_switch_cmd_ref_id[dataref_name] = cmd_ref_id;
								xplane_commands.emplace_back(XplaneCommand(cmd_ref_id, XplaneCommand_enum::begin));
								LOG() << "Command begin " << cmd_ref_string << " id " << cmd_ref_id;
							}
						}
					}
					//
					// Rotary
					else if (switch_data.switch_type == common::SwitchType::rotary) {
						if (!rotary_switch_value.contains(dataref_name)) {
							// initialize to current xplane value -- so we can sync to Hardware 
							rotary_switch_value[dataref_name] = xplane_value;
						}
						int switch_value = rotary_switch_value[dataref_name];

						LOG() << "HW = " << hw_value << " switch value = " << switch_value; 

						direction = 1;
						command_ref_name = commands->names[0];
						if (hw_value <= switch_value) {
							// send decrease command
							command_ref_name = commands->names[1];
							direction = -1;
						}
						const auto cmd_ref_string = get_cmd_ref_string(command_ref_name);


						while (hw_value != switch_value) {
							xplane_commands.emplace_back(XplaneCommand(cmd_ref_id, XplaneCommand_enum::once));
							switch_value += direction;
							LOG() << "Rotary Command once " << cmd_ref_string << " id " << cmd_ref_id;
						}
						rotary_switch_value[dataref_name] = hw_value;
					}
					else {
						LOG() << "Unsupported Command " << cmd_ref_string << " id " << cmd_ref_id;
					}
				}
				else {
					LOG() << "XPlane Command: refId not found for dataref " << static_cast<int>(dataref_name);
				}
			}
		}
		else {
			LOG() << "XPlane Command: commands is null";
		}
		
		return xplane_commands;
	}

	// Switch that change XPlane State by changing DatRefs values
	std::vector<common::packet_data_t> AircraftModel::process_hw_switch_values(const hardware::ZcockpitSwitch sw_data, const ZCockpitSwitchData switch_data)
	{
		auto const dataref_name = sw_data.dataref_name;
		std::vector<packet_data_t> xplane_dataref;
		
		const auto operation = (static_cast<SwitchValues*>(switch_data.operation));

		if(switch_data.hw_type == ZCockpitType::ZFloat) {
			float xplane_value = *(static_cast<float*>(switch_data.xplane_data));
			const float float_hw_value = sw_data.float_hw_value;
			if(sw_data.switch_type == SwitchType::encoder) {
				//if (dataref_name == DataRefName::spd_ref_adjust) {
				//	LOG() << "spdref xp = " << xplane_value << " hw = " << float_hw_value;
				//}

				xplane_value += float_hw_value;
				if(xplane_value > operation->max_value) {
						xplane_value = operation->max_value;
				}
				else if(xplane_value < operation->min_value) {
					xplane_value = operation->min_value;
				}
				*(static_cast<float*>(switch_data.xplane_data)) = xplane_value;

				common::var_t variant = xplane_value;
				int data_ref_id = dataref_to_ref_id[dataref_name];
				common::SetDataRef set_data_ref(data_ref_id, variant);
				xplane_dataref.emplace_back(set_data_ref);
				LOG() << "Send DataRef " << get_data_ref_string(dataref_name) << " id " << data_ref_id;
			}
			else if(float_hw_value != xplane_value) {
				if(dataref_to_ref_id.contains(dataref_name)){
					// change xplane_value so send change multiple times
					*(static_cast<float*>(switch_data.xplane_data)) = float_hw_value;

					int data_ref_id = dataref_to_ref_id[dataref_name];
					common::var_t variant = float_hw_value;
					common::SetDataRef set_data_ref(data_ref_id, variant);
					xplane_dataref.emplace_back(set_data_ref);
					LOG() << "Send DataRef " << get_data_ref_string(dataref_name) << " id " << data_ref_id;
				}
			}
		}
		else if(switch_data.hw_type == ZCockpitType::ZVectorFloat) {
			auto size = operation->size;
			std::vector<float> xplane_values = *(static_cast<std::vector<float>*>(switch_data.xplane_data));
			xplane_values.resize(size);
			const int index = sw_data.offset;
			if(index < xplane_values.size()) {
				if(sw_data.switch_type == SwitchType::encoder) {
					xplane_values[index] += sw_data.float_hw_value;
					if(xplane_values[index] > operation->max_value) {
						xplane_values[index] = operation->max_value;
					}
					else if(xplane_values[index] < operation->min_value) {
						xplane_values[index] = operation->min_value;
					}
					LOG() << "Vector Float index " << index << " increment " << sw_data.float_hw_value << " new value " << xplane_values[index];
					common::var_t variant = xplane_values;
					int data_ref_id = dataref_to_ref_id[dataref_name];
					common::SetDataRef set_data_ref(data_ref_id, variant);
					xplane_dataref.emplace_back(set_data_ref);
					LOG() << "Send DataRef " << get_data_ref_string(dataref_name) << " id " << data_ref_id;
				}
				else
				{
					LOG() << "Expecting Switch Encoder, other types not supported";
				}
			}

		}
		return xplane_dataref;
	}


	std::vector<common::packet_data_t> AircraftModel::process_hw_switch_commands(const hardware::ZcockpitSwitch sw_data, const ZCockpitSwitchData switch_data)
	{
		auto const dataref_name = sw_data.dataref_name;
		auto const int_hw_value = sw_data.int_hw_value;
		auto const sw_type = sw_data.switch_type;
		std::vector<packet_data_t> xplane_command;

		const auto commands = (static_cast<SwitchCommands*>(switch_data.operation));

			if(common::SwitchType::pushbutton == sw_type) {

				if(commands != nullptr && commands->size == 1) {
					const CommandRefName command_ref_name = commands->names[0];
					if(cmdref_to_ref_id.contains(command_ref_name)){
						auto cmd_ref_id = cmdref_to_ref_id[command_ref_name];
						xplane_command.emplace_back(XplaneCommand(cmd_ref_id, XplaneCommand_enum::once));
						LOG() << "Pushbutton Command once " << get_cmd_ref_string(command_ref_name) << " id " << cmd_ref_id;
					}
				}
			}
			else if(sw_type == common::SwitchType::multiposition) {
				if(int_hw_value < commands->size) {
					const CommandRefName command_ref_name = commands->names[int_hw_value];
					if(cmdref_to_ref_id.contains(command_ref_name)){
						auto cmd_ref_id = cmdref_to_ref_id[command_ref_name];
						xplane_command.emplace_back(XplaneCommand(cmd_ref_id, XplaneCommand_enum::once));
						LOG() << "Multiposition Command once " << get_cmd_ref_string(command_ref_name) << " id " << cmd_ref_id;
					}
				}
			}
			else if(sw_type == common::SwitchType::rotary || sw_type == common::SwitchType::toggle || sw_type == common::SwitchType::spring_loaded)
			{
				const int xplane_value = *(static_cast<int*>(switch_data.xplane_data));
				if (int_hw_value != xplane_value || (sw_type == common::SwitchType::spring_loaded && int_hw_value == sw_data.spring_loaded_return_value)) {
					xplane_command = build_xplane_commands(sw_data, commands, static_cast<int>(xplane_value), int_hw_value);
				}
			}
			else {
				LOG() << "unknows hardware switch type";
			}

		return xplane_command;
	}

	[[nodiscard]] std::vector<common::packet_data_t> AircraftModel::process_hw_switch(const hardware::ZcockpitSwitch sw_data)
	{
		auto const dataref_name = sw_data.dataref_name;
		if (std::holds_alternative<ZCockpitSwitchData>(z_cockpit_data[dataref_name])) 
		{
			const ZCockpitSwitchData switch_data = std::get<ZCockpitSwitchData>(z_cockpit_data[dataref_name]);
			if(switch_data.operation){
				if(switch_data.operation_type == OperationType::Command)
				{
					return process_hw_switch_commands(sw_data, switch_data);
				}
				else if(switch_data.operation_type == OperationType::Value)

				{
					return process_hw_switch_values(sw_data, switch_data);
				}
				else {
					LOG() << "Unexpected switch type " << get_data_ref_string(dataref_name);
				}
			}
			else {
				LOG() << "Switch Operation is null";
			}
		}
		return {};
	}

	void AircraftModel::process_data_packet(const DataPacket& data_packet)
	{
		auto& changed_data = data_packet.data_;

		for (const auto& [id, value] : changed_data) {
			try {
				auto& ac_param = ref_id_to_dataref.at(id);

				auto TypesOfValue = Overload
				{

					// Int
					[this, ac_param, id](const int data)
					{
						// TODO process INT
						LOG() << "ERROR unexpected INT type for " << data_ref_strings[ac_param.short_name].dataref_name;
					},
					// Float
					[this, ac_param, id](const float data)
					{
						//
						// Switches
						if (std::holds_alternative<ZCockpitSwitchData>(z_cockpit_data[ac_param.short_name])) {
							const ZCockpitSwitchData switch_data = std::get<ZCockpitSwitchData>(z_cockpit_data[ac_param.short_name]);
							// record the xplane switch state
							if(switch_data.hw_type == ZCockpitType::ZInt || switch_data.hw_type == ZCockpitType::ZBool) {
								*(static_cast<int*>(switch_data.xplane_data)) = static_cast<int>(data);
								LOG() << "Packet IN data: SwitchData  " << data_ref_strings[ac_param.short_name].dataref_name << " in data = " << data << " --> xplane_data = " << *(static_cast<int*>(switch_data.xplane_data)) ;
							}
							else if(switch_data.hw_type == ZCockpitType::ZFloat) {
								*(static_cast<float*>(switch_data.xplane_data)) = data;
								LOG() << "Packet IN data: SwitchData  " << data_ref_strings[ac_param.short_name].dataref_name << " in data = " << data << " --> xplane_data = " << *(static_cast<float*>(switch_data.xplane_data)) ;
							}
							else {
								LOG() << "ERROR: Packet IN data: Type Conversion NOT Supported for SwitchData 'xplane_data' " << data_ref_strings[ac_param.short_name].dataref_name;
							}
						}
						//
						// Annunciators
						else {
							const ZCockpitInData annun_data = std::get<ZCockpitInData>(z_cockpit_data[ac_param.short_name]);

							if (annun_data.hw_type == ZCockpitType::ZBool) {
								if (data == 0.0f) {
									*(static_cast<bool*>(annun_data.in_data)) = false;
									LOG() << data_ref_strings[ac_param.short_name].dataref_name << " changed to false";
								}
								else {
									*(static_cast<bool*>(annun_data.in_data)) = true;
									LOG() << data_ref_strings[ac_param.short_name].dataref_name << " changed to true";
								}
							}
							else {
							*(static_cast<float*>(annun_data.in_data)) = data;
								LOG() << data_ref_strings[ac_param.short_name].dataref_name << " changed to " << data;
							}
						}
					},

					[this, ac_param, id](const double data)
					{
						//*(static_cast<double*>(z_cockpit_data[ac_param.short_name].in_data)) = data;
					},
					[this, ac_param, id](const std::vector<int>& data)
					{

						//const auto& items = (std::vector<int>&)data;

					},
					[this, ac_param, id](const std::vector<float>& data)
					{
						//
						// Switche Data
						if (std::holds_alternative<ZCockpitSwitchData>(z_cockpit_data[ac_param.short_name])) {
							const ZCockpitSwitchData switch_data = std::get<ZCockpitSwitchData>(z_cockpit_data[ac_param.short_name]);
							// record the xplane switch state
							*(static_cast<std::vector<float>*>(switch_data.xplane_data)) = static_cast<std::vector<float>>(data);
							LOG() << "Switch Data name index " << (int)(ac_param.short_name)  << " = " << data[0];
						}
						else {
							const ZCockpitInData in_data = std::get<ZCockpitInData>(z_cockpit_data[ac_param.short_name]);
							
						}

						//const auto& items = (std::vector<float>&)data;

					},
					[this, ac_param, id](const std::vector<char>& data)
					{
						if (ac_param.xplane_type == XplaneType::type_String && std::holds_alternative<ZCockpitInData>(z_cockpit_data[ac_param.short_name]))
						{
							const ZCockpitInData in_data = std::get<ZCockpitInData>(z_cockpit_data[ac_param.short_name]);
							for (auto i = 0; i < data.size(); i++) {
								char* ptr = static_cast<char*>(in_data.in_data);
								if (i < 40) {
									ptr[i] = data[i];
								}
								if (*ptr == '\0') {
									break;
								}
							}
							LOG() << "ID " << id << " " << data_ref_strings[ac_param.short_name].dataref_name << " changed to " << z737InData.tail_number;;
						}
					},
					[](auto& data) {}

				};
				std::visit(TypesOfValue, value);

			}
			catch (std::out_of_range) {
				LOG() << "Received unscribed data  id = " << id << " value  variant index " << value.index();
			}
		}
	}

	[[nodiscard]] int AircraftModel::process_dataref_conformation_packet(const std::vector<DataRefConfirmation>& confirmations, std::unordered_map<std::string, DataRefParameter>& requested_subscriptions)
	{
		int actual_number_of_subscriptions = 0;
		for (const auto& [name, id, data_type, value] : confirmations) {

			if (requested_subscriptions.contains(name)) {
				DataRefParameter& ac_param = requested_subscriptions.at(name);

				if (data_type == ac_param.xplane_type) {
					actual_number_of_subscriptions += 1;

					auto TypesOfValue = Overload{

						// Int
						[this, ac_param, id, data_type](const int data)
						{
							if (XplaneType::type_Int == data_type) {
								if (std::holds_alternative<ZCockpitInData>(z_cockpit_data[ac_param.short_name])) {
									const ZCockpitInData in_data = std::get<ZCockpitInData>(z_cockpit_data[ac_param.short_name]);

									ref_id_to_dataref.emplace(std::pair(id, ac_param));
									dataref_to_ref_id.emplace(ac_param.short_name, id);
									*(static_cast<int*>(in_data.in_data)) = data;
									return std::string("int = ") + std::to_string(data);
								}
							}
						return  std::string("unknown");
						},
						// Float
						[this, ac_param, id, data_type](const float data)
						{
							if (XplaneType::type_Float == data_type) {
								ref_id_to_dataref.emplace(std::pair(id, ac_param));
								dataref_to_ref_id.emplace(ac_param.short_name, id);
								//
								// Switches
								if (std::holds_alternative<ZCockpitSwitchData>(z_cockpit_data[ac_param.short_name])) {
									const ZCockpitSwitchData switch_data = std::get<ZCockpitSwitchData>(z_cockpit_data[ac_param.short_name]);

									// record the xplane switch state
									if (switch_data.hw_type == ZCockpitType::ZInt) {
										*(static_cast<int*>(switch_data.xplane_data)) = static_cast<int>(data);
										return std::string("Switch float to Int = ") + std::to_string(data) + " to " + std::to_string(static_cast<int>(data));
									}
									else if(DataRefName::landing_gear == ac_param.short_name) {
										// Landing Gear needs to be scaled by 2.0  -- up=0, off=0.5, dn=1.0	-->	goes to up=0, off=1, dn=2
										const float scaled_data = data * 2.0;
										*(static_cast<int*>(switch_data.xplane_data)) = static_cast<int>(scaled_data);
										return std::string("Switch float scaled  = ") + std::to_string(data) + " scaled to " + std::to_string(scaled_data);

									}
									else if (switch_data.hw_type == ZCockpitType::ZFloat) {
										*static_cast<float*>(switch_data.xplane_data) = data;
										LOG() << "Confirmation Float Data " << data_ref_strings[ac_param.short_name].dataref_name << " = " << data;
										return std::string("Switch float = ") + std::to_string(data);

									}
									else {
										LOG() << "ERROR: expected type float for " << data_ref_strings[ac_param.short_name].dataref_name;
									}
								}
								//
								// Annunciators
								else {
									const ZCockpitInData annun_data = std::get<ZCockpitInData>(z_cockpit_data[ac_param.short_name]);

									if (annun_data.hw_type == ZCockpitType::ZBool) {
										if (data == 0.0f) {
											*(static_cast<bool*>(annun_data.in_data)) = false;
											LOG() << data_ref_strings[ac_param.short_name].dataref_name << " = false";
											return std::string("bool = false");
										}
										else {
											*(static_cast<bool*>(annun_data.in_data)) = true;
											LOG() << data_ref_strings[ac_param.short_name].dataref_name << " = true";
											return std::string("bool = true");
										}
									}
									else {
									*(static_cast<float*>(annun_data.in_data)) = data;
										LOG() << data_ref_strings[ac_param.short_name].dataref_name << " = " << data;
										return std::string("float = ") + std::to_string(data);
									}
								}
							}
							return  std::string("unknown");
						},


						[this, ac_param, id, data_type](const double data)
						{
							if (XplaneType::type_Double == data_type && std::holds_alternative<ZCockpitInData>(z_cockpit_data[ac_param.short_name])) {
								const ZCockpitInData in_data = std::get<ZCockpitInData>(z_cockpit_data[ac_param.short_name]);
								ref_id_to_dataref.emplace(std::pair(id, ac_param));
								dataref_to_ref_id.emplace(ac_param.short_name, id);
								*(static_cast<double*>(in_data.in_data)) = data;
								return std::string("double = ") + std::to_string(data);
							}
							return  std::string("unknown");
						},
						[this, ac_param, id, data_type](const std::vector<int>& data)
						{
							if (XplaneType::type_IntArray == data_type) {
								ref_id_to_dataref.emplace(std::pair(id, ac_param));
								dataref_to_ref_id.emplace(ac_param.short_name, id);
								const auto& items = (std::vector<int>&)data;
								return std::string("intArray = NOT IMPLEMENTED");
							}
							return  std::string("unknown");
						},
						[this, ac_param, id, data_type](const std::vector<float>& data)
						{
							if (XplaneType::type_FloatArray == data_type) {
								if (std::holds_alternative<ZCockpitSwitchData>(z_cockpit_data[ac_param.short_name])) {
									const ZCockpitSwitchData switch_data = std::get<ZCockpitSwitchData>(z_cockpit_data[ac_param.short_name]);
									ref_id_to_dataref.emplace(std::pair(id, ac_param));
									dataref_to_ref_id.emplace(ac_param.short_name, id);
									*static_cast<std::vector<float>*>(switch_data.xplane_data) = data;
									return std::string("Vector Float size ") + std::to_string(data.size());
								}


								return std::string("floatArray = nOT IMPLEMENYED");

							}
							return  std::string("unknown");
						},
						[this, ac_param, id, data_type](const std::vector<char>& data)
						{
							if (XplaneType::type_String == data_type && std::holds_alternative<ZCockpitInData>(z_cockpit_data[ac_param.short_name])) {
								const ZCockpitInData in_data = std::get<ZCockpitInData>(z_cockpit_data[ac_param.short_name]);
								ref_id_to_dataref.emplace(std::pair(id, ac_param));
								dataref_to_ref_id.emplace(ac_param.short_name, id);
								for (auto i = 0; i < data.size(); i++) {
									char* ptr = static_cast<char*>(in_data.in_data);
									if (i < 40) {
										ptr[i] = data[i];
									}
									if (*ptr == '\0') {
										break;
									}
								}
								return std::string("Byte Array = ") + std::string(static_cast<char*>(in_data.in_data));
							}
							return  std::string("unknown");
						},
						[this, ac_param, id](auto& data)
						{
							return  std::string("unknown");
						}

					};
					auto str_data = std::visit(TypesOfValue, value);

					LOG() << "Confirmation " << name << " id " << id << "\t\t\t\t" << str_data;

					/////////////////////////////////////////////
					if (ac_param.short_name == DataRefName::acf_tailnum && std::strlen(z737InData.tail_number) > 0) {
						auto tail_num = std::string(z737InData.tail_number);
						if ("ZB738" == tail_num) {
							z738_available = true;
						}
					}
				}
				else {
					// type mismatch
					LOG() << "DataRef " << name << " type mismatch expected " << XplaneTypeToString(ac_param.xplane_type) << " but received " << XplaneTypeToString(data_type);
				}
				// clear current subscription
				requested_subscriptions.erase(name);
			}
			else {
				LOG() << "ERROR: " << name << " Not In Subscribed DataRefs";
			}
		}
		if(requested_subscriptions.size() < 4) {
			for(auto& item : requested_subscriptions) {
				LOG() << "Still in Requeste Subscription: " << item.first;
			}
		}
		return actual_number_of_subscriptions;
	}

	//
	// COMMAND CONFIRMATION
	//
	[[nodiscard]] int AircraftModel::process_command_conformation_packet(const std::vector<CommandConfirmation>& confirmations, std::unordered_map<std::string, CommandRefParameter>& requested_subscriptions)
	{
		int actual_number_of_subscriptions = 0;
		for (const auto& [name, id] : confirmations) {
			if (requested_subscriptions.contains(name)) {
				CommandRefParameter& ac_param = requested_subscriptions.at(name);

				LOG() << "Received Comfirmation CmdRef " << name << " id " << id << " datarefName index " << static_cast<int>(ac_param.short_name);
				cmdref_to_ref_id[ac_param.short_name] = id;

				// clear current subscription
				requested_subscriptions.erase(name);
				actual_number_of_subscriptions += 1;

			}
		}
		return  actual_number_of_subscriptions;
	}

	void AircraftModel::clear_ref_id_lists()
	{
		ref_id_to_dataref.clear();
		dataref_to_ref_id.clear();
		cmdref_to_ref_id.clear();
	}

	void AircraftModel::push_switch_change(hardware::ZcockpitSwitch switch_data)
	{
		pending_switch_changes.push(switch_data);
		LOG() << "pushed switch change " << get_data_ref_string(switch_data.dataref_name) << " value " << switch_data.int_hw_value << " or " << switch_data.float_hw_value;
	}

	std::vector<packet_data_t> AircraftModel::update_switch_values()
	{
		std::vector<packet_data_t> xplane_commands{};

		while(pending_switch_changes.size() > 0) {
			if(auto maybe_switch_data = pending_switch_changes.pop()) {
				if(maybe_switch_data) {
					hardware::ZcockpitSwitch switch_data = *maybe_switch_data;

					LOG() << "Pop pending Switch changes " << get_data_ref_string(switch_data.dataref_name) << " value " << switch_data.int_hw_value << " or " << switch_data.float_hw_value;
					auto commands = process_hw_switch(switch_data);
					xplane_commands.insert(xplane_commands.end(), commands.begin(), commands.end());
				}
			}

		}
		return xplane_commands;
	}

	std::vector<common::packet_data_t> AircraftModel::open_guards()
	{
		return process_hw_switch( hardware::ZcockpitSwitch(DataRefName::guarded_covers, SwitchType::toggle, 1.0f));

	}
}
