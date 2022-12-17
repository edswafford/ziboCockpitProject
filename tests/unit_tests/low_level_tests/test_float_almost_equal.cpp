#include <gtest/gtest.h>

#include "../../../common/almost_equal.hpp"

using namespace zcockpit;

namespace {
/** Regular large numbers - generally not problematic */
	TEST(TestLowLevel_nearly_equal, BigTest) {
		EXPECT_TRUE(common::almost_equal(1000000.0f, 1000001.0f));
		EXPECT_TRUE(common::almost_equal(1000001.f, 1000000.f));
		EXPECT_FALSE(common::almost_equal(10000.f, 10001.f));
		EXPECT_FALSE(common::almost_equal(10001.f, 10000.f));
	}

	   /** Negative large numbers */
	TEST(TestLowLevel_nearly_equal, BigNegTest) {
		EXPECT_TRUE(common::almost_equal(-1000000.0f, -1000001.0f));
		EXPECT_TRUE(common::almost_equal(-1000001.0f, -1000000.0f));
		EXPECT_FALSE(common::almost_equal(-10000.0f, -10001.0f));
		EXPECT_FALSE(common::almost_equal(-10001.0f, -10000.0f));
	}

	/** Numbers around 1 */
	TEST(TestLowLevel_nearly_equal, NearOneTest) {
		EXPECT_TRUE(common::almost_equal(1.0000001f, 1.0000002f));
		EXPECT_TRUE(common::almost_equal(1.0000002f, 1.0000001f));
		EXPECT_FALSE(common::almost_equal(1.0002f, 1.0001f));
		EXPECT_FALSE(common::almost_equal(1.0001f, 1.0002f));
	}

	/** Numbers around -1 */
	TEST(TestLowLevel_nearly_equal, NearNegOneTest) {
		EXPECT_TRUE(common::almost_equal(-1.000001f, -1.000002f));
		EXPECT_TRUE(common::almost_equal(-1.000002f, -1.000001f));
		EXPECT_FALSE(common::almost_equal(-1.0001f, -1.0002f));
		EXPECT_FALSE(common::almost_equal(-1.0002f, -1.0001f));
	}

	/** Numbers between 1 and 0 */
	TEST(TestLowLevel_nearly_equal, BetweenOneAndZeroTest) {
		EXPECT_TRUE(common::almost_equal(0.000000001000001f, 0.000000001000002f));
		EXPECT_TRUE(common::almost_equal(0.000000001000002f, 0.000000001000001f));
		EXPECT_FALSE(common::almost_equal(0.000000000001002f, 0.000000000001001f));
		EXPECT_FALSE(common::almost_equal(0.000000000001001f, 0.000000000001002f));
	}

	/** Numbers between -1 and 0 */
	TEST(TestLowLevel_nearly_equal, BetweenNegOneAndZeroTest) {
		EXPECT_TRUE(common::almost_equal(-0.000000001000001f, -0.000000001000002f));
		EXPECT_TRUE(common::almost_equal(-0.000000001000002f, -0.000000001000001f));
		EXPECT_FALSE(common::almost_equal(-0.000000000001002f, -0.000000000001001f));
		EXPECT_FALSE(common::almost_equal(-0.000000000001001f, -0.000000000001002f));
	}

	/** Small differences away from zero */
	TEST(TestLowLevel_nearly_equal, AwatFromZeroTest) {
		EXPECT_TRUE(common::almost_equal(0.3f, 0.30000003f));
		EXPECT_TRUE(common::almost_equal(-0.3f, -0.30000003f));
	}

	/** Comparisons involving zero */
	TEST(TestLowLevel_nearly_equal, ZeroTest) {
		EXPECT_TRUE(common::almost_equal(0.0f, 0.0f));
		EXPECT_TRUE(common::almost_equal(0.0f, -0.0f));
		EXPECT_TRUE(common::almost_equal(-0.0f, -0.0f));
		EXPECT_FALSE(common::almost_equal(0.00000001f, 0.0f));
		EXPECT_FALSE(common::almost_equal(0.0f, 0.00000001f));
		EXPECT_FALSE(common::almost_equal(-0.00000001f, 0.0f));
		EXPECT_FALSE(common::almost_equal(0.0f, -0.00000001f));

		EXPECT_TRUE(common::almost_equal(0.0f, 1e-40f, 0.01f));
		EXPECT_TRUE(common::almost_equal(1e-40f, 0.0f, 0.01f));
		EXPECT_FALSE(common::almost_equal(1e-40f, 0.0f, 0.000001f));
		EXPECT_FALSE(common::almost_equal(0.0f, 1e-40f, 0.000001f));

		EXPECT_TRUE(common::almost_equal(0.0f, -1e-40f, 0.1f));
		EXPECT_TRUE(common::almost_equal(-1e-40f, 0.0f, 0.1f));
		EXPECT_FALSE(common::almost_equal(-1e-40f, 0.0f, 0.000001f));
		EXPECT_FALSE(common::almost_equal(0.0f, -1e-40f, 0.000001f));
	}

	/**
	 * Comparisons involving extreme values (overflow potential)
	 */
	TEST(TestLowLevel_nearly_equal, ExtremeValuesTest) {
		EXPECT_TRUE(common::almost_equal(std::numeric_limits<float>::max(), std::numeric_limits<float>::max()));
		EXPECT_FALSE(common::almost_equal(std::numeric_limits<float>::max(), -std::numeric_limits<float>::max()));
		EXPECT_FALSE(common::almost_equal(-std::numeric_limits<float>::max(), std::numeric_limits<float>::max()));
		EXPECT_FALSE(common::almost_equal(std::numeric_limits<float>::max(), std::numeric_limits<float>::max() / 2));
		EXPECT_FALSE(common::almost_equal(std::numeric_limits<float>::max(), -std::numeric_limits<float>::max() / 2));
		EXPECT_FALSE(common::almost_equal(-std::numeric_limits<float>::max(), std::numeric_limits<float>::max() / 2));
	}

