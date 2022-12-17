#include "../mocks/xplane_mock.hpp"


#include <gtest/gtest.h>

#include "XPLMDataAccess.h"
#include "xp_data_ref.hpp"
#include "xp_data_ref.cpp"
#include "shared_types.hpp"

using namespace zcockpit::common;

namespace {

	using ::testing::AtLeast;
	using ::testing::Exactly;
	using ::testing::_;
	using ::testing::Return;
	using ::testing::ElementsAreArray;
	using ::testing::SetArgPointee;
	using ::testing::SetArrayArgument;
	using ::testing::DoAll;

	using namespace zcockpit::common;


	MATCHER_P(CompareCharArray, array, "") { return strcmp(array, arg) == 0; }

	TEST(TestCockpitDataref, TestInitialization) {
		const XplaneDataAccessMock ds_mock;

		int fake_ref = 0;


		// INT
		std::string data_ref_name = "data ref 1";
		EXPECT_CALL(ds_mock, XPLMFindDataRef(CompareCharArray(data_ref_name.c_str()))).WillOnce(Return(&fake_ref));
		EXPECT_CALL(ds_mock, XPLMGetDataRefTypes(&fake_ref)).WillOnce(Return(xplmType_Int));

		auto data_ref = XPDataRef(data_ref_name, XplaneType::type_Int);
		EXPECT_EQ(std::get<IntXPDataRef>(data_ref.variant()).value(), -999999);

		// Float
		data_ref_name = "data ref 2";
		EXPECT_CALL(ds_mock, XPLMFindDataRef(CompareCharArray(data_ref_name.c_str()))).WillOnce(Return(&fake_ref));
		EXPECT_CALL(ds_mock, XPLMGetDataRefTypes(&fake_ref)).WillOnce(Return(xplmType_Float));

		data_ref = XPDataRef(data_ref_name, XplaneType::type_Float);
		EXPECT_EQ(std::get<FloatXPDataRef>(data_ref.variant()).value(), -999999.0f);

		// DOUBLE
		data_ref_name = "data ref 3";
		EXPECT_CALL(ds_mock, XPLMFindDataRef(CompareCharArray(data_ref_name.c_str()))).WillOnce(Return(&fake_ref));
		EXPECT_CALL(ds_mock, XPLMGetDataRefTypes(&fake_ref)).WillOnce(Return(xplmType_Double));

		data_ref = XPDataRef(data_ref_name, XplaneType::type_Double);
		EXPECT_EQ(std::get<DoubleXPDataRef>(data_ref.variant()).value(), -999999.0);

		// VECTOR FLOAT
		data_ref_name = "data ref 4";
		EXPECT_CALL(ds_mock, XPLMFindDataRef(CompareCharArray(data_ref_name.c_str()))).WillOnce(Return(&fake_ref));
		EXPECT_CALL(ds_mock, XPLMGetDataRefTypes(&fake_ref)).WillOnce(Return(xplmType_Float | xplmType_Int | xplmType_FloatArray | xplmType_IntArray | xplmType_Data | xplmType_Unknown));

		data_ref = XPDataRef(data_ref_name, XplaneType::type_FloatArray);
		EXPECT_EQ(std::get<VectorFloatXPDataRef>(data_ref.variant()).value().size(), 0);

		// VECTOR INT
		data_ref_name = "data ref 5";
		EXPECT_CALL(ds_mock, XPLMFindDataRef(CompareCharArray(data_ref_name.c_str()))).WillOnce(Return(&fake_ref));
		EXPECT_CALL(ds_mock, XPLMGetDataRefTypes(&fake_ref)).WillOnce(Return(xplmType_Float | xplmType_Int | xplmType_FloatArray | xplmType_IntArray | xplmType_Data | xplmType_Unknown));

		data_ref = XPDataRef(data_ref_name, XplaneType::type_IntArray);
		EXPECT_EQ(std::get<VectorIntXPDataRef>(data_ref.variant()).value().size(), 0);

		// VECTOR BYTE
		data_ref_name = "data ref 6";
		EXPECT_CALL(ds_mock, XPLMFindDataRef(CompareCharArray(data_ref_name.c_str()))).WillOnce(Return(&fake_ref));
		EXPECT_CALL(ds_mock, XPLMGetDataRefTypes(&fake_ref)).WillOnce(Return(xplmType_Float | xplmType_Int | xplmType_FloatArray | xplmType_IntArray | xplmType_Data | xplmType_Unknown));

		data_ref = XPDataRef(data_ref_name, XplaneType::type_String);
		EXPECT_EQ(std::get<VectorCharXPDataRef>(data_ref.variant()).value().size(), 0);

	}

