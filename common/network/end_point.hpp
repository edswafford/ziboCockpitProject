#pragma once
#include <memory>
#include <vector>

namespace zcockpit::common::network
{
	static constexpr int kBufferSize = 4096;

	struct EndPoint
	{
		EndPoint() = default;

		EndPoint(const bool requires_buffer, const unsigned short port, const unsigned long network_order_ip = 0) : network_order_ip(network_order_ip), port(port)
		{
			if (requires_buffer) {
				byte_buffer.resize(kBufferSize);
				byte_buffer_size = byte_buffer.size();
			}
		}
		unsigned long network_order_ip{0};
		unsigned short port{0};
		std::vector<uint8_t> byte_buffer{};
		int byte_buffer_size{ 0 };
		int message_size{0};

#if !defined(_WIN32)
		int socket{ -1 };
#else
		uint64_t socket{~0ull};
#endif

		[[nodiscard]] bool is_valid() const
		{
#if !defined(_WIN32)
			return -1 != socket;
#else
			return ~0ull != socket;
#endif
		}
	};
}
