#pragma once

#undef min
#undef max

#include <limits>
#include <algorithm>
namespace zcockpit::common {
	
	// float epsilon is  1.192092896e-07F   
	// 800 * epsilon == 9.536743e-05
	template <typename T>
	bool almost_equal(const T a, const T b, const T epsilon = 800 * std::numeric_limits<T>::epsilon()) {
		if (a == b) {
			return true;
		}

		const auto absA = std::abs(a);
		const auto absB = std::abs(b);
		const auto diff = std::abs(a - b);

		if (a == 0 || b == 0 || diff < std::numeric_limits<T>::min()) {
			// a or b is zero or difference is close to zero
			return diff < (epsilon* std::numeric_limits<T>::min());
		}

		return  diff / std::min((absA + absB), std::numeric_limits<T>::max()) < epsilon;
	}
}