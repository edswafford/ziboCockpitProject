#pragma once

#include "gmock/gmock.h"

#include "packet.hpp"

namespace zcockpit::common::network
{
	template<class T>
	class ConnectionMock
	{
	public:
		ConnectionMock(T&){}

		MOCK_METHOD(packet_t, receive_request, (), (const, noexcept));


	};
}