#pragma once
#include <concepts>
#include <chrono>

#include "../queue.hpp"
#include "../packet.hpp"
#include "end_point.hpp"
#include "udp.hpp"

extern logger LOG;

namespace zcockpit::common::network
{

	//// Connect Concept
	//template<class T>
	//concept ConnectConcept = requires (T & t, char* receive_buffer, const int length) {
	//	{ t.receive_request() } noexcept -> std::same_as<void>;
	//};


	//template<class T, class U>
	//concept ConnectControllerCencept = requires (T & t, U & u, std::string & request) {
	//	{t.process(request)}noexcept -> std::same_as<void>;
	//	{ t.add(u) } noexcept -> std::same_as<void>;
	//};


	//template<class T>
	//class ConnectionControler {
	//public:
	//	void process(ConnectRequest_1 connection_request) noexcept
	//	{

	//	}
	//	void add(T t) noexcept {
	//	}
	//};

	using packageID = uint32_t;
	using clientID = uint64_t;

	enum class ConnectionState
	{
		disconnected,
		both_connected,
		timedout,
	};

		using namespace std::chrono_literals;

	//
	// CONNECTION
	//
	template<class T>
	class Connection
	{
	private:
		T& udp_;
		EndPoint receiver_{true, 0, 0,};
		EndPoint transmitter_{};
		clientID client_id_{};
		packageID package_id_{};
		unsigned long other_network_order_ip{ 0 };
		ThreadSafeQueue<packet_t> pending_messages;
		ConnectionState state_{ ConnectionState::disconnected };
		std::chrono::time_point<std::chrono::system_clock> latest_rx_msg{};
		std::chrono::time_point<std::chrono::system_clock> latest_tx_msg{};
		bool timedout_{false};


		std::chrono::seconds timeout{ 10 };

	public:
		const std::chrono::milliseconds kTimeoutInterval = 5000ms;		// 5 seconds
		const std::chrono::milliseconds kHeartbeatInterval = 1000ms;	// 1 Second

		Connection() = delete;

		Connection(T& udp, const clientID client_id, const packageID package_id) : udp_(udp), client_id_(client_id), package_id_(package_id)
		{
			const std::chrono::time_point<std::chrono::system_clock> now =
				std::chrono::system_clock::now();

			latest_rx_msg = now;
			latest_tx_msg = now;
		}



		~Connection()
		{
			if (receiver_.is_valid()) {
				udp_.close_socket(receiver_);
			}
			if (transmitter_.is_valid()) {
				udp_.close_socket(transmitter_);
			}
		}
		Connection(const Connection& other) = delete; // II. copy constructor
		Connection(Connection&& other) noexcept = delete; // III. move constructor
		Connection& operator=(const Connection& other) = delete; // IV. copy assignment
		Connection& operator=(Connection&& other) noexcept = delete; // V. move assignment


		[[nodiscard]] bool timedout() const { return timedout_; }

		[[nodiscard]] bool receiver_valid() const { return receiver_.is_valid(); }


		[[nodiscard]] bool create_receiver(const unsigned short port = 0, bool multicast = false)
		{
			receiver_.port = port;
			auto valid = udp_.create_receiver(receiver_, multicast);
			if(valid && transmitter_.is_valid()) {
				state_ = ConnectionState::both_connected;
			}
			return valid;
		}

		void close_receiver_socket()
		{
			udp_.close_socket(receiver_.socket);
			state_ = ConnectionState::disconnected;
		}

		[[nodiscard]] unsigned short get_rx_port() const { return receiver_.port; }


		[[nodiscard]] bool transmitter_valid() const { return transmitter_.is_valid(); }
		[[nodiscard]] bool create_transmitter(const unsigned long ip, const unsigned short port)
		{
			transmitter_.network_order_ip = ip;
			other_network_order_ip = ip;
			transmitter_.port = port;
			auto valid = udp_.create_transmitter(transmitter_);
			if(valid && receiver_.is_valid()) {
				state_ = ConnectionState::both_connected;
			}
			return valid;
		}

		void close_transmitter_socket()
		{
			udp_.close_socket(transmitter_);
			state_ = ConnectionState::disconnected;
		}

		[[nodiscard]] unsigned short get_tx_port() const { return transmitter_.port; }
		void set_tx_port(unsigned short port) { transmitter_.port = port; }
		void set_tx_ip(const uint32_t ip)
		{
			transmitter_.network_order_ip = ip;
		}

