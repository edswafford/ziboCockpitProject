#pragma once
#include <vector>
#include <string>

#include "gmock/gmock.h"

//#include "udp_interface.hpp"
#include "network/end_point.hpp"
//#include "network/udp.hpp"

namespace zcockpit::common::network
{
	class UdpMock 
	{
	public:

		MOCK_METHOD(bool, recv_from, (EndPoint&), (const, noexcept));

		MOCK_METHOD(void, close_socket, (EndPoint&), (const, noexcept));
		MOCK_METHOD(bool, enable_nonblocking, (uint64_t), (const, noexcept));

		MOCK_METHOD(bool, create_receiver_imp, (EndPoint&, const bool, const bool), (const, noexcept));
		bool create_receiver(EndPoint& end_point, const bool multicast = false, const bool non_blocking = true) noexcept
		{
			return create_receiver_imp(end_point, multicast, non_blocking);
		}

		MOCK_METHOD(bool, create_transmitter_imp, (EndPoint&, const bool), (const, noexcept));
		bool create_transmitter(EndPoint& endpoint, const bool non_blocking = true) noexcept
		{
			return create_transmitter_imp(endpoint, non_blocking);
		}
		MOCK_METHOD(void, send_to, (EndPoint&, const char*, const int), (const, noexcept));
		MOCK_METHOD(bool, would_block, (), (const, noexcept));
		MOCK_METHOD(std::string, get_ip, (), (const, noexcept));
		MOCK_METHOD(uint32_t, string_to_ip, (const std::string&), (const, noexcept));

		MOCK_METHOD(std::string, ip_to_string, (const unsigned long), (const, noexcept));
		MOCK_METHOD(int, receive, (EndPoint&, char*, int), (const));
		MOCK_METHOD(std::string, get_error_message, (), (const, noexcept));

	};
}