	TEST(TestCockpitDataref, TestDefaultXplaneType) {
		const XplaneDataAccessMock ds_mock;

		int fake_ref = 0;

		// Double
		std::string data_ref_name = "data ref";
		EXPECT_CALL(ds_mock, XPLMFindDataRef(CompareCharArray(data_ref_name.c_str()))).WillOnce(Return(&fake_ref));
		EXPECT_CALL(ds_mock, XPLMGetDataRefTypes(&fake_ref)).WillOnce(Return(xplmType_Int | xplmType_Double | xplmType_Float));

		auto data_ref = XPDataRef(data_ref_name);
		EXPECT_TRUE(std::holds_alternative<FloatXPDataRef>(data_ref.variant()));

		// Float
		data_ref_name = "data ref";
		EXPECT_CALL(ds_mock, XPLMFindDataRef(CompareCharArray(data_ref_name.c_str()))).WillOnce(Return(&fake_ref));
		EXPECT_CALL(ds_mock, XPLMGetDataRefTypes(&fake_ref)).WillOnce(Return(xplmType_Int | xplmType_Float));

		data_ref = XPDataRef(data_ref_name);
		EXPECT_TRUE(std::holds_alternative<FloatXPDataRef>(data_ref.variant()));

		// Int
		data_ref_name = "data ref";
		EXPECT_CALL(ds_mock, XPLMFindDataRef(CompareCharArray(data_ref_name.c_str()))).WillOnce(Return(&fake_ref));
		EXPECT_CALL(ds_mock, XPLMGetDataRefTypes(&fake_ref)).WillOnce(Return(xplmType_Int | xplmType_FloatArray | xplmType_IntArray | xplmType_Data | xplmType_Unknown));

		data_ref = XPDataRef(data_ref_name);
		EXPECT_TRUE(std::holds_alternative<IntXPDataRef>(data_ref.variant()));

		// FloatArray
		data_ref_name = "data ref";
		EXPECT_CALL(ds_mock, XPLMFindDataRef(CompareCharArray(data_ref_name.c_str()))).WillOnce(Return(&fake_ref));
		EXPECT_CALL(ds_mock, XPLMGetDataRefTypes(&fake_ref)).WillOnce(Return(xplmType_FloatArray | xplmType_IntArray | xplmType_Data | xplmType_Unknown));

		data_ref = XPDataRef(data_ref_name);
		EXPECT_TRUE(std::holds_alternative<VectorFloatXPDataRef>(data_ref.variant()));

		// IntArray
		data_ref_name = "data ref";
		EXPECT_CALL(ds_mock, XPLMFindDataRef(CompareCharArray(data_ref_name.c_str()))).WillOnce(Return(&fake_ref));
		EXPECT_CALL(ds_mock, XPLMGetDataRefTypes(&fake_ref)).WillOnce(Return(xplmType_IntArray | xplmType_Data | xplmType_Unknown));

		data_ref = XPDataRef(data_ref_name);
		EXPECT_TRUE(std::holds_alternative<VectorIntXPDataRef>(data_ref.variant()));

		// Data
		data_ref_name = "data ref";
		EXPECT_CALL(ds_mock, XPLMFindDataRef(CompareCharArray(data_ref_name.c_str()))).WillOnce(Return(&fake_ref));
		EXPECT_CALL(ds_mock, XPLMGetDataRefTypes(&fake_ref)).WillOnce(Return(xplmType_Data | xplmType_Unknown));

		data_ref = XPDataRef(data_ref_name);
		EXPECT_TRUE(std::holds_alternative<VectorCharXPDataRef>(data_ref.variant()));

		// Unknown
		data_ref_name = "data ref expect exception";
		EXPECT_CALL(ds_mock, XPLMFindDataRef(CompareCharArray(data_ref_name.c_str()))).WillOnce(Return(&fake_ref));
		EXPECT_CALL(ds_mock, XPLMGetDataRefTypes(&fake_ref)).WillOnce(Return(xplmType_Unknown));

		EXPECT_THROW(
			{
				try
				{
				data_ref = XPDataRef(data_ref_name);
				}
				catch (const std::invalid_argument e) {
					EXPECT_STREQ("Cannot find data type for data ref expect exception", e.what());
					throw;
				}
			}, std::invalid_argument);
	}

