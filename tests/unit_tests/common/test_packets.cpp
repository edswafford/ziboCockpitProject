
#include <gtest/gtest.h>
#include "../../../common/network/udp.hpp"
#include "../../../common/packet.hpp"


using namespace zcockpit::common;

logger LOG("test_common.log");

namespace {
	TEST(TestCommonPacktes, TestEmptyJsonCmd) {
		const auto maybe_packet = Packet::transform_client_request(nullptr, 0);
		EXPECT_FALSE(maybe_packet);
	}

	TEST(TestCommonPacktes, TestStartConnection)
	{

		ConnectPacket start_connection(
			Packet::kSimDataId,
			ClientType::hardware,
			RequestType::connect,
			MulticastRequestType::open_connection,
			1695670,
			2345,
			std::string{"192.168.0.1"},
			737
		);
		ConnectPacket packet = start_connection;

		// Serialize the packet
		nlohmann::json j;
		ConnectPacket::to_json(j, packet);
		auto str = j.dump();

		auto maybe_packet = Packet::transform_client_request(reinterpret_cast<const uint8_t*>(str.c_str()), str.size());

		EXPECT_TRUE(maybe_packet);
		EXPECT_TRUE(std::holds_alternative<ConnectPacket>(maybe_packet.value()));
		ConnectPacket start_request = std::get<ConnectPacket>(maybe_packet.value());
		EXPECT_EQ(start_request.client_type, ClientType::hardware);
		EXPECT_EQ(start_request.request, RequestType::connect);
		EXPECT_EQ(start_request.multicast_request, MulticastRequestType::open_connection);
		EXPECT_EQ(start_request.client_id, 1695670);
		EXPECT_EQ(start_request.package_id, 2345);
		EXPECT_EQ(start_request.ip_address, "192.168.0.1");
		EXPECT_EQ(start_request.port, 737);
	}
}