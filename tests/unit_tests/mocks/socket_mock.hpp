#pragma once

#include "gmock/gmock.h"

#include "udp_interface.hpp"

struct SocketMock
{
	SocketMock();
	~SocketMock();


	MOCK_METHOD(int, recvfrom, (SOCKET, char*, int, int, sockaddr*, int*), (const));


};