	TEST(TestCockpitDataref, TestFindException)
	{
		const XplaneDataAccessMock ds_mock;
		std::string data_ref_name = "data ref expect exception";
		EXPECT_CALL(ds_mock, XPLMFindDataRef(CompareCharArray(data_ref_name.c_str()))).WillOnce(Return(nullptr));

		EXPECT_THROW(
			{
				try
				{
				auto data_ref = XPDataRef(data_ref_name);
				}
				catch (const std::invalid_argument e) {
					EXPECT_STREQ("Cannot find dataref data ref expect exception", e.what());
					throw;
				}
			}, std::invalid_argument);
	}


	TEST(TestCockpitDataref, TestNewValues)
	{
		const XplaneDataAccessMock ds_mock;
		int fake_ref;
		std::unordered_map<int, std::unique_ptr<XPDataRef>> refs;

		EXPECT_CALL(ds_mock, XPLMFindDataRef(_)).WillRepeatedly(Return(&fake_ref));
		EXPECT_CALL(ds_mock, XPLMGetDataRefTypes(&fake_ref)).WillRepeatedly(Return(xplmType_Int | xplmType_Double | xplmType_Float));

		std::unique_ptr<XPDataRef> data_ref_int = std::make_unique<XPDataRef>("data ref int", XplaneType::type_Int);
		std::unique_ptr<XPDataRef> data_ref_float = std::make_unique<XPDataRef>("data ref float", XplaneType::type_Float);
		std::unique_ptr<XPDataRef> data_ref_double = std::make_unique<XPDataRef>("data ref double", XplaneType::type_Double);

		refs.emplace(1, std::move(data_ref_int));
		refs.emplace(2, std::move(data_ref_float));
		refs.emplace(3, std::move(data_ref_double));

		EXPECT_CALL(ds_mock, XPLMGetDatai(_)).WillOnce(Return(225));
		EXPECT_CALL(ds_mock, XPLMGetDataf(_)).WillOnce(Return(2398.5f));
		EXPECT_CALL(ds_mock, XPLMGetDatad(_)).WillOnce(Return(0.005678));

		XPDataRef::update_new_values(refs);
		EXPECT_NE(std::get<IntXPDataRef>(refs[1]->variant()).value(), 225);
		EXPECT_NE(std::get<FloatXPDataRef>(refs[2]->variant()).value(), 2398.5f);
		EXPECT_NE(std::get<DoubleXPDataRef>(refs[3]->variant()).value(), 0.005678);

		auto values = XPDataRef::changed_values(refs);

		EXPECT_EQ(std::get<IntXPDataRef>(refs[1]->variant()).value(), 225);
		EXPECT_EQ(std::get<FloatXPDataRef>(refs[2]->variant()).value(), 2398.5f);
		EXPECT_EQ(std::get<DoubleXPDataRef>(refs[3]->variant()).value(), 0.005678);

		EXPECT_EQ(values.size(), 3);
		EXPECT_EQ(std::get<int>(values[1]), 225);
		EXPECT_EQ(std::get<float>(values[2]), 2398.5f);
		EXPECT_EQ(std::get<double>(values[3]), 0.005678);

		// Test Unchanged Values
		EXPECT_CALL(ds_mock, XPLMGetDatai(_)).WillOnce(Return(225));
		EXPECT_CALL(ds_mock, XPLMGetDataf(_)).WillOnce(Return(2398.5f));
		EXPECT_CALL(ds_mock, XPLMGetDatad(_)).WillOnce(Return(0.0056700));
		XPDataRef::update_new_values(refs);
		values = XPDataRef::changed_values(refs);
		EXPECT_EQ(std::get<IntXPDataRef>(refs[1]->variant()).value(), 225);
		EXPECT_EQ(std::get<FloatXPDataRef>(refs[2]->variant()).value(), 2398.5f);
		EXPECT_EQ(std::get<DoubleXPDataRef>(refs[3]->variant()).value(), 0.00567);

		EXPECT_EQ(values.size(), 1);
		EXPECT_EQ(std::get<double>(values[3]), 0.00567);
	}

