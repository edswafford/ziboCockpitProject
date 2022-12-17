#pragma once
#include <unordered_map>
#include <set>

#include "xp_data_ref_repository.hpp"
#include "../common/xp_command_repository.hpp"
#include "network/udp.hpp"
#include "logger.hpp"
#include "network/multicast_controller.hpp"
#include "network/udp_controller.hpp"


extern logger LOG;

namespace zcockpit::plugin
{
	using namespace zcockpit::common;
	using namespace zcockpit::common::network;

	struct RefIds
	{
		std::set<int> datarefs{};
		std::set<int> cmdrefs{};
	};

	class Server
	{
	public:
		Server(std::unique_ptr<Udp> udp_ptr);
		void client_request();
		void multicast_request();

		void update();




	private:
		std::unique_ptr<Udp> udp_{nullptr};
		uint32_t group_ip_{ 0 };
		std::unique_ptr<Connection<Udp>> multicast_connection_{nullptr};
		std::unique_ptr<MulticastController<Udp>> multicast_controller_{nullptr};
		UdpController<Udp> udp_controller_;
		XpDataRefRepository xp_data_ref_repository;
		XpCommandRepository xp_command_repository;
		
		std::unordered_map<clientID, RefIds> connected_clients_to_xplane_refs_;
	};

}
