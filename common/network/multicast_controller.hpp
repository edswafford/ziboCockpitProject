#pragma once
#include <cassert>
#include <unordered_map>
#include <queue>
#include <random>
#include <chrono>
#include "udp.hpp"
#include "connection.hpp"

extern logger LOG;

namespace zcockpit::common::network
{
	using packageID = uint32_t;
	using clientID = uint64_t;


	enum class ControllerState
	{
		unknown,
		server_connected,
		client_connected,
		server_and_client_connected
	};

	// random number generator
	static std::uniform_int_distribution<uint32_t> make_uid(1, UINT32_MAX);


	template <class T>
	class MulticastController
	{
	private:
		T& udp_;
		Connection<T>& multicast_connection_;
		std::string local_ip_address_;
		std::unordered_map<packageID, clientID> package_id_to_client_id;
		std::unordered_map<clientID, std::unique_ptr<Connection<T>>> connections_;
		std::mt19937& gen_;

	public:
		MulticastController(const std::string ip_address, T& udp, Connection<T>& connection, std::mt19937& gen);

		[[nodiscard]] ControllerState process_client_to_server_request() noexcept;
		[[nodiscard]] std::string process_app_ip_address() noexcept;
		[[nodiscard]] ControllerState process_server_to_client(Connection<T>& connection) noexcept;

		std::optional<clientID> find_connected();

		[[nodiscard]] std::optional<clientID> get_client_id(const packageID package_id) const;

		[[nodiscard]] std::optional<Connection<T>*> get_connection(clientID client_id, const packageID package_id);

		[[nodiscard]] auto get_connection(clientID client_id) const->std::optional<Connection<T>*>;

		void process_transmissions() const noexcept { multicast_connection_.send_pending_messages(); }
		void add_message(packet_t msg) const { multicast_connection_.add_message_to_queue(std::move(msg)); }
		auto remove_connection(clientID client_id) { return connections_.extract(client_id); }
		void add_connection(std::unordered_map<clientID, std::unique_ptr<Connection<T>>>::node_type node) noexcept { connections_.insert(std::move(node)); }

		void clear_packaget_id(packageID id) { package_id_to_client_id.erase(id); }
	private:
		[[nodiscard]] std::optional<Connection<T>*> create_connection(const packageID package_id);


		[[nodiscard]] std::optional<ControllerState> respond_to_open_connection(const ConnectPacket& packet) noexcept;

		[[nodiscard]] std::optional<ControllerState> respond_to_client_connected(const ConnectPacket& packet) noexcept;


		[[nodiscard]] std::optional<ControllerState> respond_to_server_connected(Connection<T>& connection, const ConnectPacket& packet) noexcept;
	};


	/**
	 * \brief 
	 * \param udp 
	 * \param connection 
	 * \param gen 
	 */
	template <class T>
	MulticastController<
		T>::MulticastController(const std::string ip_address, T& udp, Connection<T>& connection, std::mt19937& gen) : local_ip_address_(ip_address), udp_(udp),multicast_connection_(connection), gen_(gen)
	{
	}

	/**
	 * \brief 
	 * \return 
	 */
	template <class T>
	ControllerState MulticastController<T>::process_client_to_server_request() noexcept
	{
		std::optional<ControllerState> state = ControllerState::unknown;

		const auto maybe_packet = multicast_connection_.receive_request();

		if(maybe_packet && std::holds_alternative<ConnectPacket>(maybe_packet.value())) {
			const ConnectPacket packet = std::get<ConnectPacket>(maybe_packet.value());

			// Client Request -- Server responds
			if(packet.multicast_request == MulticastRequestType::open_connection) {
				if((state = respond_to_open_connection(packet))) {
					LOG() << "Server responding to client request to connect";
				}
			}

			if(multicast_connection_.timedout()) {
				LOG() << "Multicast connection timed out";
			}
		}
		return *state;
	}

	template <class T>
	std::string MulticastController<T>::process_app_ip_address() noexcept
	{
		std::string ip;
		const auto maybe_packet = multicast_connection_.receive_request();
		if(maybe_packet && std::holds_alternative<ConnectPacket>(maybe_packet.value())) {
			const ConnectPacket packet = std::get<ConnectPacket>(maybe_packet.value());
			ip = packet.ip_address;
		}
		return ip;
	}

	template <class T>
	ControllerState MulticastController<T>::process_server_to_client(Connection<T>& connection) noexcept
	{
		std::optional<ControllerState> state = ControllerState::unknown;

		const auto maybe_packet = multicast_connection_.receive_request();

		if(maybe_packet && std::holds_alternative<ConnectPacket>(maybe_packet.value())) {
			const ConnectPacket packet = std::get<ConnectPacket>(maybe_packet.value());

			// From server to Client
			if(packet.multicast_request == MulticastRequestType::server_completed_connection) {
				state = respond_to_server_connected(connection, packet);
			}

			if(multicast_connection_.timedout()) {
				LOG() << "Multicast connection timed out";
			}
		}
		return *state;
	}

	/**
	 * \brief 
	 * \return 
	 */
	template <class T>
	std::optional<clientID> MulticastController<T>::find_connected()
	{
		for(auto& [key, value] : connections_) {
			if(value->is_connected()) {
				return key;
			}
		}
		return {};
	}

