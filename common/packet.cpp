#include "packet.hpp"

#include <optional>

#include "../common/logger.hpp"
#include <cereal/types/vector.hpp>
#include <cereal/archives/portable_binary.hpp>


extern logger LOG;

namespace zcockpit::common
{

	std::optional<packet_t> Packet::transform_client_request(const uint8_t* buffer, const int size) noexcept
	{

		if (buffer != nullptr) {

			auto default_packet = DefaultPacket{};
			{
				std::stringstream binary_stream;
				binary_stream.write(reinterpret_cast<const char*>(buffer), sizeof(DefaultPacket));
				cereal::BinaryInputArchive archive(binary_stream);
				default_packet.load(archive);
				if(kSimDataId == default_packet.sim_id) {
					if (RequestType::subscribe_dataref == default_packet.request) {
					LOG() << "Transform Client Request Packet: SubscribeDataRef";

						auto subscribe_dataref = SubscribeDataRefPacket{};
						{
							std::stringstream binary_stream;
							binary_stream.write(reinterpret_cast<const char*>(buffer), size);
							cereal::BinaryInputArchive archive(binary_stream);
							subscribe_dataref.load(archive);
						}
						return subscribe_dataref;

					}
					else if(RequestType::subscribe_cmdref == default_packet.request) {
						LOG() << "Transform Client Request Packet: SubscribeCommandRef";
						auto subscribe_cmdref = SubscribeCommandRefPacket{};
						{
							std::stringstream binary_stream;
							binary_stream.write(reinterpret_cast<const char*>(buffer), size);
							cereal::BinaryInputArchive archive(binary_stream);
							subscribe_cmdref.load(archive);
						}
						return subscribe_cmdref;

					}
					else if (RequestType::confirmation == default_packet.request) {
						LOG() << "Transform Client Request Packet: ConfirmationPacket";
						auto confirmation = ConfirmationPacket{};
						{
							std::stringstream binary_stream;
							binary_stream.write(reinterpret_cast<const char*>(buffer), size);
							cereal::BinaryInputArchive archive(binary_stream);
							confirmation.load(archive);
						}
						return confirmation;
					}
					else if (RequestType::data == default_packet.request) {
						LOG() << "Transform Client Request Packet: Data";

						auto data = DataPacket{};
						{
							std::stringstream binary_stream;
							binary_stream.write(reinterpret_cast<const char*>(buffer), size);
							cereal::BinaryInputArchive archive(binary_stream);
							data.load(archive);
						}
						return data;
					}
					else if(RequestType::set_value == default_packet.request) {
						LOG() << "Transform Client Request Packet: SetDataRef";
						auto set_dataref = SetDataRefPacket{};
						{
							std::stringstream binary_stream;
							binary_stream.write(reinterpret_cast<const char*>(buffer), size);
							cereal::BinaryInputArchive archive(binary_stream);
							set_dataref.load(archive);
						}
						return set_dataref;
					}
					else if (RequestType::command == default_packet.request) {
						LOG() << "Transform Client Request Packet: Command";
						auto command = CommandPacket{};
						{
							std::stringstream binary_stream;
							binary_stream.write(reinterpret_cast<const char*>(buffer), size);
							cereal::BinaryInputArchive archive(binary_stream);
							command.load(archive);
						}
						return command;
					}
					else if (RequestType::disconnect == default_packet.request) {
						LOG() << "ERROR: 'disconnect' is not implemented";
						return {};
					}
					else if (RequestType::unsubscribe == default_packet.request) {
						LOG() << "ERROR: 'unsubscribe' is not implemented";
						return {};
					}
					else if (RequestType::get_value == default_packet.request) {
						LOG() << "ERROR: 'get_value' is not implemented";
						return {};
					}

					else if (RequestType::interival == default_packet.request) {
						LOG() << "ERROR: 'interval' is not implemented";
						return {};
					}
					else if (RequestType::heartbeat == default_packet.request) {
						auto heartbeat = HeartbeatPacket{};
						return heartbeat;
					}
					//
					// Multicast
					//
					else if(RequestType::connect == default_packet.request) {
						{
							auto start_connection = ConnectPacket{};
							std::stringstream binary_stream;
							binary_stream.write(reinterpret_cast<const char*>(buffer), size);
							cereal::BinaryInputArchive archive(binary_stream);
							start_connection.load(archive);
							return start_connection;
						}
					}
					else {
						LOG() << "ERROR: Unknown Request";
						return {};
					}
				}

			}

		}
		return {};
	}

	//void ConnectPacket::to_json(nlohmann::json& j, const ConnectPacket& p)
	//{
	//	j = nlohmann::json{
	//		{"sim_id",p.sim_id },
	//		{"client_type", p.client_type},
	//		{"request", p.request},
	//		{"multicast_request", p.multicast_request},
	//		{"client_id", p.client_id},
	//		{"package_id", p.package_id},
	//		{"ip_address", p.ip_address},
	//		{"port", p.port}
	//	};
	//}

	//void ConnectPacket::from_json(const nlohmann::json& j, ConnectPacket& p)
	//{
	//	try {
	//		j.at("sim_id").get_to(p.sim_id);
	//		j.at("client_type").get_to(p.client_type);
	//		j.at("request").get_to(p.request);
	//		j.at("multicast_request").get_to(p.multicast_request);
	//		j.at("client_id").get_to(p.client_id);
	//		j.at("package_id").get_to(p.package_id);
	//		j.at("ip_address").get_to(p.ip_address);
	//		j.at("port").get_to(p.port);
	//	}
	//	catch (...) {
	//		std::cout << "json error";
	//	}
	//}
}