		[[nodiscard]] std::string get_other_ip_str() const
		{
			return udp_.ip_to_string(other_network_order_ip);
		}
		[[nodiscard]] unsigned long get_other_ip() const { return other_network_order_ip; }

		void set_other_ip(unsigned long other_ip)
		{
			other_network_order_ip = other_ip;
		}

		[[nodiscard]] clientID get_client_id() const { return client_id_; }
		void set_client_id(const clientID id) { client_id_ = id; }

		[[nodiscard]] clientID get_package_id() const { return package_id_; }
		void set_package_id(const packageID id) { package_id_ = id; }


		[[nodiscard]] std::optional<packet_t> receive_request() noexcept
		{
			if (receiver_.is_valid()) {
				if (udp_.recv_from(receiver_)) {
					timedout_ = false;
					LOG() << "Received message: size " << receiver_.message_size;
					return Packet::transform_client_request(reinterpret_cast<const uint8_t*>(receiver_.byte_buffer.data()),
						receiver_.message_size);
				}
			}
			else {
				udp_.create_receiver(receiver_, true);
			}

			return{};
		}
		[[nodiscard]] std::optional<packet_t> receive_request(const std::chrono::time_point<std::chrono::system_clock>& now ) noexcept
		{
			if (receiver_.is_valid()) {
				if (udp_.recv_from(receiver_)) {
					latest_rx_msg = now;
					timedout_ = false;
					//LOG() << "Received message with timeout: size " << receiver_.message_size;
					return Packet::transform_client_request(reinterpret_cast<const uint8_t*>(receiver_.byte_buffer.data()),
						receiver_.message_size);
				}
			}
			else {
				udp_.create_receiver(receiver_, true);
			}
			// no message received on this connection
			const auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(now - latest_rx_msg);
			if (elapsed_time > kTimeoutInterval) {
				timedout_ = true;
				state_ = ConnectionState::timedout;
				LOG() << "No message received";
			}

			return{};
		}


