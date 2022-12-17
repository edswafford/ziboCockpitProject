#include "server.hpp"
#include "../common/xp_data_ref.hpp"
#include "../common/xp_command.hpp"


namespace zcockpit::plugin
{


	Server::Server(std::unique_ptr<Udp> udp_ptr) : udp_(std::move(udp_ptr))
	{
		group_ip_ = udp_->string_to_ip(Udp::kMulticastServerGroupIP);
		if (group_ip_ == 0) {
			throw std::runtime_error("Cannot create group ip address ... terminating");
		}
		multicast_connection_ = std::make_unique<Connection<Udp>>(*udp_, 0, 0);
		if (!multicast_connection_->create_receiver(Udp::kServerMulticastRxPortAddress, true)) {
			throw std::runtime_error("Cannot create Multicast Receiver Port");
		}
		if (!multicast_connection_->create_transmitter(group_ip_, Udp::kServerMulticastTxPortAddress)) {
			throw std::runtime_error("Cannot create Multicast Transmitter Port");
		}
		const std::string ip_address = udp_->get_ip();
		if(ip_address.empty()) {
			LOG(Severe) << "Cannot continue execution when my IP address cannot be found";
			throw std::runtime_error("Cannot find ip address");
		}

		std::random_device rd; // Will be used to obtain a seed for the random number engine
		std::seed_seq seed{ rd(), rd(), rd(), rd() };
		std::mt19937 gen(seed); // Standard mersenne_twister_engine seeded with rd()
		multicast_controller_ = std::make_unique<MulticastController<Udp>>(ip_address, *udp_, *multicast_connection_, gen);

	}


	void Server::update()
	{
		multicast_request();

		client_request();

		xp_data_ref_repository.update_values();

		auto changed_values = xp_data_ref_repository.changed_values();


		// send changed dat to subscribed clients
		if (!changed_values.empty()) {
			//LOG() << "Send Changed values to Clients";
			for (auto& [client_id, refs] : connected_clients_to_xplane_refs_) {

				if (std::optional<Connection<Udp>*> maybe_connection = udp_controller_.get_connection(client_id)) {
					//LOG() << "Client " << client_id; 
					std::vector<Data> data;
					for (auto& [id, value] : changed_values) {
						if (refs.datarefs.contains(id)) {
							data.push_back({ id, value });
							LOG() << "Sending id " << id;
						}
					}
					if (!data.empty()) {
						const packet_t packet = DataPacket(client_id, data);
						udp_controller_.add_message(**maybe_connection, packet);
					}
				}
			}
		}


		udp_controller_.server_send();
	}


