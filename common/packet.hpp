#pragma once
#include <optional>
#include <variant>
#include <vector>
#include <string>
#include <tuple>

#include <cereal/types/unordered_map.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/variant.hpp>
#include <cereal/types/tuple.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/binary.hpp>


#include "shared_types.hpp"
#include "xp_data_ref.hpp"
#include "logger.hpp"
#include "network/end_point.hpp"
#include "xp_data_ref.hpp"


namespace zcockpit::common
{
	enum class ClientType : uint8_t
	{
		unknown = 0,
		hardware,
		du_displays,
	};
	constexpr const char* ClientTypeToString(const ClientType ct) noexcept
	{
		switch (ct) {
		case ClientType::unknown: return "Unknown Client";
		case ClientType::hardware: return "Hardware Client, Switches and Annunciators";
		case ClientType::du_displays: return "DU Displays Client";
		}
		return "ClentType to string conversion failed";
	}
	enum class RequestType : uint8_t
	{
		heartbeat = 0,
		connect,
		disconnect,
		subscribe_dataref,
		unsubscribe,
		confirmation,
		data,				//// Update changed values
		get_value,			//// get dataref
		set_value,			//// set dataref
		subscribe_cmdref,
		command,
				//find_command,		// XPLMFindCommand
				//command_once,		//// XPLMCommandOnce(cmdRef),
				//command_begin,		//// XPLMCommandBegin(),
				//command_end,		//// XPLMCommandEnd()
		interival,			//// Used to set flightLoopInterval
	};

	constexpr const char* RequestTypeToString(const RequestType rt) noexcept
	{
		switch(rt) {
		case RequestType::heartbeat: return "heartbeat";
		case RequestType::connect: return "connect";
		case RequestType::disconnect: return "disconnect";
		case RequestType::subscribe_dataref: return "subscribe dataref";
		case RequestType::unsubscribe: return "unsubscribe";
		case RequestType::subscribe_cmdref: return "subscribe commandref";
		case RequestType::confirmation: return "confirmation";
		case RequestType::data: return  "data";
		case RequestType::get_value: return "get value";
		case RequestType::set_value: return "set value";
		case RequestType::command: return "command";

		case RequestType::interival: return "interval";
		}
		return "RequestType to string conversion failed";
	}

	enum class MulticastRequestType : uint8_t
	{
		open_connection = 0,
		server_completed_connection,
		no_op,
	};
	constexpr const char* MulticastRequestTypeToString(const MulticastRequestType mrt) noexcept
	{
		switch (mrt) {
		case MulticastRequestType::open_connection: return "open connection";
		case MulticastRequestType::server_completed_connection: return "server completed connection";
		case MulticastRequestType::no_op: return "no op";
		}
		return "MulticastRequestType to string conversion failed";
	}

	struct DataRefConfirmation;
	struct CommandConfirmation;
	//
	//
	using confirmation_t = std::variant<std::vector<DataRefConfirmation>, std::vector<CommandConfirmation>>;

	struct ConnectPacket;
	struct HeartbeatPacket;
	struct SetDataRefPacket;
	struct SubscribeDataRefPacket;
	struct ConfirmationPacket;
	struct DataPacket;
	struct CommandPacket;
	struct SubscribeCommandRefPacket;
	//
	//  
	//
	using packet_t = std::variant<ConnectPacket, HeartbeatPacket, SetDataRefPacket, SubscribeDataRefPacket, ConfirmationPacket, DataPacket, CommandPacket, SubscribeCommandRefPacket>;

	/**
	 * \brief	Packet
	 */
	class Packet
	{
	public:
		static constexpr uint16_t kSimDataId = 0X0A5A5;

		static std::optional<packet_t> transform_client_request(const uint8_t* buffer, const int size) noexcept;
	};

	//
	// Default Request
	//
	struct DefaultPacket
	{
		uint16_t sim_id{Packet::kSimDataId};
		RequestType request{RequestType::heartbeat};

		template <class Archive>
		void save(Archive& ar) const
		{
			ar(sim_id, request);
		}

