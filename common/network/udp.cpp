#include "udp.hpp"

#include <cassert>
#include <cstring>
#include <iostream>
#include <vector>

#include "end_point.hpp"
#include "../../common/logger.hpp"
#include "../../common/udp_interface.hpp"

extern logger LOG;

namespace zcockpit::common::network
{
	static socklen_t sock_addr_size{static_cast<int>(sizeof(sockaddr_in))};

#if defined(_WIN32)

	Udp::Udp()
	{
		// Initialize Winsock
		//  The MAKEWORD(2,2) parameter of WSAStartup makes a request for version 2.2 of Winsock on the system
		// and sets the passed version as the highest version of Windows Sockets support that the caller can use.
		WSADATA wsaData{};
		if(const auto result = WSAStartup(MAKEWORD(2, 2), &wsaData); result != 0) {
			LOG(Severe) << "WSAStartup failed error code :" << result;
			WSACleanup();
			throw std::runtime_error("Cannot initialize Winsock ... terminating");
		}
	}

	Udp::~Udp()
	{
		WSACleanup();
	}


	std::string Udp::get_error_message() noexcept
	{
		constexpr int max_message = 512;
		static char message[max_message] = {0};

		FormatMessageA(
		              FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		              0,
		              WSAGetLastError(),
		              0,
		              message,
		              max_message,
		              0);

		std::string msg(message);
		// remove trailing new line
		if(msg.ends_with('\n')) {
			msg.pop_back();
			if(msg.ends_with('\r')) {
				msg.pop_back();
			}
		}
		return msg;
	}

#else

	Udp::Udp() = default;
	Udp::~Udp() = default;

	std::string Udp::get_error_message() noexcept
	{
		return std::string{ strerror(errno) };
	}
#endif


	bool Udp::create_receiver(EndPoint& end_point,
	                          const bool multicast, const bool non_blocking) noexcept
	{
		const SOCKET socket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		if(VALID_SOCKET(socket_)) {
			sockaddr_in receiver_addr{};
			receiver_addr.sin_family = AF_INET;
			receiver_addr.sin_port = htons(end_point.port);
			receiver_addr.sin_addr.s_addr = INADDR_ANY;


			if(bind(socket_, reinterpret_cast<sockaddr*>(&receiver_addr), sizeof(receiver_addr)) != 0) {
				LOG(Severe) << "Error cannot bind port " << end_point.port << ",  socket error " << get_error_message();
				close_socket(socket_);
			}
			else {
				if(non_blocking) {
					// turn on non-blocking
					if(!enable_nonblocking(socket_)) {
						LOG(Severe) << "Error cannot turn on non-blocking port " << end_point.port << ", socket error: " << get_error_message();
						close_socket(socket_);
					}
				}
				if(multicast) {
					struct ip_mreq group{};
					const auto success = inet_pton(AF_INET, Udp::kMulticastServerGroupIP.c_str(), &(group.imr_multiaddr.s_addr));
					assert(success);
					group.imr_interface.s_addr = htonl(INADDR_ANY);


					if(setsockopt(socket_, IPPROTO_IP, IP_ADD_MEMBERSHIP, reinterpret_cast<char*>(&group), sizeof(group)) < 0) {
						LOG(Warning) << "Failed to add multicast group " << kMulticastServerGroupIP;
						close_socket(socket_);
					}
					constexpr char enable = 1;
					if(setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0){
						LOG(Warning) << "Failed to enable port reuse Socket Error: " << get_error_message();
						close_socket(socket_);
					}
				}

				if(0 == end_point.port && VALID_SOCKET(socket_)) {
					// Port was assigned by OS
					sockaddr_in address{};
					socklen_t len = sizeof(address);
					const auto retval = getsockname(socket_, reinterpret_cast<sockaddr*>(&address), &len);
					if(0 != retval) {

						LOG(Severe) << "Udp error cannot get OS assigned port number, error: " << get_error_message();
					}
					else {
						end_point.port = ntohs(address.sin_port);
					}
				}
			}
		}
		else {
			LOG(Severe) << "Error cannot create socket for port " << end_point.port << ", socket error: " << get_error_message();
		}

		end_point.socket = socket_;
		return end_point.is_valid();
	}

	bool Udp::create_transmitter(EndPoint& end_point, bool non_blocking) noexcept
	{
		// Create a socket for broadcasting
		const SOCKET socket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if(VALID_SOCKET(socket_)) {
			if(non_blocking) {
				// turn on non-blocking
				if(!enable_nonblocking(socket_)) {
					LOG(Severe) << "Cannot create socket error code : " << get_error_message();
					close_socket(socket_);
				}
			}
		}
		else {
			LOG(Severe) << "Cannot create socket error code : " << get_error_message();
		}

		end_point.socket = socket_;
		return end_point.is_valid();
	}


	void Udp::close_socket(EndPoint& end_point) noexcept
	{
		if(INVALID_SOCKET != end_point.socket) {
#if defined(_WIN32)
			const auto retval = closesocket(end_point.socket);
#else
			const auto retval = close(end_point.socket);
#endif
			if(0 != retval) {
				LOG(Severe) << "Udp error closing socket: " << get_error_message();
			}
			end_point.socket = INVALID_SOCKET;
		}
	}

	void Udp::close_socket(SOCKET socket) noexcept
	{
		if (INVALID_SOCKET != socket) {
#if defined(_WIN32)
			const auto retval = closesocket(socket);
#else
			const auto retval = close(socket);
#endif
			if (0 != retval) {
				LOG(Severe) << "Udp error closing socket: " << get_error_message();
			}
			socket = INVALID_SOCKET;
		}
	}



