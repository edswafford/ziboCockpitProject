
#include "udp_interface.hpp"
#include "socket_mock.hpp"


static std::function<int(SOCKET, char*, int, int, sockaddr*, int*)> recvfrom_;



SocketMock::SocketMock()
{
	assert(!recvfrom_);
	recvfrom_ = [this](SOCKET socket, char* buffer, int buffer_size, int flags, sockaddr* from, int* from_size )
	{
		return recvfrom(socket, buffer, buffer_size, flags, from, from_size);
	};

}

SocketMock::~SocketMock()
{
	recvfrom_ = {};
}

int recvfrom(SOCKET socket, char* buffer, int buffer_size, int flags, sockaddr* from, int* from_size )
{
	return recvfrom_(socket, buffer, buffer_size, flags, from, from_size);
}
