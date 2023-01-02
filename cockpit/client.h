#pragma once
#include <random>
#include <chrono>
#include <tuple>
#include "network/connection.hpp"
#include "network/multicast_controller.hpp"
#include "network/udp_controller.hpp"
#include "xp_data_ref_repository.hpp"
#include "aircraft_model.hpp"
#include "logger.hpp"


extern logger LOG;
namespace zcockpit::cockpit
{

	template<typename ... Ts>
	struct Overload : Ts ... {
		using Ts::operator() ...;
	};
	template<class... Ts> Overload(Ts...) -> Overload<Ts...>;

	using namespace common;
	using namespace std::chrono_literals;



	enum class ClientState
	{
		disconnected,
		connected,
		check_zbo_available,
		zbo_available,
		zbo_subscribed,
		zbo_active,
	};

	//
	// State transitions
	// 1. initial == disconnected
	// 2. connected -> then back to 1. or up to  2.
	// 3. check_zbo_available -> back to 2 or then 4.
	// 4. zbo_available  -> back to 3 or then 5
	// 5. zbo_subscribed
	// 6. zbo_active

	// random number
	static std::uniform_int_distribution<uint32_t> uid(1, UINT32_MAX);

	template <class T>
	class Client
	{

		T& udp_;
		network::packageID package_id{ 0 };
		network::clientID client_id{ 0 };
		ClientState client_state{ ClientState::disconnected };
		int check_zbo_available_timeout{ 0 };
		int zbo_subscribed_timeout{ 0 };
		int expected_number_of_dataref_subscriptions{ 0 };
		int actual_number_of_dataref_subscriptions{ 0 };

		int expected_number_of_cmdref_subscriptions{ 0 };
		int actual_number_of_cmdref_subscriptions{0};


		network::MulticastController<T>& multicast_to_server_controller_;
		network::MulticastController<T>& multicast_to_app_controller_;
		network::UdpController<T>& udp_controller_;
		network::Connection<T>& connection_;

		packet_t connection_packet_;

		////// Websocket
		////WebsocketClientEndpoint client_endpoint;
		////bool websocket_connected = false;

		std::string ip_address;

		std::mt19937& gen_;
		AircraftModel& aircraft_model;
		std::unordered_map<std::string, DataRefParameter> requested_dataref_subscriptions;
		std::unordered_map<std::string, CommandRefParameter> requested_cmdref_subscriptions;

		int ONE_SECOND;
		int FIVE_SECONDS;
		int FIVE_HZ;
		int TEN_HZ;
		int TWENTY_HZ;
		int	five_second_counter = FIVE_SECONDS;
		bool sync_hw_switches{true};

	public:
		std::string get_ip_address() { return ip_address; }
		bool xplaneStatus() const { return client_state != ClientState::disconnected; }
		bool ziboStatus() const { return client_state == ClientState::zbo_active; }

		Client(
			const std::string ip, 
			network::Udp& udp,
			network::MulticastController<T>& multicast_server_controller,
			network::MulticastController<T>& multicast_app_controller,
			network::UdpController<T>& udp_controller,
			network::Connection<T>& connection,
			std::mt19937& gen,
			AircraftModel& ac_model,
			int update_rate
		) :
			ip_address(ip),
			udp_(udp),
			package_id(uid(gen)),
			multicast_to_server_controller_(multicast_server_controller),
			multicast_to_app_controller_(multicast_app_controller),
			udp_controller_(udp_controller),
			connection_(connection),

			gen_(gen),
			aircraft_model(ac_model),

			ONE_SECOND(1000 / update_rate),
			FIVE_SECONDS(ONE_SECOND * 5),
			FIVE_HZ(200 / update_rate),
			TEN_HZ(100 / update_rate),
			TWENTY_HZ(50 / update_rate)

		{

			connection_.set_package_id(package_id);

			if (!ip_address.empty())
			{
				if (!connection_.create_receiver())
				{
					LOG(Severe) << "Cannot create RX port";
					assert(false);
				}

				//
				// Connection Packet
				//
				connection_packet_ = ConnectPacket(client_id, ClientType::hardware,
					MulticastRequestType::open_connection,
					package_id, ip_address, connection.get_rx_port()
				);

				LOG() << "Starting CLIENT.";
			}
			else {
				LOG(Severe) << "Cannot determine local IP address, Terminating ...";
				assert(false);
			}
		}