	void Server::multicast_request()
	{
		const auto state = multicast_controller_->process_client_to_server_request();
		multicast_controller_->process_transmissions();

		if (ControllerState::server_connected == state) {
			// move connection to UDP
			bool has_connected = true;
			while (has_connected) {
				if (auto maybe_client_id = multicast_controller_->find_connected()) {
					auto client_id = maybe_client_id.value();

					if (auto maybe_connection = multicast_controller_->get_connection((client_id))) {
						const auto conn = maybe_connection.value();

						LOG() << "Connected (" << (conn)->get_client_id() << ") RX port " << (conn)->get_rx_port() <<
							" TX " << (conn)->get_tx_port() << " Client IP " << (conn)->get_other_ip_str();
						(conn)->reset_rx_timeout();
						(conn)->timeout_tx();
					}

					if (!connected_clients_to_xplane_refs_.contains(client_id)) {
						connected_clients_to_xplane_refs_[client_id] = RefIds();
					}
					else {
						LOG() << "Server is already connected to client ID: " << client_id;
					}
					auto node = multicast_controller_->remove_connection(client_id);
					udp_controller_.add_connection(std::move(node));
					LOG() << "Server connection moved to UDP Controller.";
				}
				else {
					has_connected = false;
				}
			}
		}

		// Move all timedout connections
		auto has_timedout = true;
		while (has_timedout) {
			if (auto maybe_client_id = udp_controller_.get_timedout_connection_key()) {
				auto client_id = maybe_client_id.value();

				if (auto maybe_connection = udp_controller_.get_connection((client_id))) {
					const auto connection = maybe_connection.value();
					LOG() << "Disconnected (" << connection->get_client_id() << ") RX port " << connection->get_rx_port() <<
						" TX " << connection->get_tx_port() << " Client IP " << connection->get_other_ip_str();

					// Remove Conection and let it die -- there is no way to match client when it restarts.
					if (connected_clients_to_xplane_refs_.contains(client_id)) {
						// Unsubscribe client's datarefs
						auto& refs = connected_clients_to_xplane_refs_.at(client_id);
						for (const auto dataref : refs.datarefs) {
							xp_data_ref_repository.unsubscribe(dataref);
						}
						refs.datarefs.clear();
						for (const auto cmdref : refs.cmdrefs) {
							xp_command_repository.unsubscribe(cmdref);
						}
						refs.cmdrefs.clear();
						connected_clients_to_xplane_refs_.erase(client_id);
					}
					else {
						LOG() << "Connection to client timed out. Tried to remove client from Server, but it is already disconnected Client Id: " << client_id;
					}
					{
						const auto package_id = connection->get_package_id();
						multicast_controller_->clear_packaget_id(package_id);
						udp_controller_.remove_connection(client_id);
					}
				}
				else {
					LOG(Severe) << "Timed out connection should never be absent from the list of connections.";
					assert(false);
				}
			}
			else {
				has_timedout = false;
			}
		}
	}