		template <class Archive>
		void load(Archive& ar)
		{
			ar(sim_id, request);
		}
	};
	static constexpr int kDafaultRequestSize{sizeof(DefaultPacket)};

	struct HeartbeatPacket
	{
		uint16_t sim_id{ Packet::kSimDataId };
		RequestType heartbeat{ RequestType::heartbeat };
		template <class Archive>
		void save(Archive& ar) const
		{
			ar(sim_id, heartbeat);
		}

		template <class Archive>
		void load(Archive& ar)
		{
			ar(sim_id, heartbeat);
		}
	};


	//
	// Connect
	struct ConnectPacket
	{
		uint16_t sim_id{Packet::kSimDataId};
		ClientType client_type{ ClientType::unknown };
		RequestType request{RequestType::connect};
		MulticastRequestType multicast_request{ MulticastRequestType::no_op };
		uint64_t client_id{};
		uint32_t package_id{};
		std::string ip_address;
		uint16_t port{};

		ConnectPacket(uint64_t id, ClientType type, MulticastRequestType multicast_request_type, uint32_t pkg_id, std::string ip, uint16_t udp_port ) :
		client_id(id), client_type(type), multicast_request(multicast_request_type), package_id(pkg_id), ip_address(ip), port(udp_port)
		{}
		ConnectPacket() = default;

		template <class Archive>
		void save(Archive& ar) const
		{
			ar(sim_id, client_type, request, multicast_request, client_id, package_id, ip_address, port);
		}

		template <class Archive>
		void load(Archive& ar)
		{
			ar(sim_id, client_type, request, multicast_request, client_id, package_id, ip_address, port);
		}

	};

	//
	// Subscribe Command
	struct SubscribeCommandRefPacket
	{
		uint16_t sim_id{ Packet::kSimDataId };
		RequestType request{ RequestType::subscribe_cmdref };
		uint64_t client_id{ 0 };
		std::vector<std::string> xplane_command;

		SubscribeCommandRefPacket(uint64_t id, std::vector<std::string> command) : client_id(id), xplane_command(std::move(command))
		{
		}

		SubscribeCommandRefPacket() = default;

		template <class Archive>
		void save(Archive& ar) const
		{
			ar(sim_id, request, client_id, xplane_command);
		}

		template <class Archive>
		void load(Archive& ar)
		{
			ar(sim_id, request, client_id, xplane_command);
		}
	};

	//
	// Xplane Command
	struct XplaneCommand
	{
		int id{-1};
		XplaneCommand_enum cmd{XplaneCommand_enum::none};

		template <class Archive>
		void save(Archive& ar) const
		{
			ar(id, cmd);
		}

		template <class Archive>
		void load(Archive& ar)
		{
			ar(id, cmd);
		}

	};

	//
	// Command
	struct CommandPacket
	{
		uint16_t sim_id{ Packet::kSimDataId };
		RequestType request{ RequestType::command };
		uint64_t client_id{ 0 };
		std::vector<XplaneCommand> command_;


		CommandPacket(uint64_t id, std::vector<XplaneCommand> command) : client_id(id), command_(std::move(command))
		{
		}

		CommandPacket() = default;

		template <class Archive>
		void save(Archive& ar) const
		{
			ar(sim_id, request, client_id, command_);
		}

		template <class Archive>
		void load(Archive& ar)
		{
			ar(sim_id, request, client_id, command_);
		}
	};

	
	//
	// Set DataRef
	struct SetDataRef
	{
		int id{-1};
		var_t value;

		template <class Archive>
		void save(Archive& ar) const
		{
			ar(id, value);
		}

		template <class Archive>
		void load(Archive& ar)
		{
			ar(id, value);
		}

	};

	struct SetDataRefPacket
	{
		uint16_t sim_id{Packet::kSimDataId};
		RequestType request{ RequestType::set_value };
		uint64_t client_id{0};
		std::vector<SetDataRef> data_;