	/**
	 * Comparisons involving infinities
	 */
	TEST(TestLowLevel_nearly_equal, InfinityTest) {
		EXPECT_TRUE(common::almost_equal(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()));
		EXPECT_TRUE(common::almost_equal(-std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity()));
		EXPECT_FALSE(common::almost_equal(-std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()));
		EXPECT_FALSE(common::almost_equal(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::max()));
		EXPECT_FALSE(common::almost_equal(-std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::max()));
	}

	/**
	 * Comparisons involving NaN values
	 */
	TEST(TestLowLevel_nearly_equal, NaNTest) {
		EXPECT_FALSE(common::almost_equal(std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN()));
		EXPECT_FALSE(common::almost_equal(std::numeric_limits<float>::quiet_NaN(), 0.0f));
		EXPECT_FALSE(common::almost_equal(-0.0f, std::numeric_limits<float>::quiet_NaN()));
		EXPECT_FALSE(common::almost_equal(std::numeric_limits<float>::quiet_NaN(), -0.0f));
		EXPECT_FALSE(common::almost_equal(0.0f, std::numeric_limits<float>::quiet_NaN()));
		EXPECT_FALSE(common::almost_equal(std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::infinity()));
		EXPECT_FALSE(common::almost_equal(std::numeric_limits<float>::max(), std::numeric_limits<float>::quiet_NaN()));
		EXPECT_FALSE(common::almost_equal(std::numeric_limits<float>::quiet_NaN(), -std::numeric_limits<float>::infinity()));
		EXPECT_FALSE(common::almost_equal(-std::numeric_limits<float>::infinity(), std::numeric_limits<float>::quiet_NaN()));
		EXPECT_FALSE(common::almost_equal(std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::max()));
		EXPECT_FALSE(common::almost_equal(std::numeric_limits<float>::max(), std::numeric_limits<float>::quiet_NaN()));
		EXPECT_FALSE(common::almost_equal(std::numeric_limits<float>::quiet_NaN(), -std::numeric_limits<float>::max()));
		EXPECT_FALSE(common::almost_equal(-std::numeric_limits<float>::max(), std::numeric_limits<float>::quiet_NaN()));
		EXPECT_FALSE(common::almost_equal(std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::denorm_min()));
		EXPECT_FALSE(common::almost_equal(std::numeric_limits<float>::denorm_min(), std::numeric_limits<float>::quiet_NaN()));
		EXPECT_FALSE(common::almost_equal(std::numeric_limits<float>::quiet_NaN(), -std::numeric_limits<float>::denorm_min()));
		EXPECT_FALSE(common::almost_equal(-std::numeric_limits<float>::denorm_min(), std::numeric_limits<float>::quiet_NaN()));
	}

	/** Comparisons of numbers on opposite sides of 0 */
	TEST(TestLowLevel_nearly_equal, OppositeSidesOfZeroTest) {
		EXPECT_FALSE(common::almost_equal(1.000000001f, -1.0f));
		EXPECT_FALSE(common::almost_equal(-1.0f, 1.000000001f));
		EXPECT_FALSE(common::almost_equal(-1.000000001f, 1.0f));
		EXPECT_FALSE(common::almost_equal(1.0f, -1.000000001f));
		EXPECT_TRUE(common::almost_equal(10 * std::numeric_limits<float>::denorm_min(), 10 * -std::numeric_limits<float>::denorm_min()));
		EXPECT_FALSE(common::almost_equal(10000 * std::numeric_limits<float>::denorm_min(), 10000 * -std::numeric_limits<float>::denorm_min()));
	}

	/**
	 * The really tricky part - comparisons of numbers very close to zero.
	 */
	TEST(TestLowLevel_nearly_equal, CloseToZeroTest) {
		EXPECT_TRUE(common::almost_equal(std::numeric_limits<float>::denorm_min(), std::numeric_limits<float>::denorm_min()));
		EXPECT_TRUE(common::almost_equal(std::numeric_limits<float>::denorm_min(), -std::numeric_limits<float>::denorm_min()));
		EXPECT_TRUE(common::almost_equal(-std::numeric_limits<float>::denorm_min(), std::numeric_limits<float>::denorm_min()));
		EXPECT_TRUE(common::almost_equal(std::numeric_limits<float>::denorm_min(), 0.0f));
		EXPECT_TRUE(common::almost_equal(0.0f, std::numeric_limits<float>::denorm_min()));
		EXPECT_TRUE(common::almost_equal(-std::numeric_limits<float>::denorm_min(), 0.0f));
		EXPECT_TRUE(common::almost_equal(0.0f, -std::numeric_limits<float>::denorm_min()));

		EXPECT_FALSE(common::almost_equal(0.000000001f, -std::numeric_limits<float>::denorm_min()));
		EXPECT_FALSE(common::almost_equal(0.000000001f, std::numeric_limits<float>::denorm_min()));
		EXPECT_FALSE(common::almost_equal(std::numeric_limits<float>::denorm_min(), 0.000000001f));
		EXPECT_FALSE(common::almost_equal(-std::numeric_limits<float>::denorm_min(), 0.000000001f));
	}
}