	void Server::client_request()
	{
		for (auto& [client_id, refs] : connected_clients_to_xplane_refs_) {
			if (auto maybe_packet = udp_controller_.server_receive(client_id)) {
				packet_t packet = maybe_packet.value();

				// Subscribe Dataref
				if (std::holds_alternative<SubscribeDataRefPacket>(packet)) {
					//LOG() << "Packet == Subscribe DataRef";
					SubscribeDataRefPacket subscribe_packet = std::get<SubscribeDataRefPacket>(packet);
					std::vector<int> subscribed_dataref_id;
					for (const SubscribeData& xplane_dataref : subscribe_packet.xplane_dataref) {
						//LOG() << "Subscribe DataRef " << xplane_dataref.dataref_string << " is_annum: " << xplane_dataref.is_boolean_annun;
						if (auto maybe_id = xp_data_ref_repository.subscribe_dataref(xplane_dataref)) {
							auto id = maybe_id.value();
							subscribed_dataref_id.push_back(id);
							refs.datarefs.emplace(id);
						}
					}
					if (!subscribed_dataref_id.empty()) {
						LOG() << "number of subscribed datarefs " << subscribed_dataref_id.size();
						xp_data_ref_repository.update_values_for_requested_ids(subscribed_dataref_id);
						const auto values = xp_data_ref_repository.values_for_requested_ids(subscribed_dataref_id);

						LOG() << "number of confirmation datarefs " << values.size();
						const auto confirmation_data = xp_data_ref_repository.build_dataref_confirmation_data(values);

						const packet_t packet = ConfirmationPacket(client_id, confirmation_data);
						if (std::optional<Connection<Udp>*> maybe_connection = udp_controller_.get_connection(client_id)) {
							udp_controller_.add_message(**maybe_connection, packet);
						}
					}
				}
				// Subscribe Command
				else if (std::holds_alternative<SubscribeCommandRefPacket>(packet)) {
					//LOG() << "Packet == Subscribe CommandRef";
					SubscribeCommandRefPacket subscribe_packet = std::get<SubscribeCommandRefPacket>(packet);
					std::vector<int> subscribed_cmd_id;
					for (const std::string& xplane_command : subscribe_packet.xplane_command) {
						if (auto maybe_id = xp_command_repository.subscribed_command(xplane_command)){
							auto id = maybe_id.value();
							subscribed_cmd_id.push_back(id);
							refs.cmdrefs.emplace(id);
						}
					}
					if (!subscribed_cmd_id.empty()) {
						LOG() << "number of subscribed Command refs " << subscribed_cmd_id.size();

						const auto confirmation_data = xp_command_repository.build_confirmation_data(subscribed_cmd_id);

						const packet_t packet = ConfirmationPacket(client_id, confirmation_data);
						if (std::optional<Connection<Udp>*> maybe_connection = udp_controller_.get_connection(client_id)) {
							udp_controller_.add_message(**maybe_connection, packet);
						}
					}
				}
				// Command
				else if (std::holds_alternative<CommandPacket>(packet)) {
					LOG() << "Packet == Command";
					CommandPacket command_packet = std::get<CommandPacket>(packet);
					for(auto& command : command_packet.command_) {
						auto id = command.id;
						auto cmd_request = command.cmd;
						if(cmd_request == XplaneCommand_enum::once) {
							if (auto maybe_command = xp_command_repository.get_XpCommand(id)) {
								XpCommand xp_command = maybe_command.value();
								LOG() << "Once Command " << xp_command.name() << " id = " << id;
								xp_command.command_once();
							}
						}
						else if(cmd_request == XplaneCommand_enum::begin) {
							if (auto maybe_command = xp_command_repository.get_XpCommand(id)) {
								XpCommand xp_command = maybe_command.value();
								LOG() << "Begin Command " << xp_command.name() << " id = " << id;
								xp_command.command_begin();
							}
						}
						else if (cmd_request == XplaneCommand_enum::end) {
							if (auto maybe_command = xp_command_repository.get_XpCommand(id)) {
								XpCommand xp_command = maybe_command.value();
								LOG() << "End Command " << xp_command.name() << " id = " << id;
								xp_command.command_end();
							}
						}
						else {
							LOG() << "unexpected Xplane Command " << XplaneCommandToString(cmd_request);
						}
					}
					
					
				}
				else if (std::holds_alternative<SetDataRefPacket>(packet)) {
					LOG() << "Packet == SetDataRef";
					SetDataRefPacket set_data_ref_packet = std::get<SetDataRefPacket>(packet);
					LOG() << "Packet client data size: " << set_data_ref_packet.data_.size() << " id " << set_data_ref_packet.data_[0].id;
					std::vector<std::tuple<int, var_t>> new_values;
					for (const auto& set_dataref : set_data_ref_packet.data_) {
						auto new_value = std::make_tuple(set_dataref.id, set_dataref.value);
						new_values.push_back(new_value);

						//if (std::holds_alternative<float>(std::get<1>(new_value))) {
						//	auto float_val = std::get<float>(std::get<1>(new_value));
						//	LOG() << "Setting DataRef id " << std::get<0>(new_value) << " new float value " << float_val;
						//}
						//else if (std::holds_alternative<int>(std::get<1>(new_value))) {
						//	auto int_val = std::get<int>(std::get<1>(new_value));
						//	LOG() << "Setting DataRef id " << std::get<0>(new_value) << " new int value " << int_val;
						//}
						//else if (std::holds_alternative<double>(std::get<1>(new_value))) {
						//	auto double_val = std::get<double>(std::get<1>(new_value));
						//	LOG() << "Setting DataRef id " <<std::get<0>(new_value) << " new int value " << double_val;
						//}
						//else {
						//	LOG() << "Setting DataRef id " << std::get<0>(new_value) << " VECTOR TYPE ";
						//}
					}
					xp_data_ref_repository.set_xplane_dataref_values(new_values);
				}
				else if (std::holds_alternative<HeartbeatPacket>(packet)) {
						//LOG() << "Receive heartbeat RX port " << connection->get_rx_port();
				}
				else {
					LOG() << "Received Unknown packet type";
				}
			}
		}
	}
}