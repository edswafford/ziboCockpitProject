#pragma once

/************************************************************************/
/*	TEMPLATE                                                                      */
/************************************************************************/
/*
#ifdef _WIN64
	// Windows 64
#elif TARGET_OS_MAC
	// Apple OS
#elif __linux__
	// Linux
#elif __unix__
	// Other Unix OS
#else
	// Unidentified OS
#endif

*/

#if !defined(_WIN32)
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <cerrno>


using SOCKET = int;

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

#define VALID_SOCKET(S) ((S) >= 0)
#define CLOSE_SOCKET(S) (close(S))
#define GET_SOCKET_ERRNO() (errno)
#else
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")


#define VALID_SOCKET(S) ((S) != INVALID_SOCKET)
#define CLOSE_SOCKET(S) (closesocket(S))
#define GET_SOCKET_ERRNO() (WSAGetLastError())
#endif

