//#include "gtest/gtest.h"
//#include <typeinfo>
//#include <optional>
//
//
//#include "../../common/queue.hpp"
//#include "../../plugin/src/connection.hpp"
//
//
////#include "../../plugin/src/server.hpp"
//
//
//using namespace zcockpit;
//using namespace zcockpit::plugin;
//
//class Udp {
//
//};
//
//// Server 
////		Listens for clients on multicast	-- opens UPD multicast service
////		Receives Rx client messages			-- opens UDP receiver service
////		Transmits Tx message to clients		-- opens UDP transmitter service
//
//
//class Server {
//public:
//
//	// receive_multicast listens for incoming UDP Multicast
//	// messages from Clients. A connection is valid once we (Server)
//	// and the Client have RX and TX ports open.
//
//
//	// Listens for UDP Multicast messages from Client
//	// 
//	// It returns an std::optional<Connection> 
//	// If a client has connected it returns the Connection
//	//
//	std::optional<Connection> listen_for_clients() {
//		return  connected_clients_.pop();
//	}
//
//	void add_connection(Connection connection) {
//		connected_clients_.push(std::move(connection));
//	}
//	
//private:
//	common::Queue<Connection> connected_clients_;
//	common::Queue<Connection> potential_clients_;
//};
//
//namespace {
//	//
//	// Tests Server
//	//
//
//
//	TEST(cockpit_server, listen_return_a_connection_if_a_client_has_connected) {
//		Server server;
//		
//		Connection connection{};
//		server.add_connection(std::move(connection));
//
//		auto result = server.listen_for_clients();
//		EXPECT_TRUE(result);
//		EXPECT_TRUE(typeid(Connection) == typeid(*result));
//	}
//
//	TEST(cockpit_server, listen_return_a_nullopt_if_a_client_has_not_connected) {
//		Server server;
//
//		auto connection = server.listen_for_clients();
//		EXPECT_FALSE(connection);
//	}
//
//}