	TEST(TestCockpitDataref, TestNewIntVectorValues)
	{
		const XplaneDataAccessMock ds_mock;
		int fake_ref;
		std::unordered_map<int, std::unique_ptr<XPDataRef>> refs;

		EXPECT_CALL(ds_mock, XPLMFindDataRef(_)).WillRepeatedly(Return(&fake_ref));
		EXPECT_CALL(ds_mock, XPLMGetDataRefTypes(&fake_ref)).WillRepeatedly(Return(xplmType_FloatArray | xplmType_IntArray | xplmType_Data | xplmType_Unknown));

		std::unique_ptr<XPDataRef> data_ref_vector_int = std::make_unique<XPDataRef>("data ref vector int", XplaneType::type_IntArray);

		refs.emplace(11, std::move(data_ref_vector_int));
		EXPECT_CALL(ds_mock, XPLMGetDatavi(_, nullptr, 0, 0)).WillRepeatedly(Return(0));

		int new_int_values[3] = { 23, -7, 22456 };
		EXPECT_CALL(ds_mock, XPLMGetDatavi(_, nullptr, 0, 0)).WillRepeatedly(Return(3));
		EXPECT_CALL(ds_mock, XPLMGetDatavi(_, _, 0, 3)).WillRepeatedly(DoAll(SetArrayArgument<1>(new_int_values, new_int_values + 3), Return(3)));
		// Get New values
		XPDataRef::update_new_values(refs);

		std::vector<int> int_values = std::get<VectorIntXPDataRef>(refs[11]->variant()).value();
		EXPECT_NE(int_values[0], 23);
		EXPECT_NE(int_values[1], -7);
		EXPECT_NE(int_values[2], 22456);

		// Update Changed Values
		auto  data_ref_values = XPDataRef::changed_values(refs);
		int_values = std::get<VectorIntXPDataRef>(refs[11]->variant()).value();
		EXPECT_EQ(int_values[0], 23);
		EXPECT_EQ(int_values[1], -7);
		EXPECT_EQ(int_values[2], 22456);

		// Check values returned
		EXPECT_EQ(data_ref_values.size(), 1);
		int_values = std::get<std::vector<int>>(data_ref_values[11]);
		EXPECT_EQ(int_values[0], 23);
		EXPECT_EQ(int_values[1], -7);
		EXPECT_EQ(int_values[2], 22456);

		// Check Values have not Changed 
		EXPECT_CALL(ds_mock, XPLMGetDatavi(_, nullptr, 0, 0)).WillRepeatedly(Return(3));
		EXPECT_CALL(ds_mock, XPLMGetDatavi(_, _, 0, 3)).WillRepeatedly(DoAll(SetArrayArgument<1>(new_int_values, new_int_values + 3), Return(3)));
		XPDataRef::update_new_values(refs);
		data_ref_values = XPDataRef::changed_values(refs);
		int_values = std::get<VectorIntXPDataRef>(refs[11]->variant()).value();
		EXPECT_EQ(int_values[0], 23);
		EXPECT_EQ(int_values[1], -7);
		EXPECT_EQ(int_values[2], 22456);
		EXPECT_EQ(data_ref_values.size(), 0);
	}
	TEST(TestCockpitDataref, TestNewFloatVectorValues)
	{
		const XplaneDataAccessMock ds_mock;
		int fake_ref;
		std::unordered_map<int, std::unique_ptr<XPDataRef>> refs;

		EXPECT_CALL(ds_mock, XPLMFindDataRef(_)).WillRepeatedly(Return(&fake_ref));
		EXPECT_CALL(ds_mock, XPLMGetDataRefTypes(&fake_ref)).WillRepeatedly(Return(xplmType_FloatArray | xplmType_IntArray | xplmType_Data | xplmType_Unknown));

		std::unique_ptr<XPDataRef> data_ref_vector_float = std::make_unique<XPDataRef>("data ref vector float", XplaneType::type_FloatArray);
	//	std::unique_ptr<XPDataRef> data_ref_vector_byte = std::make_unique<XPDataRef>("data ref vector byte", XplaneType::type_String);

		refs.emplace(12, std::move(data_ref_vector_float));
		EXPECT_CALL(ds_mock, XPLMGetDatavf(_, nullptr, 0, 0)).WillRepeatedly(Return(0));


		// FLOAT VECTOR
		float new_float_values[4] = { 2.3f, -7.1f, 22456.55f, -0.00003f };
		EXPECT_CALL(ds_mock, XPLMGetDatavf(_, nullptr, 0, 0)).WillRepeatedly(Return(4));
		EXPECT_CALL(ds_mock, XPLMGetDatavf(_, _, 0, 4)).WillRepeatedly(DoAll(SetArrayArgument<1>(new_float_values, new_float_values + 4), Return(4)));

		XPDataRef::update_new_values(refs);
		std::vector<float>  float_values = std::get<VectorFloatXPDataRef>(refs[12]->variant()).value();
		EXPECT_NE(float_values[0], 2.3f);
		EXPECT_NE(float_values[1], -7.1f);
		EXPECT_NE(float_values[2], 22456.55f);
		EXPECT_NE(float_values[3], 0.00003f);

		auto data_ref_values = XPDataRef::changed_values(refs);
		float_values = std::get<VectorFloatXPDataRef>(refs[12]->variant()).value();
		EXPECT_EQ(float_values[0], 2.3f);
		EXPECT_EQ(float_values[1], -7.1f);
		EXPECT_EQ(float_values[2], 22456.55f);
		EXPECT_EQ(float_values[3], -0.00003f);

		EXPECT_EQ(data_ref_values.size(), 1);
		float_values = std::get<std::vector<float>>(data_ref_values[12]);
		EXPECT_EQ(float_values[0], 2.3f);
		EXPECT_EQ(float_values[1], -7.1f);
		EXPECT_EQ(float_values[2], 22456.55f);
		EXPECT_EQ(float_values[3], -0.00003f);
	}

