#include "../mocks/udp_mock.hpp"
#include <chrono>
#include <optional>
#include <typeinfo>
#include "gmock/gmock.h"
#include "gtest/gtest-spi.h"
#include "gtest/gtest.h"


#include "network/connection.hpp"
#include "network/multicast_controller.hpp"
#include "network/udp.hpp"

using namespace std::chrono_literals;
using namespace zcockpit::common::network;
using ::testing::AtLeast;
using ::testing::_;
using ::testing::Return;

// random number generator
std::random_device rd; // Will be used to obtain a seed for the random number engine
std::seed_seq seed{rd(), rd(), rd(), rd()};
static std::mt19937 gen(seed); // Standard mersenne_twister_engine seeded with rd()

clientID client_id = 0;
packageID package_id = 456;

void update_byte_buffer(EndPoint& endpoint)
{

	zcockpit::common::ConnectPacket packet(zcockpit::common::Packet::kSimDataId, zcockpit::common::ClientType::hardware, zcockpit::common::RequestType::connect,
	                                       zcockpit::common::MulticastRequestType::open_connection,
	                                       client_id, package_id, "192.168.0.1", 0
	                                      );

	nlohmann::json j;
	zcockpit::common::ConnectPacket::to_json(j, packet);
	auto str = j.dump();

	const auto length = str.size();
	endpoint.message_size = length;
	memcpy(endpoint.byte_buffer.data(), str.c_str(), length);
}

void set_end_point_valid(EndPoint& endpoint, const bool multicast, const bool non_blocking)
{
	endpoint.socket = 1;
}

class MulticastControllerTest : public ::testing::Test
{
protected:
	UdpMock udp_mock{};
	Connection<UdpMock> connection{udp_mock, 0, 0};

	uint32_t group_ip{0};


	std::string my_ip{"192.168.0.100"};

	void SetUp() override
	{
		client_id = 0;
		package_id = 456;

		EXPECT_CALL(udp_mock, string_to_ip(_))
			.Times(AtLeast(1))
			.WillRepeatedly(Return(123456789));

		EXPECT_CALL(udp_mock, create_receiver_imp(_,_,_))
			.WillOnce(Return(22));
		EXPECT_CALL(udp_mock, create_transmitter_imp(_,_))
			.WillOnce(Return(23));

		group_ip = udp_mock.string_to_ip(Udp::kMulticastServerGroupIP);

		if(group_ip == 0) {
			std::cout << "Cannot create group ip address ... terminating\n";
			assert(false);
		}
		if(!connection.create_receiver(Udp::kServerMulticastRxPortAddress, true)) {
			std::cout << "Cannot create Receiver Port ... terminating\n";
			assert(false);
		}
		if(!connection.create_transmitter(group_ip, Udp::kServerMulticastTxPortAddress)) {
			std::cout << "Cannot create Transmitter Port ... terminating\n";
			assert(false);
		}
	}
};


using ::testing::Return;
using ::testing::_;
using ::testing::Invoke;
using ::testing::DoAll;

void multicast_wrapper(const MulticastController<UdpMock>& controller)
{
}

using DeathTest = MulticastControllerTest;
TEST_F(DeathTest, requires_local_ip_to_run)
{
	static UdpMock udp_mock{};
	static Connection<UdpMock> connection{udp_mock, 0, 0};
	EXPECT_CALL(udp_mock, get_ip())
		.WillOnce(Return(std::string()));

	EXPECT_DEATH({
	             multicast_wrapper(MulticastController(udp_mock, connection, gen ));
	             }, "");
}

TEST_F(MulticastControllerTest, no_action_when_threre_are_no_client_request)
{
	EXPECT_CALL(udp_mock, get_ip())
		.WillOnce(Return(my_ip));

	MulticastController udp_multicast{udp_mock, connection, gen};

	EXPECT_CALL(udp_mock, create_receiver_imp(_, _, _))
		.WillRepeatedly(DoAll(
		                      Invoke(set_end_point_valid),
		                      Return(24))
		               );

	//
	// Initial state
	//
	auto state = udp_multicast.process_client_to_server_request();
	EXPECT_EQ(state, ControllerState::unknown);


	EXPECT_CALL(udp_mock, recv_from(_))
		.WillRepeatedly(Return(false));
	//
	// No message received
	//
	for(auto i = 0; i < 10; i++) {
		state = udp_multicast.process_client_to_server_request();
		EXPECT_EQ(state, ControllerState::unknown);
	}
}

TEST_F(MulticastControllerTest, should_open_a_connection_when_a_client_request_one)
{
	EXPECT_CALL(udp_mock, get_ip())
		.WillOnce(Return(my_ip));

	MulticastController udp_multicast{udp_mock, connection, gen};


	EXPECT_CALL(udp_mock, create_receiver_imp(_, _, _))
		.WillRepeatedly(DoAll(
		                      Invoke(set_end_point_valid),
		                      Return(24))
		               );

	EXPECT_CALL(udp_mock, create_transmitter_imp(_, _))
		.WillOnce(Return(25));
	//
	// Initial state
	//
	auto state = udp_multicast.process_client_to_server_request();
	EXPECT_EQ(state, ControllerState::unknown);


	EXPECT_CALL(udp_mock, recv_from(_))
		.WillOnce(DoAll(
		                Invoke(update_byte_buffer),
		                Return(true)
		               ));
	//
	// Received Client Connect Request
	//
	state = udp_multicast.process_client_to_server_request();
	EXPECT_EQ(state, ControllerState::server_connected);

}

TEST_F(MulticastControllerTest, should_return_same_connection_when_multiple_open_request)
{
	EXPECT_CALL(udp_mock, get_ip())
		.WillOnce(Return(my_ip));

	MulticastController udp_multicast{ udp_mock, connection, gen };


	EXPECT_CALL(udp_mock, create_receiver_imp(_, _, _))
		.WillRepeatedly(DoAll(
			Invoke(set_end_point_valid),
			Return(24))
		);
	EXPECT_CALL(udp_mock, create_transmitter_imp(_, _))
		.WillOnce(Return(25));

	//
	// Initial state
	//
	auto state = udp_multicast.process_client_to_server_request();
	EXPECT_EQ(state, ControllerState::unknown);

	client_id = 0;
	package_id = 7378;
	EXPECT_CALL(udp_mock, recv_from(_))
		.WillOnce(DoAll(
			Invoke(update_byte_buffer),
			Return(true)
		));
	//
	// Received Client Connect Request
	//
	state = udp_multicast.process_client_to_server_request();
	EXPECT_EQ(state, ControllerState::server_connected);

	if (const auto maybe_client_id = udp_multicast.get_client_id(package_id)) {
		auto connection = udp_multicast.get_connection(*maybe_client_id, package_id);

		EXPECT_CALL(udp_mock, recv_from(_))
			.WillOnce(DoAll(
				Invoke(update_byte_buffer),
				Return(true)
			));

		state = udp_multicast.process_client_to_server_request();
		EXPECT_EQ(state, ControllerState::server_connected);

		if(const auto maybe_client_id_2 = udp_multicast.get_client_id(package_id)) {
			EXPECT_EQ(*maybe_client_id, *maybe_client_id_2);
			EXPECT_EQ(connection, udp_multicast.get_connection(*maybe_client_id_2, package_id));
		}
		else {
			EXPECT_TRUE(false);
		}
	}
	else {
		EXPECT_TRUE(false);
	}
}
