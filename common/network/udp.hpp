#pragma once
#include <concepts>
#include <optional>
#include <string>

#include "end_point.hpp"


namespace zcockpit::common::network
{

		// UDP Concept
		template<class T>
		concept UdpConcept = requires (T udp, EndPoint endpoint, std::string message) {
			{ udp.recv_from(endpoint) } noexcept -> std::same_as<std::optional<std::string>>;
			{ udp.send_to(endpoint, message)} noexcept -> std::same_as<int>;
			{ udp.get_error_message() } noexcept->std::same_as<std::string>;
		};



		//// Udp Multicast
		//template<UdpConcept Udp, ConnectConcept Connect, ConnectControllerCencept<Connect> Controller>
		//class UdpMulticast {
		//public:

		//	explicit UdpMulticast(Udp& udp, Controller& controller) : udp_(udp), controller_(controller)
		//	{
		//	}

		//	//
		//	// Process clients
		//	void process_client()
		//	{
		//		EndPoint endpoint{};
		//		auto maybe_msg_str = udp_.recv_from(endpoint);
		//		if (maybe_msg_str)
		//		{
		//			controller_.process(*maybe_msg_str);
		//		}
		//	}

		//private:
		//	Udp& udp_;
		//	Controller& controller_;

		//};

		//
		// UDP
		//
		class Udp {
		public:
			static constexpr unsigned long kAnyAddress = 0ul;

			static constexpr unsigned short kAppMulticastTxPortAddress = 31337;
			static constexpr unsigned short kAppMulticastRxPortAddress = 31338;
			static constexpr unsigned short kServerMulticastRxPortAddress = 31339;
			static constexpr unsigned short kServerMulticastTxPortAddress = 31340;
			inline static auto kMulticastAppGroupIP = std::string{ "226.7.3.7" };
			inline static auto kMulticastServerGroupIP = std::string{ "226.7.3.8" };
			Udp();
			~Udp();
			Udp(const Udp&) = delete;
			Udp(Udp&&) = delete;
			Udp& operator=(const Udp&) = delete;
			Udp& operator=(Udp&&) = delete;


#if !defined(_WIN32)
			 void close_socket(EndPoint& end_point) noexcept;
			[[nodiscard]] bool enable_nonblocking(int socket) noexcept;
			#else
			 void close_socket(EndPoint& end_point) noexcept;
			 [[nodiscard]] bool enable_nonblocking(uint64_t socket)noexcept;
#endif
			
			 bool create_receiver(EndPoint& end_point, const bool multicast = false, const bool non_blocking = true) noexcept;
			 bool create_transmitter(EndPoint& end_point, const bool non_blocking = true) noexcept;

			
			 [[nodiscard]] bool recv_from(EndPoint& endpoint) noexcept;
			 [[nodiscard]] int receive(EndPoint& endpoint, char* data, int data_size) noexcept;
			 void send_to(const EndPoint& endpoint, const char* buffer, const int size) noexcept;

			[[nodiscard]] bool would_block() noexcept;
			 [[nodiscard]] std::string get_ip() noexcept;
			 [[nodiscard]] uint32_t string_to_ip(const std::string& ip_str) noexcept;
			 [[nodiscard]] static std::string ip_to_string(const unsigned long ip) noexcept;
			 [[nodiscard]] static std::string get_error_message() noexcept;

		private:
#if !defined(_WIN32)
			void close_socket(int socket) noexcept;
#else
			void close_socket(uint64_t socket) noexcept;
#endif

			std::string local_ip_address_ = "";
		};
}
