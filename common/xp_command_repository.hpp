#pragma once
#include <optional>
#include "shared_types.hpp"
#include "xp_command.hpp"

#include "logger.hpp"
#include "packet.hpp"




extern logger LOG;


namespace zcockpit::common
{


	class XpCommandRepository
	{
		int next_id_{ 0 };
		std::unordered_map<std::string, int> subscribed_command_id_by_name;
		std::unordered_map<int, std::unique_ptr<XpCommand>> subscribed_command_by_id;

	public:
		std::optional<XpCommand> get_XpCommand(int id)
		{
			if(subscribed_command_by_id.contains(id)){
				auto command = *subscribed_command_by_id.at(id);
				if(command.valid_ref()) {
					return command;
				}
			}
			return {};
		}

		std::optional<int> subscribed_command(const std::string& requested_command);

		void unsubscribe(const int id)
		{
			if(subscribed_command_by_id.contains(id)) {
				const auto& cmdref = subscribed_command_by_id.at(id);
				const auto count = cmdref->decrement_subscribers();
				if(count == 0) {
					subscribed_command_id_by_name.erase(cmdref->name());
					subscribed_command_by_id.erase(id);
				}
			}
		}

		[[nodiscard]] confirmation_t  build_confirmation_data(const std::vector<int>& ids)
		{
			std::vector<CommandConfirmation> confirmation_data;
			for(auto& id : ids) {
				const auto& command = subscribed_command_by_id.at(id);
				confirmation_data.push_back(CommandConfirmation(command->name(), id) );
			}
			return confirmation_data;
		}
	};
}
