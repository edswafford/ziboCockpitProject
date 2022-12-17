//#include "udp_interface.hpp"
//#include "udp_mock.hpp"
//
//namespace zcockpit::common::network
//{
//
//	static std::function<int(EndPoint&)> recv_from_;
//
//	static std::function<void(uint64_t)> close_socket_;
//	static std::function<bool(uint64_t)> enable_nonblocking_;
//	static std::function<bool(EndPoint&, bool, bool)> create_receiver_;
//	static std::function<bool(EndPoint&, bool)> create_transmitter_;
//	static std::function<void(EndPoint&, char*, int)> send_to_;
//	static std::function<bool()> would_block_;
//	static std::function<std::vector<std::string>()> get_ip_;
//	static std::function<uint32_t(std::string&)> string_to_ip_;
//	static std::function<std::string(unsigned long)> ip_to_string_;
//	static std::function<int(EndPoint&, char*, int)> receive_;
//	static std::function<std::string()> get_error_message_;
//
//	UdpMock::UdpMock()
//	{
//		assert(!recv_from_ && !close_socket_ && !enable_nonblocking_ && !create_receiver_ && !create_transmitter_);
//		assert(!send_to_ && !would_block_ && !get_ip_ && !string_to_ip_ && !ip_to_string_ && !receive_ && !get_error_message_);
//
//		recv_from_ = [this](EndPoint& endpoint)
//		{
//			return recv_from(endpoint);
//		};
//		close_socket_ = [this](uint64_t socket)
//		{
//			return close_socket(socket);
//		};
//		enable_nonblocking_ = [this](uint64_t socket)
//		{
//			return enable_nonblocking(socket);
//		};
//		create_receiver_ = [this](EndPoint& endpoint, bool multicast, bool non_blocking)
//		{
//			return create_receiver(endpoint, multicast, non_blocking);
//		};
//		create_transmitter_ = [this](EndPoint& endpoint, bool non_blocking)
//		{
//			return create_transmitter(endpoint, non_blocking);
//		};
//		send_to_ = [this](EndPoint& endpoint, char* buffer, int length)
//		{
//			return send_to(endpoint, buffer, length);
//		};
//		would_block_ = [this]() {
//			return would_block(); };
//		get_ip_ = [this]()
//		{
//			return get_ip();
//		};
//		string_to_ip_ = [this](std::string& ip_str)
//		{
//			return string_to_ip(ip_str);
//		};
//		ip_to_string_ = [this](unsigned long ip)
//		{
//			return ip_to_string(ip);
//		};
//		receive_ = [this](EndPoint& endpoint, char* buffer, int length)
//		{
//			return receive(endpoint, buffer, length);
//		};
//		get_error_message_ = [this]()
//		{
//			return get_error_message();
//		};
//	}
//
//	UdpMock::~UdpMock()
//	{
//		recv_from_ = {};
//		close_socket_ = {};
//		enable_nonblocking_ = {};
//		create_receiver_ = {};
//		create_transmitter_ = {};
//		send_to_ = {};
//		would_block_ = {};
//		get_ip_ = {};
//		string_to_ip_ = {};
//		ip_to_string_ = {};
//		receive_ = {};
//		get_error_message_ = {};
//	}
//
//	bool recv_from(EndPoint& endpoint)
//	{
//		return recv_from_(endpoint);
//	}
//
//	void close_socket(uint64_t socket)
//	{
//		return close_socket_(socket);
//	};
//	bool enable_nonblocking(uint64_t socket) { return enable_nonblocking_(socket); };
//
//	bool create_receiver(EndPoint& endpoint, bool multicast, bool non_blocking)
//	{
//		return create_receiver_(endpoint, multicast, non_blocking);
//	};
//	bool create_transmitter(EndPoint& endpoint, bool non_blocking) { return create_transmitter_(endpoint, non_blocking); };
//	void send_to(EndPoint& endpoint, char* buffer, int length) { return send_to_(endpoint, buffer, length); };
//	bool would_block() { return would_block_(); };
//	std::vector<std::string> get_ip() { return get_ip_(); };
//	uint32_t string_to_ip(std::string& ip_str) { return string_to_ip_(ip_str); };
//	std::string ip_to_string(unsigned long ip) { return ip_to_string_(ip); };
//	int receive(EndPoint& endpoint, char* buffer, int length) { return receive_(endpoint, buffer, length); };
//	std::string get_error_message() { return get_error_message_(); };
//}