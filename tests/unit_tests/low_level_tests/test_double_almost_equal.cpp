#include <gtest/gtest.h>

#include "../../../common/almost_equal.hpp"

using namespace zcockpit;

namespace {
/** Regular large numbers - generally not problematic */
	TEST(TestLowLevel_nearly_equal, DoubleBigTest) {
		EXPECT_TRUE(common::almost_equal(1000000.0, 1000001.0, 1.0e-5));
		EXPECT_TRUE(common::almost_equal(1000001., 1000000.0, 1.0e-5));
		EXPECT_FALSE(common::almost_equal(10000., 10001.0, 1.0e-5));
		EXPECT_FALSE(common::almost_equal(10001., 10000.0, 1.0e-5));
	}

	   /** Negative large numbers */
	TEST(TestLowLevel_nearly_equal, DoubleBigNegTest) {
		EXPECT_TRUE(common::almost_equal(-1000000.0, -1000001.0, 1.0e-5));
		EXPECT_TRUE(common::almost_equal(-1000001.0, -1000000.0, 1.0e-5));
		EXPECT_FALSE(common::almost_equal(-10000.0, -10001.0, 1.0e-5));
		EXPECT_FALSE(common::almost_equal(-10001.0, -10000.0, 1.0e-5));
	}

	/** Numbers around 1 */
	TEST(TestLowLevel_nearly_equal, DoubleNearOneTest) {
		EXPECT_TRUE(common::almost_equal(1.0000001, 1.0000002, 1.0e-5));
		EXPECT_TRUE(common::almost_equal(1.0000002, 1.0000001, 1.0e-5));
		EXPECT_FALSE(common::almost_equal(1.0002, 1.0001, 1.0e-5));
		EXPECT_FALSE(common::almost_equal(1.0001, 1.0002, 1.0e-5));
	}

	/** Numbers around -1 */
	TEST(TestLowLevel_nearly_equal, DoubleNearNegOneTest) {
		EXPECT_TRUE(common::almost_equal(-1.000001, -1.000002, 1.0e-5));
		EXPECT_TRUE(common::almost_equal(-1.000002, -1.000001, 1.0e-5));
		EXPECT_FALSE(common::almost_equal(-1.0001, -1.0002, 1.0e-5));
		EXPECT_FALSE(common::almost_equal(-1.0002, -1.0001, 1.0e-5));
	}

	/** Numbers between 1 and 0 */
	TEST(TestLowLevel_nearly_equal, DoubleBetweenOneAndZeroTest) {
		EXPECT_TRUE(common::almost_equal(0.000000001000001, 0.000000001000002, 1.0e-5));
		EXPECT_TRUE(common::almost_equal(0.000000001000002, 0.000000001000001, 1.0e-5));
		EXPECT_FALSE(common::almost_equal(0.000000000001002, 0.000000000001001));
		EXPECT_FALSE(common::almost_equal(0.000000000001001, 0.000000000001002));
	}

	/** Numbers between -1 and 0 */
	TEST(TestLowLevel_nearly_equal, DoubleBetweenNegOneAndZeroTest) {
		EXPECT_TRUE(common::almost_equal(-0.000000001000001, -0.000000001000002, 1.0e-5));
		EXPECT_TRUE(common::almost_equal(-0.000000001000002, -0.000000001000001, 1.0e-5));
		EXPECT_FALSE(common::almost_equal(-0.000000000001002, -0.00000000000100, 1.0e-5));
		EXPECT_FALSE(common::almost_equal(-0.000000000001001, -0.000000000001002, 1.0e-5));
	}

	/** Small differences away from zero */
	TEST(TestLowLevel_nearly_equal, DoubleAwayFromZeroTest) {
		EXPECT_TRUE(common::almost_equal(0.3, 0.30000003, 1.0e-5));
		EXPECT_TRUE(common::almost_equal(-0.3, -0.30000003, 1.0e-5));
	}

	/** Comparisons involving zero */
	TEST(TestLowLevel_nearly_equal, DoubleZeroTest) {
		EXPECT_TRUE(common::almost_equal(0.0, 0.0));
		EXPECT_TRUE(common::almost_equal(0.0, -0.0));
		EXPECT_TRUE(common::almost_equal(-0.0, -0.0));
		EXPECT_FALSE(common::almost_equal(0.00000001, 0.0));
		EXPECT_FALSE(common::almost_equal(0.0, 0.00000001));
		EXPECT_FALSE(common::almost_equal(-0.00000001, 0.0));
		EXPECT_FALSE(common::almost_equal(0.0, -0.00000001));

		EXPECT_TRUE(common::almost_equal(0.0, 1.e-314, 1.0e-5));
		EXPECT_TRUE(common::almost_equal(1e-314, 0.0, 1.0e-5));
		EXPECT_FALSE(common::almost_equal(1e-40, 0.0, 0.000001));
		EXPECT_FALSE(common::almost_equal(0.0, 1e-40, 0.000001));

		EXPECT_TRUE(common::almost_equal(0.0, -1e-314, 1.0e-5));
		EXPECT_TRUE(common::almost_equal(-1e-314, 0.0, 1.0e-5));
		EXPECT_FALSE(common::almost_equal(-1e-40, 0.0, 0.000001));
		EXPECT_FALSE(common::almost_equal(0.0, -1e-40, 0.000001));
	}