	int set_4_data_vector_values(void* ref, void* ptr, int start, int max)
	{
		const auto byte_ptr = static_cast<uint8_t*>(ptr);
		byte_ptr[0] = 2;
		byte_ptr[1] = 0xF9;
		byte_ptr[2] = 5;
		byte_ptr[3] = 1;
		return 4;
	}

	TEST(TestCockpitDataref, TestNewDataVectorValues)
	{
		const XplaneDataAccessMock ds_mock;
		int fake_ref;
		std::unordered_map<int, std::unique_ptr<XPDataRef>> refs;

		EXPECT_CALL(ds_mock, XPLMFindDataRef(_)).WillRepeatedly(Return(&fake_ref));
		EXPECT_CALL(ds_mock, XPLMGetDataRefTypes(&fake_ref)).WillRepeatedly(Return(xplmType_FloatArray | xplmType_IntArray | xplmType_Data | xplmType_Unknown));

		std::unique_ptr<XPDataRef> data_ref_vector_byte = std::make_unique<XPDataRef>("data ref vector byte", XplaneType::type_String);

		refs.emplace(12, std::move(data_ref_vector_byte));
		EXPECT_CALL(ds_mock, XPLMGetDatab(_, nullptr, 0, 0)).WillRepeatedly(Return(0));


		// Data VECTOR;
		EXPECT_CALL(ds_mock, XPLMGetDatab(_, nullptr, 0, 0)).WillRepeatedly(Return(4));
		EXPECT_CALL(ds_mock, XPLMGetDatab(_, _, 0, 4)).WillRepeatedly(&set_4_data_vector_values);

		XPDataRef::update_new_values(refs);
		std::vector<char>  data_values = std::get<VectorCharXPDataRef>(refs[12]->variant()).value();
		EXPECT_NE(data_values[0], 2);
		EXPECT_NE(data_values[1], 0XF9);
		EXPECT_NE(data_values[2], 5);
		EXPECT_NE(data_values[3], 1);

		auto data_ref_values = XPDataRef::changed_values(refs);
		data_values = std::get<VectorCharXPDataRef>(refs[12]->variant()).value();
		EXPECT_EQ(data_values[0], 2);
		EXPECT_EQ(data_values[1], -7);
		EXPECT_EQ(data_values[2], 5);
		EXPECT_EQ(data_values[3], 1);

		EXPECT_EQ(data_ref_values.size(), 1);
		data_values = std::get<std::vector<char>>(data_ref_values[12]);
		EXPECT_EQ(data_values[0], 2);
		EXPECT_EQ(data_values[1], -7);
		EXPECT_EQ(data_values[2], 5);
		EXPECT_EQ(data_values[3], 1);

	}
}