		void send_pending_messages() noexcept
		{
			if (transmitter_.is_valid()) {
				while (pending_messages.size() > 0) {
					if (auto maybe_packet = pending_messages.pop()) {
						if (maybe_packet) {
							if (std::holds_alternative<ConnectPacket>(*maybe_packet)) {
								ConnectPacket packet = std::get<ConnectPacket>(*maybe_packet);
								{
									std::stringbuf strbuf;
									{
										std::ostream os(&strbuf);
										cereal::BinaryOutputArchive archive(os);
										packet.save(archive);
									}
									LOG() << "Multicast Sending Packet " << MulticastRequestTypeToString(packet.multicast_request);
									udp_.send_to(transmitter_, strbuf.str().c_str(), static_cast<int>(strbuf.str().size()));
								}

							}

						}
					}
				}
			}
			else {
				udp_.create_transmitter(transmitter_);
			}
		}
		void send_pending_messages(const std::chrono::time_point<std::chrono::system_clock>& now) noexcept
		{
			if (transmitter_.is_valid()) {
				bool message_sent = false;
				while (pending_messages.size() > 0) {
					if (auto maybe_packet = pending_messages.pop()) {
						if (maybe_packet) {
							if (std::holds_alternative<ConnectPacket>(*maybe_packet)) {
								ConnectPacket packet = std::get<ConnectPacket>(*maybe_packet);
								{
									std::stringbuf strbuf;
									{
										std::ostream os(&strbuf);
										cereal::BinaryOutputArchive archive(os);
										packet.save(archive);
									}
									LOG() << "UDP Sending Packet " << MulticastRequestTypeToString(packet.multicast_request);
									message_sent = true;
								}
							}
							else if (std::holds_alternative<SubscribeDataRefPacket>(*maybe_packet)) {
								{
								SubscribeDataRefPacket packet = std::get<SubscribeDataRefPacket>(*maybe_packet);
									std::stringbuf strbuf;
									{
										std::ostream os(&strbuf);
										cereal::BinaryOutputArchive archive(os);
										packet.save(archive);
									}
									LOG() << "UDP Sending Subscribe DataRef Packet, cnt " << packet.xplane_dataref.size() <<  " number of bytes " << strbuf.str().size();
									udp_.send_to(transmitter_, strbuf.str().c_str(), static_cast<int>(strbuf.str().size()));
									message_sent = true;
								}
							}
							else if(std::holds_alternative<SubscribeCommandRefPacket>(*maybe_packet)) {
								SubscribeCommandRefPacket packet = std::get<SubscribeCommandRefPacket>(*maybe_packet);
								std::stringbuf strbuf;
									{
										std::ostream os(&strbuf);
										cereal::BinaryOutputArchive archive(os);
										packet.save(archive);
									}
									LOG() << "UDP Sending Subscribe Command Packet, cnt " << packet.xplane_command.size() <<  " number of bytes " << strbuf.str().size();
									udp_.send_to(transmitter_, strbuf.str().c_str(), static_cast<int>(strbuf.str().size()));
									message_sent = true;
							}
							else if (std::holds_alternative<ConfirmationPacket>(*maybe_packet)) {
								ConfirmationPacket packet = std::get<ConfirmationPacket>(*maybe_packet);
								{
									LOG() << "UDP Sending Confirmation Packet ";
									std::stringbuf strbuf;
									{
										std::ostream os(&strbuf);
										cereal::BinaryOutputArchive archive(os);
										packet.save(archive);
									}
									udp_.send_to(transmitter_, strbuf.str().c_str(), static_cast<int>(strbuf.str().size()));
									message_sent = true;
								}
							}
							else if (std::holds_alternative<DataPacket>(*maybe_packet)) {
								DataPacket packet = std::get<DataPacket>(*maybe_packet);
								{
									//LOG() << "UDP Sending Data Packet ";
									std::stringbuf strbuf;
									{
										std::ostream os(&strbuf);
										cereal::BinaryOutputArchive archive(os);
										packet.save(archive);
									}
									udp_.send_to(transmitter_, strbuf.str().c_str(), static_cast<int>(strbuf.str().size()));
									message_sent = true;
								}
							}
							else if (std::holds_alternative<SetDataRefPacket>(*maybe_packet)) {
								SetDataRefPacket packet = std::get<SetDataRefPacket>(*maybe_packet);
								{
									std::stringbuf strbuf;
									{
										std::ostream os(&strbuf);
										cereal::BinaryOutputArchive archive(os);
										packet.save(archive);
									}
									auto packet_size = static_cast<int>(strbuf.str().size());
									LOG() << "UDP Sending Set Data Packet: size = " << packet_size;
									udp_.send_to(transmitter_, strbuf.str().c_str(), packet_size);
									message_sent = true;
								}
							}
							else if (std::holds_alternative<CommandPacket>(*maybe_packet)) {
								CommandPacket packet = std::get<CommandPacket>(*maybe_packet);
								std::stringbuf strbuf;
								{
									std::ostream os(&strbuf);
									cereal::BinaryOutputArchive archive(os);
									packet.save(archive);
								}
								auto packet_size = static_cast<int>(strbuf.str().size());
								LOG() << "UDP Sending Command Packet: size = " << packet_size;
								udp_.send_to(transmitter_, strbuf.str().c_str(), packet_size);
								message_sent = true;
							}
						}
					}
				}

				if (!message_sent) {
					const auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(now - latest_tx_msg);
					if (elapsed_time > kHeartbeatInterval) {
						auto packet = HeartbeatPacket{};
						std::stringbuf strbuf;
						{
							std::ostream os(&strbuf);
							cereal::BinaryOutputArchive archive(os);
							packet.save(archive);
						}
						udp_.send_to(transmitter_, strbuf.str().c_str(), static_cast<int>(strbuf.str().size()));
						latest_tx_msg = now;
						//LOG() << "Heartbeat sent";
					}
				}
				else {
					latest_tx_msg = now;
				}
			}
			else {
				udp_.create_transmitter(transmitter_);
			}
		}

		void add_message_to_queue(packet_t&& packet)
		{
			pending_messages.push(std::move(packet));
		}

		[[nodiscard]] bool is_timedout() const noexcept { return ConnectionState::timedout == state_; }

		[[nodiscard]] bool is_connected() const noexcept { return ConnectionState::both_connected == state_; }

//		void set_state(ConnectionState state) { state_ = state; }
		[[nodiscard]] ConnectionState get_state() const { return state_; }


		void reset_rx_timeout()
		{
			const std::chrono::time_point<std::chrono::system_clock> now =
				std::chrono::system_clock::now();
			latest_rx_msg = now;
			timedout_ = false;
		}
		void timeout_tx()
		{
			const std::chrono::time_point<std::chrono::system_clock> now =
				std::chrono::system_clock::now();
			latest_tx_msg = now - kTimeoutInterval;
		}
	};
}