	/**
	 * Comparisons involving extreme values (overflow potential)
	 */
	TEST(TestLowLevel_nearly_equal, DoubleExtremeValuesTest) {
		EXPECT_TRUE(common::almost_equal(std::numeric_limits<double>::max(), std::numeric_limits<double>::max()));
		EXPECT_FALSE(common::almost_equal(std::numeric_limits<double>::max(), -std::numeric_limits<double>::max()));
		EXPECT_FALSE(common::almost_equal(-std::numeric_limits<double>::max(), std::numeric_limits<double>::max()));
		EXPECT_FALSE(common::almost_equal(std::numeric_limits<double>::max(), std::numeric_limits<double>::max() / 2));
		EXPECT_FALSE(common::almost_equal(std::numeric_limits<double>::max(), -std::numeric_limits<double>::max() / 2));
		EXPECT_FALSE(common::almost_equal(-std::numeric_limits<double>::max(), std::numeric_limits<double>::max() / 2));
	}

	/**
	 * Comparisons involving infinities
	 */
	TEST(TestLowLevel_nearly_equal, DoubleInfinityTest) {
		EXPECT_TRUE(common::almost_equal(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()));
		EXPECT_TRUE(common::almost_equal(-std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity()));
		EXPECT_FALSE(common::almost_equal(-std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()));
		EXPECT_FALSE(common::almost_equal(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::max()));
		EXPECT_FALSE(common::almost_equal(-std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::max()));
	}

	/**
	 * Comparisons involving NaN values
	 */
	TEST(TestLowLevel_nearly_equal, DoubleNaNTest) {
		EXPECT_FALSE(common::almost_equal(std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN()));
		EXPECT_FALSE(common::almost_equal(std::numeric_limits<double>::quiet_NaN(), 0.0));
		EXPECT_FALSE(common::almost_equal(-0.0, std::numeric_limits<double>::quiet_NaN()));
		EXPECT_FALSE(common::almost_equal(std::numeric_limits<double>::quiet_NaN(), -0.0));
		EXPECT_FALSE(common::almost_equal(0.0, std::numeric_limits<double>::quiet_NaN()));
		EXPECT_FALSE(common::almost_equal(std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::infinity()));
		EXPECT_FALSE(common::almost_equal(std::numeric_limits<double>::max(), std::numeric_limits<double>::quiet_NaN()));
		EXPECT_FALSE(common::almost_equal(std::numeric_limits<double>::quiet_NaN(), -std::numeric_limits<double>::infinity()));
		EXPECT_FALSE(common::almost_equal(-std::numeric_limits<double>::infinity(), std::numeric_limits<double>::quiet_NaN()));
		EXPECT_FALSE(common::almost_equal(std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::max()));
		EXPECT_FALSE(common::almost_equal(std::numeric_limits<double>::max(), std::numeric_limits<double>::quiet_NaN()));
		EXPECT_FALSE(common::almost_equal(std::numeric_limits<double>::quiet_NaN(), -std::numeric_limits<double>::max()));
		EXPECT_FALSE(common::almost_equal(-std::numeric_limits<double>::max(), std::numeric_limits<double>::quiet_NaN()));
		EXPECT_FALSE(common::almost_equal(std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::denorm_min()));
		EXPECT_FALSE(common::almost_equal(std::numeric_limits<double>::denorm_min(), std::numeric_limits<double>::quiet_NaN()));
		EXPECT_FALSE(common::almost_equal(std::numeric_limits<double>::quiet_NaN(), -std::numeric_limits<double>::denorm_min()));
		EXPECT_FALSE(common::almost_equal(-std::numeric_limits<double>::denorm_min(), std::numeric_limits<double>::quiet_NaN()));
	}

	/** Comparisons of numbers on opposite sides of 0 */
	TEST(TestLowLevel_nearly_equal, DoubleOppositeSidesOfZeroTest) {
		EXPECT_FALSE(common::almost_equal(1.000000001, -1.0));
		EXPECT_FALSE(common::almost_equal(-1.0, 1.000000001));
		EXPECT_FALSE(common::almost_equal(-1.000000001, 1.0));
		EXPECT_FALSE(common::almost_equal(1.0, -1.000000001));
		EXPECT_TRUE(common::almost_equal(10 * std::numeric_limits<double>::denorm_min(), 10 * -std::numeric_limits<double>::denorm_min()));
		EXPECT_FALSE(common::almost_equal(10000 * std::numeric_limits<double>::denorm_min(), 10000 * -std::numeric_limits<double>::denorm_min()));
	}

	/**
	 * The really tricky part - comparisons of numbers very close to zero.
	 */
	TEST(TestLowLevel_nearly_equal, DoubleCloseToZeroTest) {
		EXPECT_TRUE(common::almost_equal(std::numeric_limits<double>::denorm_min(), std::numeric_limits<double>::denorm_min()));
		EXPECT_TRUE(common::almost_equal(std::numeric_limits<double>::denorm_min(), -std::numeric_limits<double>::denorm_min()));
		EXPECT_TRUE(common::almost_equal(-std::numeric_limits<double>::denorm_min(), std::numeric_limits<double>::denorm_min()));
		EXPECT_TRUE(common::almost_equal(std::numeric_limits<double>::denorm_min(), .00));
		EXPECT_TRUE(common::almost_equal(0.0, std::numeric_limits<double>::denorm_min()));
		EXPECT_TRUE(common::almost_equal(-std::numeric_limits<double>::denorm_min(), 0.0));
		EXPECT_TRUE(common::almost_equal(0.0, -std::numeric_limits<double>::denorm_min()));

		EXPECT_FALSE(common::almost_equal(0.000000001, -std::numeric_limits<double>::denorm_min()));
		EXPECT_FALSE(common::almost_equal(0.000000001, std::numeric_limits<double>::denorm_min()));
		EXPECT_FALSE(common::almost_equal(std::numeric_limits<double>::denorm_min(), 0.000000001));
		EXPECT_FALSE(common::almost_equal(-std::numeric_limits<double>::denorm_min(), 0.000000001));
	}
}