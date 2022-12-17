#pragma once
#include <unordered_map>
#include <list>
#include <random>
#include "udp.hpp"
#include "connection.hpp"

namespace zcockpit::common::network
{
	using packageID = uint32_t;
	using clientID = uint64_t;

	template <class T>
	class UdpController
	{
	private:
		std::unordered_map<clientID, std::unique_ptr<Connection<T>>> connections;
		std::list<clientID> timedout_connections;

	public:
		std::optional<clientID> get_timedout_connection_key()
		{
			if(!timedout_connections.empty()) {
				auto key = timedout_connections.front();
				timedout_connections.pop_front();
				return key;
			}
			return {};
		}

		[[nodiscard]] auto get_connection(clientID client_id) const->std::optional<Connection<T>*>
		{
			if(connections.contains(client_id)) {
				return connections.at(client_id).get();
			}
			return {};
		}


		void add_connection(std::unordered_map<clientID, std::unique_ptr<Connection<T>>>::node_type node) noexcept
		{
			connections.insert(std::move(node));
		}

		void remove_connection(clientID client_id) noexcept
		{
			connections.extract(client_id);
		}


		std::optional<packet_t> server_receive(clientID client_id) noexcept
		{
			const std::chrono::time_point<std::chrono::system_clock> now =
					std::chrono::system_clock::now();
			if (connections.contains(client_id)) {
				auto connection = connections.at(client_id).get();

				if (const auto maybe_packet = connection->receive_request(now)) {
					return maybe_packet;
				}
				else {
					if (connection->timedout()) {
						timedout_connections.push_back(connection->get_client_id());
					}
				}
			}
			return {};
		}

		
		[[nodiscard]] std::optional<packet_t> client_receive(Connection<T>& connection) noexcept
		{
			const std::chrono::time_point<std::chrono::system_clock> now =
					std::chrono::system_clock::now();

			auto maybe_packet = connection.receive_request(now);
			if(maybe_packet) {

				if (std::holds_alternative<HeartbeatPacket>(maybe_packet.value())) {
					//LOG() << "Receive heartbeat RX port " << connection.get_rx_port();
				}
			}
			else {
				if (connection.timedout()) {
					timedout_connections.push_back(connection.get_client_id());
				}
			}
			return maybe_packet;
		}


		void server_send() const noexcept
		{
			for(auto& [key, connection] : connections) {
				std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
				connection->send_pending_messages(now);
			}
		}

		void add_message(Connection<T>& connection, packet_t msg) const
		{
			connection.add_message_to_queue(std::move(msg));
		}

		void client_send(Connection<T>& connection) const noexcept
		{
			const std::chrono::time_point<std::chrono::system_clock> now =
					std::chrono::system_clock::now();

			connection.send_pending_messages(now);
		}
	};
}