		SetDataRefPacket(const uint64_t id, std::vector<SetDataRef> data) : client_id(id), data_(std::move(data)){}
		SetDataRefPacket() = default;
		
		template <class Archive>
		void save(Archive& ar) const
		{
			ar(sim_id, request, client_id, data_);
		}

		template <class Archive>
		void load(Archive& ar)
		{
			ar(sim_id, request, client_id, data_);
		}
	};


	//
	// Subscribed Data
	struct SubscribeData
	{
		std::string dataref_string{""};
		XplaneType requested_type{XplaneType::type_Unknown};
		bool is_rounded{false};

		SubscribeData()=default;
		SubscribeData(std::string name, XplaneType type, bool is_bool): dataref_string(name), requested_type(type), is_rounded(is_bool){}

		template <class Archive>
		void save(Archive& ar) const
		{
			ar(dataref_string, requested_type, is_rounded);
		}

		template <class Archive>
		void load(Archive& ar)
		{
			ar(dataref_string, requested_type, is_rounded);
		}
	};

	//
	// Subscribe DataRef
	struct SubscribeDataRefPacket
	{
		uint16_t sim_id{Packet::kSimDataId};
		RequestType request{ RequestType::subscribe_dataref };
		uint64_t client_id{0};
		std::vector<SubscribeData> xplane_dataref;

		SubscribeDataRefPacket(uint64_t id, std::vector<SubscribeData> data_ref) : client_id(id), xplane_dataref(std::move(data_ref))
		{
		}

		SubscribeDataRefPacket() = default;

		template <class Archive>
		void save(Archive& ar) const
		{
			ar(sim_id, request, client_id, xplane_dataref);
		}

		template <class Archive>
		void load(Archive& ar)
		{
			ar(sim_id, request, client_id, xplane_dataref);
		}
	};

	//
	// Valid Subscrition
	struct DataRefConfirmation
	{
		std::string name;
		int id{-1};
		XplaneType data_type{XplaneType::type_Unknown};
		var_t value;

		template <class Archive>
		void save(Archive& ar) const
		{
			ar(name, id, data_type, value);
		}

		template <class Archive>
		void load(Archive& ar)
		{
			ar(name, id, data_type, value);
		}

	};

	struct CommandConfirmation
	{
		std::string name;
		int id{ -1 };

		template <class Archive>
		void save(Archive& ar) const
		{
			ar(name, id);
		}

		template <class Archive>
		void load(Archive& ar)
		{
			ar(name, id);
		}

	};

	struct ConfirmationPacket
	{
		uint16_t sim_id{ Packet::kSimDataId };
		RequestType request{ RequestType::confirmation };
		uint64_t client_id{ 0 };
		confirmation_t data_;

		ConfirmationPacket(uint64_t id, const confirmation_t data_) : client_id(id), data_(data_){}
		ConfirmationPacket() = default;

		template <class Archive>
		void save(Archive& ar) const
		{
			ar(sim_id, request, client_id, data_);
		}

		template <class Archive>
		void load(Archive& ar)
		{
			ar(sim_id, request, client_id, data_);
		}

	};

	//
	// Data
	struct Data
	{
		int id{ -1 };
		var_t value;

		template <class Archive>
		void save(Archive& ar) const
		{
			ar(id, value);
		}

		template <class Archive>
		void load(Archive& ar)
		{
			ar(id, value);
		}

	};


	struct DataPacket
	{
		uint16_t sim_id{ Packet::kSimDataId };
		RequestType request{ RequestType::data };
		uint64_t client_id{ 0 };

		std::vector<Data> data_;

		DataPacket(uint64_t id, const std::vector<Data> data_) : client_id(id), data_(data_) {}
		DataPacket() = default;

		template <class Archive>
		void save(Archive & ar) const
		{
			ar(sim_id, request, client_id, data_);
		}

		template <class Archive>
		void load(Archive & ar)
		{
			ar(sim_id, request, client_id, data_);
		}
	};

	using packet_data_t = std::variant<XplaneCommand, SetDataRef>;
}
