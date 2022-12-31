#pragma once
#include <vector>
#include <variant>

namespace zcockpit::common
{
	static constexpr int HZ_40 = 25;
	static constexpr int HZ_20 = 50; // 50ms
	static constexpr int HZ_10 = 100; // 100ms
	static constexpr int HZ_5 = 200; // 200ms
	static constexpr int HZ_1 = 1000; // 1000ms

	// Initialize Update Rate
	static constexpr int UPDATE_RATE = HZ_20;
	static constexpr int ONE_SECOND = 1000 / UPDATE_RATE;
	static constexpr int FIVE_SECONDS = ONE_SECOND * 5;
	static constexpr int FIVE_HZ = 200 / UPDATE_RATE;
	static constexpr int TEN_HZ = 100 / UPDATE_RATE;
	static constexpr int TWENTY_HZ = 50 / UPDATE_RATE;

	/**
	 *
	 * TOGGLE
	 * Sends Command Once
	 * XPlane will leave the switch in the commanded position (ON/OFF)
	 *
	 * PUSHBUTTON
	 * Sends Command Once
	 * XPlane will return the button to the released position 
	 *
	 * SPRING LOADED
	 * Send Command Begin to hold the commanded position
	 * When HW returns to zero Sends Command End
	 * For these XPlane switches Command Once does not work
	 *
	 * ROTARY
	 * Sends Command Once for each position change
	 * XPlane only has two command for this switch (UP/DN)
	 * TODO We need to sync the hw with xplane because the does get out of sync
	 *
	 * MULTIPOSITION
	 * Send Command once for each position
	 * Xplane has command for each position
	 * Note: This switch can also get out of sync
	 */
	enum class SwitchType : unsigned {
		unused,
		toggle,
		pushbutton,
		spring_loaded,
		rotary,				// xplane only supports up or down
		multiposition,		// xplane supports all positions
		other
	};

	enum class XplaneType : unsigned {
		// Data of a type the current XPLM doesn't do. 
		type_Unknown = 0,

		// A single 4-byte integer, native endian.
		type_Int = 1,

		// A single 4-byte float, native endian.
		type_Float = 2,

		// A single 8-byte double, native endian.
		type_Double = 4,

		// An array of 4-byte floats, native endian.
		type_FloatArray = 8,

		// An array of 4-byte integers, native endian.
		type_IntArray = 16,

		// A variable block of data.
		type_String = 32,

	};

		enum class ZCockpitType : unsigned {
		// Data of a type the current XPLM doesn't do. 
		ZUnknown = 0,
		ZInt = 1,
		ZFloat = 2,
		ZDouble = 4,
		ZVectorFloat = 8,
		ZVectorInt = 16,
		ZString = 32,
		ZBool = 64,
	};

	constexpr const char* XplaneTypeToString(const XplaneType type) noexcept
	{
		switch (type) {
		case XplaneType::type_Unknown: return "unknown";
		case XplaneType::type_Int: return "int";
		case XplaneType::type_Float: return "float";
		case XplaneType::type_Double: return "double";
		case XplaneType::type_FloatArray: return "float array";
		case XplaneType::type_IntArray: return "int array";
		case XplaneType::type_String: return "data";
		}
		return "XplaneType to string conversion failed";
	}

	enum class XplaneCommand_enum : unsigned
	{
		none,
		find,
		begin,
		end,
		once,
	};
	constexpr const char* XplaneCommandToString(const XplaneCommand_enum type) noexcept
	{
		switch (type) {
		case XplaneCommand_enum::none: return "none";
		case XplaneCommand_enum::find: return "find";
		case XplaneCommand_enum::begin: return "begin";
		case XplaneCommand_enum::end: return "end";
		case XplaneCommand_enum::once: return "once";
		}
		return "XplaneCommand_enum to string conversion failed";
	}


	struct InvalidType {};

	using var_t = std::variant<InvalidType, bool, int, float, double, std::vector<int>, std::vector<float>, std::vector<char>>;



	template <class IndexType, class ValueType>
	class EnumArray {
	 public:  
	  ValueType& operator[](IndexType i) { 
	    return array_[static_cast<int>(i)];	
	  }

	  const ValueType& operator[](IndexType i) const {
	    return array_[static_cast<int>(i)];
	  }

	  int size() const { return size_; }

	 private:
	  ValueType array_[static_cast<int>(IndexType::kMaxValue)]{};

	  int size_ = static_cast<int>(IndexType::kMaxValue);
	}; 

}