		void process_conformation_packet(const packet_t& packet)
		{
			if (ClientState::check_zbo_available == client_state)
			{
				check_zbo_available_timeout += 1;
				if (check_zbo_available_timeout > (2 * ONE_SECOND))
				{
					client_state = ClientState::connected;
				}
			}
			else {
				check_zbo_available_timeout = 0;
			}
			if (ClientState::zbo_subscribed == client_state)
			{
				zbo_subscribed_timeout += 1;
				if (zbo_subscribed_timeout > (FIVE_SECONDS))
				{
					// temporary: DON'T TIME OUT!! 
					client_state = ClientState::zbo_available;
				}
			}
			const ConfirmationPacket confirmation_packet = std::get<ConfirmationPacket>(packet);
			if (confirmation_packet.client_id == client_id)
			{
				auto& maybe_confirmations = confirmation_packet.data_;
				//
				// DATAREF CONFIRMATION
				//
				if (std::holds_alternative<std::vector<DataRefConfirmation>>(maybe_confirmations))
				{
					const std::vector<DataRefConfirmation> confirmations = std::get<std::vector<DataRefConfirmation>>(maybe_confirmations);
					LOG() << "Received " << confirmations.size() << " DataRef Confirmationa";
					actual_number_of_dataref_subscriptions += aircraft_model.process_dataref_conformation_packet(confirmations, requested_dataref_subscriptions);

					if (aircraft_model.z738_is_available() && ClientState::check_zbo_available == client_state)
					{
						client_state = ClientState::zbo_available;
					}
				}
				//
				// COMMAND CONFIRMATION
				//
				else if (std::holds_alternative<std::vector<CommandConfirmation>>(maybe_confirmations))
				{
					auto requested_number_cmdrefs = requested_cmdref_subscriptions.size();
					const std::vector<CommandConfirmation> confirmations = std::get<std::vector<CommandConfirmation>>(maybe_confirmations);
					LOG() << "Received " << confirmations.size() << " CommandRef Confirmationd";

					actual_number_of_cmdref_subscriptions += aircraft_model.process_command_conformation_packet(confirmations, requested_cmdref_subscriptions);

				}
				else
				{
					LOG() << "Received Something Else ";
				}
				LOG() << "actual_number_of_dataref_subscriptions = " << actual_number_of_dataref_subscriptions << " remaining requested datarefs " << requested_dataref_subscriptions.size() << " cmdrefs " << requested_cmdref_subscriptions.size();
				if (actual_number_of_dataref_subscriptions > 1 && requested_dataref_subscriptions.empty() && requested_cmdref_subscriptions.empty())
				{
					client_state = ClientState::zbo_active;
				}
				else {
					if((!requested_dataref_subscriptions.empty() && requested_dataref_subscriptions.size() < 5) || (requested_cmdref_subscriptions.size() > 0 && requested_cmdref_subscriptions.size() < 5)) {
						for(auto& requested : requested_dataref_subscriptions) {
							LOG() << "Requested DATA REFS " << requested.first; 
						}
						for(auto& requested : requested_cmdref_subscriptions) {
							LOG() << "Requested COMMAND REFS " << requested.first; 
						}
					}
				}
			} // end confirmation data
		}



