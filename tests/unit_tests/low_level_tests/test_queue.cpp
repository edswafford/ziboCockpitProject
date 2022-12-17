#include "gtest/gtest.h"
#include "../../../common/queue.hpp"

using namespace zcockpit;

namespace {
	// Tests Queue().

	TEST(TestLowLevel_queue, initial_queue_is_empty) {
		common::Queue<int>queue;

		EXPECT_EQ(0, queue.size());
	}

	TEST(TestLowLevel_queue, add_one_item) {
		common::Queue<int>queue;

		queue.push(5);
		EXPECT_EQ(1, queue.size());
	}

	TEST(TestLowLevel_queue, remove_one_item) {
		common::Queue<int>queue;

		queue.push(5);
		EXPECT_EQ(1, queue.size());
		auto item = queue.pop();
		EXPECT_EQ(0, queue.size());
		EXPECT_TRUE(item);
		EXPECT_EQ(5, *item);
	}

	TEST(TestLowLevel_queue, pop_empty_queue) {
		common::Queue<int>queue;

		auto item = queue.pop();
		EXPECT_EQ(0, queue.size());
		EXPECT_FALSE(item);
	}

	TEST(TestLowLevel_queue, push_and_pop_several_values) {
		common::Queue<double>queue;

		queue.push(5);
		queue.push(0.5);
		queue.push(572.5);

		auto item1 = queue.pop();
		EXPECT_EQ(2, queue.size());
		EXPECT_TRUE(item1);
		EXPECT_EQ(5.0, *item1);

		queue.push(0);
		auto item2 = queue.pop();
		auto item3 = queue.pop();

		EXPECT_EQ(1, queue.size());
		EXPECT_TRUE(item2);
		EXPECT_EQ(0.5, *item2);
		EXPECT_TRUE(item3);
		EXPECT_EQ(572.5, *item3);
	}
}