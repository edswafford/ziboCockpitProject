// multicast_2_send.cpp : Defines the entry point for the application.
//
#include <string>
#include <iostream>
#include <thread>

#include "../../common/udp_interface.hpp"
#include "../../common/network/udp.hpp"
#include "../../common/network/multicast_controller.hpp"
#include "../../common/network/udp_controller.hpp"

#include "logger.hpp"


#ifndef _WIN32
#include <termios.h>
#include <sys/ioctl.h>

int _kbhit() {
    static const int STDIN = 0;
    static bool initialized = false;

    if (!initialized) {
        // Use termios to turn off line buffering
        termios term;
        tcgetattr(STDIN, &term);
        term.c_lflag &= ~ICANON;
        tcsetattr(STDIN, TCSANOW, &term);
        initialized = true;
    }

    int bytesWaiting;
    ioctl(STDIN, FIONREAD, &bytesWaiting);
    return bytesWaiting;
}
#else
#include <conio.h>
#endif

#include <cassert>
#include "../plugin/server.hpp"

logger LOG("integration_recv_test.log");

using namespace std::chrono_literals;
using namespace  zcockpit::common::network;
using namespace  zcockpit::plugin;

int main()
{

	Server server(std::make_unique<Udp>());



	char key = ' ';
	while(key != 'q') {
		if(_kbhit()) {
			key = getchar();
			std::cout << "\nterminating ...\n";
			break;
		}

		server.update();


		auto start = std::chrono::high_resolution_clock::now();
		std::this_thread::sleep_for(50ms);
	}

	return 0;
}