		void process_packet(const packet_t& packet)
		{
			// process packet
			if (std::holds_alternative<ConfirmationPacket>(packet)) {
				process_conformation_packet(packet);
			}
			else if (std::holds_alternative<DataPacket>(packet)) {
				const DataPacket data_packet = std::get<DataPacket>(packet);
				if (data_packet.client_id == client_id) {
					aircraft_model.process_data_packet(data_packet);
				}
			}

			//
			// If we are connected to ZBO 737 Get DataRefs
			// We only need to do this once.
			//
			if (ClientState::zbo_available == client_state) {
				client_state = ClientState::zbo_subscribed;
				//
				// Subscribe DataRefs
				//
				requested_dataref_subscriptions = aircraft_model.dataref_subscriptions;
				LOG() << "Requesting " << requested_dataref_subscriptions.size() << " DataRef Subscriptions";

				std::vector<SubscribeData> packet_dataref;
				int message_count = 0;
				expected_number_of_dataref_subscriptions = 1;
				int total_request = 1;
				for (auto& param : requested_dataref_subscriptions) {
					if(message_count >= 40) {
						message_count = 0;
						expected_number_of_dataref_subscriptions += packet_dataref.size();

						packet_t zbo_subscriptions = SubscribeDataRefPacket(client_id, packet_dataref);
						udp_controller_.add_message(connection_, zbo_subscriptions);
						udp_controller_.client_send(connection_);
						packet_dataref.clear();
					}
					packet_dataref.emplace_back(SubscribeData(param.first, param.second.xplane_type, param.second.is_rounded));
					LOG() << "# " << total_request << " " << message_count << " " << param.first;
					message_count += 1;
					total_request += 1;
				}
				LOG() << "COMPLETED SUBSCRIPTION REQUEST " << total_request - 1 << " of " << requested_dataref_subscriptions.size();

				if (!packet_dataref.empty()) {
					expected_number_of_dataref_subscriptions += packet_dataref.size();

					packet_t zbo_subscriptions = SubscribeDataRefPacket(client_id, packet_dataref);
					udp_controller_.add_message(connection_, zbo_subscriptions);
					udp_controller_.client_send(connection_);
					packet_dataref.clear();
				}

				//
				// Subscribe CommandDataRefs
				//
				requested_cmdref_subscriptions = aircraft_model.commandref_subscriptions;
				LOG() << "Requesting " << requested_cmdref_subscriptions.size() << " CommandRef Subscriptions";

				std::vector<std::string> packet_cmdref;
				message_count = 0;
				expected_number_of_cmdref_subscriptions = 0;
				for(auto& param : requested_cmdref_subscriptions) {
					if(message_count >= 40) {
						message_count = 0;
						expected_number_of_cmdref_subscriptions = packet_cmdref.size();

						packet_t zbo_subscriptions = SubscribeCommandRefPacket(client_id, packet_cmdref);
						udp_controller_.add_message(connection_, zbo_subscriptions);
						udp_controller_.client_send(connection_);
						packet_cmdref.clear();
					}
					packet_cmdref.emplace_back(param.first);
					message_count += 1;
				}
				if(!packet_cmdref.empty()) {
					expected_number_of_cmdref_subscriptions = packet_cmdref.size();
					packet_t zbo_subscriptions = SubscribeCommandRefPacket(client_id, packet_cmdref);
					udp_controller_.add_message(connection_, zbo_subscriptions);
					udp_controller_.client_send(connection_);
					packet_cmdref.clear();
				}
			}
		}

		void process_switches(int current_cycle)
		{
			//
			// zbo_active:: Is true when we have all subscription conformations for datarefs and cmdrefs
			if(ClientState::zbo_active == client_state) {
				// Sync XPlane to HW Switches
				if(sync_hw_switches) {
					// open covers
					const std::vector<common::packet_data_t> packet_data = aircraft_model.open_guards();
					if(!packet_data.empty()) {
						if(std::holds_alternative<SetDataRef>(packet_data[0])) {
							std::vector<SetDataRef>  data_ref_packet;
							const SetDataRef set_data_ref = std::get<SetDataRef>(packet_data[0]);
							data_ref_packet.push_back(set_data_ref);
							LOG() << "Open Guards COMMANDS: ";
							packet_t set_dataref_packet = SetDataRefPacket(client_id, data_ref_packet);
							udp_controller_.add_message(connection_, set_dataref_packet);
							udp_controller_.client_send(connection_);
						}
					}		


					//for(const auto& dataref_switch: data_ref_switch_list) {
					//	LOG() << "Processing Switch " << aircraft_model.get_data_ref_string(dataref_switch) << " dataref " << dataref_switch;
					//	auto commands = aircraft_model.process_hw_switch(dataref_switch);
					//	xplane_commands.insert(xplane_commands.end(), commands.begin(), commands.end());
					//}


					sync_hw_switches = false;
				}
				else if(FIVE_SECONDS == five_second_counter) {
					sync_hw_switches = true;
				}
				
				auto packet_data = aircraft_model.update_switch_values();
				if (!packet_data.empty()) {
					std::vector<XplaneCommand> xplane_commands{};
					std::vector<SetDataRef> xplane_datarefs{};

					for(packet_data_t& data : packet_data){
						if(std::holds_alternative<XplaneCommand>(data)) {
							xplane_commands.emplace_back(std::get<XplaneCommand>(data));
						}
						else if(std::holds_alternative<SetDataRef>(data)) {
							xplane_datarefs.emplace_back(std::get<SetDataRef>(data));
						}
					}

					if(!xplane_commands.empty()) {
						LOG() << "COMMANDS: Sync HW switches with Xplane, number of commands = " << xplane_commands.size();
						// send CommandRef package
						packet_t command_packet = CommandPacket(client_id, xplane_commands);
						udp_controller_.add_message(connection_, command_packet);
						udp_controller_.client_send(connection_);
					}
					
					if (!xplane_datarefs.empty()) {
						LOG() << "DATAREFS: Sync HW switches with Xplane, number of DataRefs = " << xplane_commands.size();
						packet_t set_dataref_packet = SetDataRefPacket(client_id, xplane_datarefs);
						udp_controller_.add_message(connection_, set_dataref_packet);
						udp_controller_.client_send(connection_);
					}
				}

			}
		}