	/**
	 * \brief 
	 * \param client_id 
	 * \param package_id 
	 * \return 
	 */
	template <class T>
	std::optional<Connection<T>*> MulticastController<T>::get_connection(clientID client_id, const packageID package_id)
	{
		// The client_id starts out as zero, the server generates an unique ID for the Client.
		// So we need to check to verify that the ID has not already been generated.
		if(0 == client_id) {
			if(package_id_to_client_id.contains(package_id)) {
				client_id = package_id_to_client_id.at(package_id);
			}
		}
		if(client_id != 0) {
			// We have an ID so check if we already have a connection
			if(connections_.contains(client_id)) {
				return connections_.at(client_id).get();
			}
		}
		return {};
	}

	template <class T>
	auto MulticastController<T>::get_connection(clientID client_id) const->std::optional<Connection<T>*>
	{
		if(connections_.contains(client_id)) {
			return connections_.at(client_id).get();
		}
		return {};
	}

	template <class T>
	std::optional<Connection<T>*> MulticastController<T>::create_connection(const packageID package_id)
	{
		// Generate unique ID for Client
		uint64_t client_id = 0;
		while(client_id == 0) {
			const uint64_t uid = static_cast<uint64_t>(make_uid(gen_)) << 32;
			client_id = uid | package_id;

			// extra check to make sure client_id is not in use
			if(connections_.contains(client_id)) {
				client_id = 0;
			}
		}

		// Create a new Connection
		auto connection = std::make_unique<Connection<T>>(udp_, client_id, package_id);
		if(connection->create_receiver()) {
			package_id_to_client_id[package_id] = client_id;
			connections_[client_id] = std::move(connection);
			return connections_.at(client_id).get();
		}
		return {};
	}


	/**
	 * \brief 
	 * \param packet 
	 * \return 
	 */
	template <class T>
	std::optional<ControllerState> MulticastController<T>::respond_to_open_connection(
		const ConnectPacket& packet) noexcept
	{
		auto state = ControllerState::unknown;
		auto maybe_connection = get_connection(packet.client_id, packet.package_id);

		if(!maybe_connection) {
			//
			// verify this request is not a chain of open request from a current client
			//
			if (!package_id_to_client_id.contains(packet.package_id)) {
				// Create a new Connection
				const auto ip = udp_.string_to_ip(packet.ip_address);
				maybe_connection = create_connection(packet.package_id);
				if (maybe_connection) {
					const auto connection = maybe_connection.value();
					if ((*maybe_connection)->create_transmitter(ip, packet.port)) {
						state = ControllerState::server_connected;
					}
				}
			}
		}
		else {
			state = ControllerState::server_connected;
		}
		// create response
		if(ControllerState::server_connected == state) {
			const auto connection = maybe_connection.value();
			packet_t response_packet = ConnectPacket(
				connection->get_client_id(),
				ClientType::hardware,
				MulticastRequestType::server_completed_connection,
				packet.package_id,
				local_ip_address_,
				connection->get_rx_port());

			// Put this message on the Multicast queue
			multicast_connection_.add_message_to_queue(std::move(response_packet));
			LOG() << "Server received Client open request, Server Connected.  Sending server_completed_connection.";
		}
		return state;
	}


	//
	// Client
	//
	template <class T>
	std::optional<ControllerState> MulticastController<T>::respond_to_server_connected(
		Connection<T>& connection,
		const ConnectPacket& packet) noexcept
	{
		auto state = ControllerState::unknown;
		const unsigned long ip = udp_.string_to_ip(packet.ip_address);

		if(connection.get_client_id() != packet.client_id) {
			connection.set_client_id(packet.client_id);
			connection.set_package_id(packet.package_id);
		}
		if(connection.get_other_ip() != ip) {
			connection.set_other_ip(ip);
		}
		if(!connection.transmitter_valid()) {
			if(connection.create_transmitter(ip, packet.port)) {
				// TX is valid
				state = ControllerState::client_connected;
			}
		}
		else {
			// Server may have reset -- check my TX port == Server's RX port
			if(connection.get_tx_port() != packet.port) {
				connection.set_tx_port(packet.port);
			}
			if(connection.is_connected()) {
				state = ControllerState::client_connected;
			}
		}


		if(ControllerState::client_connected == state) {
			LOG() << "Client received Server connected response.  Client connected.";
		}
		return state;;
	}


	template <class T>
	std::optional<ControllerState> MulticastController<T>::respond_to_client_connected(
		const ConnectPacket& packet) noexcept
	{
		if(const auto maybe_connection = get_connection(packet.client_id, packet.package_id)) {
			if(maybe_connection) {
				const auto connection = maybe_connection.value();
				LOG() << "Server received Client connected response. Server and Client both connected.";
				return ControllerState::server_and_client_connected;
			}
		}
		return ControllerState::unknown;
	}


	template <class T>
	std::optional<clientID> MulticastController<T>::get_client_id(const packageID package_id) const
	{
		if(package_id_to_client_id.contains(package_id)) {
			return package_id_to_client_id.at(package_id);
		}
		return {};
	}
}