	bool Udp::enable_nonblocking(SOCKET socket) noexcept
	{
#if defined(_WIN32)

		// turn on non-blocking
		u_long iMode = 1;
		return ioctlsocket(socket, FIONBIO, &iMode) == 0;

#else

		return fcntl(socket, F_SETFL, fcntl(socket, F_GETFL, 0) | O_NONBLOCK) == 0;

#endif
	}


	bool Udp::would_block() noexcept
	{
#if defined(_WIN32)
		return WSAGetLastError() == WSAEWOULDBLOCK;
#else
		return errno == EWOULDBLOCK;
#endif
	}

	std::string Udp::get_ip() noexcept
	{
		// google-public-dns-a.google.com	8.8.8.8
		// google-public-dns-b.google.com	8.8.4.4
		// Cloudflare						1.1.1.1
		// ns1.telstra.net					139.130.4.5
		const char* well_known_dns_servers[4] = {"8.8.8.8", "8.8.4.4", "1.1.1.1", "139.130.4.5"};

		const char* google_dns_server[] = { "8.8.8.8" };

		constexpr int dns_port = 53;

		if(!local_ip_address_.empty()) {
			return local_ip_address_;
		}
		else {
			for (const auto dns_server : well_known_dns_servers) {


				const SOCKET socket_ = socket(AF_INET, SOCK_DGRAM, 0);
				//Socket could not be created
				if (VALID_SOCKET(socket_)) {

					sockaddr_in serv{};
					serv.sin_family = AF_INET;
					serv.sin_addr.s_addr = string_to_ip(dns_server);
					serv.sin_port = htons(dns_port);

					if (connect(socket_, reinterpret_cast<const sockaddr*>(&serv), sizeof(serv)) < 0) {
						LOG(Severe) << "Error connecting to DNS Server " << google_dns_server << ". Error message: " << get_error_message();
					}

					sockaddr_in name{};
					socklen_t name_size = sizeof(name);
					if (getsockname(socket_, reinterpret_cast<sockaddr*>(&name), &name_size) < 0) {
						LOG(Severe) << "Error getting local IP address " << google_dns_server << ". Error message: " << get_error_message();
					}
					else {
						const auto ip_str = Udp::ip_to_string(name.sin_addr.s_addr);
						if (ip_str.empty()) {
							LOG() << "Error number: " << errno
								<< ". Error message: " << get_error_message() << std::endl;
						}
						else {
							local_ip_address_ = ip_str;
							LOG() << " Using: " << dns_server << " Local IP address is: " << ip_str << std::endl;
						}
					}
					close_socket(socket_);
				}
				else {
					LOG(Severe) << "Cannot create socket error code : " << get_error_message();
				}
				if(!local_ip_address_.empty()) {
					break;
				}
			}
		}
		return local_ip_address_;
	}



	uint32_t Udp::string_to_ip(const std::string& ip_str) noexcept
	{
		uint32_t ip = 0;
		if(inet_pton(AF_INET, ip_str.c_str(), &ip)) {
			return ip;
		}
		LOG(Severe) << "Cannnot convert ip address " << ip_str << " to network order ip";
		return 0;
	}

	std::string Udp::ip_to_string(const unsigned long ip) noexcept
	{
		char str[INET6_ADDRSTRLEN];
		if(inet_ntop(AF_INET, &ip, str, INET6_ADDRSTRLEN)) {
			return std::string{str};
		}
		else {
			LOG(Severe) << "Cannnot convert ip address " << ip << " to string";
			return std::string{};
		}
	}

	bool Udp::recv_from(EndPoint& endpoint) noexcept
	{
		sockaddr_in from_addr{};
		auto message_size = recvfrom(endpoint.socket, reinterpret_cast<char*>(endpoint.byte_buffer.data()), endpoint.byte_buffer_size, 0,
		                             reinterpret_cast<sockaddr*>(&from_addr), &sock_addr_size);

		if(0 == message_size) {
			LOG() << "recvfrom message size == 0  -- TODO-zboCockpit Sender has Closed";
		}
		else if(SOCKET_ERROR == message_size) {
			if(!would_block()) {
				LOG(Severe) << "recv_from failed with sock error : " << get_error_message();
				close_socket(endpoint);
				endpoint.socket = INVALID_SOCKET;
			}
			else {
				message_size = 0;
			}
		}
		if(message_size > 0 && message_size <= endpoint.byte_buffer_size) {
			endpoint.message_size = message_size;
			return true;
		}
		return false;
	}


	int Udp::receive(EndPoint& endpoint, char* data, const int data_size) noexcept
	{
		auto inDataLength = recv(endpoint.socket, static_cast<char*>(data), data_size, 0);
		if(SOCKET_ERROR == inDataLength) {
			if(!would_block()) {
				LOG(Severe) << "receive_request failed with sock error : " << get_error_message();
				close_socket(endpoint);
				endpoint.socket = INVALID_SOCKET;
			}
			else {
				inDataLength = 0;
			}
		}
		return inDataLength;
	}

	void Udp::send_to(const EndPoint& endpoint, const char* buffer, const int size) noexcept
	{
		sockaddr_in to_addr{};

		to_addr.sin_family = AF_INET;
		to_addr.sin_addr.s_addr = endpoint.network_order_ip;
		to_addr.sin_port = htons(endpoint.port);

		const auto number_sent = sendto(endpoint.socket, buffer, size, 0, reinterpret_cast<sockaddr*>(&to_addr), sizeof(to_addr));
		if(SOCKET_ERROR == number_sent) {
			LOG(Severe) << "send_to failed with sock error : " << get_error_message();
		}
		else if(size != number_sent) {
			LOG(Severe) << "send_to failed send " << number_sent << " chars : expected " << size;
		}
	}
}
