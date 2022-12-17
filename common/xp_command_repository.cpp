#include "xp_command_repository.hpp"


namespace zcockpit::common
{
	std::optional<int> XpCommandRepository::subscribed_command(const std::string& requested_command)
	{
		// reset next_id
		if (next_id_ > 0 && subscribed_command_by_id.empty()) {
			next_id_ = 0;
		}

		auto id = next_id_;

		if (subscribed_command_id_by_name.contains(requested_command)) {
			id = subscribed_command_id_by_name[requested_command];
			const auto& command = subscribed_command_by_id.at(id);
			command->increment_subscribers();
		}
		else {
			try {
				// Create XpCommand and store it in map
				subscribed_command_by_id.emplace(std::make_pair(next_id_, std::make_unique<XpCommand>(requested_command)));
				LOG() << "Created XpCommand " << requested_command << " id = " << next_id_;
				subscribed_command_id_by_name.emplace(std::make_pair(requested_command, id));
				next_id_ += 1;
			}
			catch (std::exception& ex) {
				LOG() << "Cannot create  " << requested_command << " " << ex.what();
				return {};
			}
		}
		return id;
	}
}