		void update(int current_cycle)
		{
			if(five_second_counter >= FIVE_SECONDS)
			{
				five_second_counter = 0;
			}
			else
			{
				five_second_counter++;
			}

			//
			// Send Request
			//

			////if (1 == current_cycle_) {
			////	if (websocket_connected) {
			////		client_endpoint.send("App Client Running");
			////	}
			////	else {
			////		std::string app_ip = multicast_to_app_controller_.process_app_ip_address();
			////		if (!app_ip.empty()) {
			////			const std::string uri = "ws://" + app_ip + ":8080";
			////			websocket_connected = client_endpoint.connect(uri);
			////		}
			////	}
			////}


			if (ClientState::disconnected == client_state) {
				// Only send/receive request when we are not connected

				if (FIVE_HZ == current_cycle) {
					// send Open connection request a reduced rate
					multicast_to_server_controller_.add_message(connection_packet_);
				}

				// When connecting there will be handshake messages that need to go out
				multicast_to_server_controller_.process_transmissions();

				//
				// Receive and Process Messages
				//
				const auto connection_state = multicast_to_server_controller_.process_server_to_client(connection_);

				if (network::ConnectionState::timedout == connection_.get_state()) {
					client_state = ClientState::disconnected;
				}

				//
				// We have connected with server
				//
				if (network::ControllerState::client_connected == connection_state) {
					client_state = ClientState::connected;
					auto id = connection_.get_client_id();
					// Server has given us a client_id
					// update packet
					if (client_id != id) {
						client_id = id;
						ConnectPacket packet = std::get<ConnectPacket>(connection_packet_);
						packet.client_id = client_id;
						connection_packet_ = packet;
					}

					LOG() << "Connected (" << id << ") RX port " << connection_.get_rx_port() << " TX "
						<< connection_.get_tx_port() << " Server IP " << connection_.get_other_ip_str();
					connection_.reset_rx_timeout();
					connection_.timeout_tx();

					LOG() << "Client connection switching to UDP Controller";
				}
			}


			// ******************************************************************************************************************
			//
			// Process Normal UDP
			//
			// ******************************************************************************************************************

			if (ClientState::disconnected != client_state) {
				//
				// Process Incoming Packets
				//
				if (auto maybe_packet = udp_controller_.client_receive(connection_)) {
					packet_t packet = maybe_packet.value();
					process_packet(packet);
				}

				//
				// Update Xplane with hardware changes
				process_switches(current_cycle);


				//
				// Check for Timed out Connections
				//
				auto id = udp_controller_.get_timedout_connection_key();
				if (id) {
					if (connection_.get_client_id() == client_id) {
						LOG() << "Disconnected (" << connection_.get_client_id() << ") RX port " << connection_.get_rx_port()
							<<
							" TX " << connection_.get_tx_port() << " Client IP " << connection_.get_other_ip_str();
					}
					client_state = ClientState::disconnected;
					// Close the Tx port -- When the server restarts it will be different
					connection_.close_transmitter_socket();
					// Clear dataref and commandref subscription IDs
					aircraft_model.clear_ref_id_lists();
					LOG() << "Client connection using Multicast Controller";
				}

				if (ClientState::disconnected != client_state) {
					//
					// When we first connect Request Current Aircraft's Tail Number
					//
					if (ClientState::connected == client_state) {
						client_state = ClientState::check_zbo_available;

						// Subscribe Tail Number
						DataRefParameter tail_number(DataRefName::acf_tailnum, common::XplaneType::type_String, false);
						requested_dataref_subscriptions.emplace(std::pair(aircraft_model.get_data_ref_string(DataRefName::acf_tailnum), tail_number));
						actual_number_of_dataref_subscriptions = 0;

						std::vector<SubscribeData> packet_data;
						for (const auto& param : requested_dataref_subscriptions) {
							packet_data.emplace_back(SubscribeData(param.first, param.second.xplane_type, param.second.is_rounded)); 
						}
						packet_t tail_number_packet = SubscribeDataRefPacket(client_id, packet_data);
						udp_controller_.add_message(connection_, tail_number_packet);
					}


					udp_controller_.client_send(connection_);

				}
			}
		}


	};
}