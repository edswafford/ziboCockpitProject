#pragma once
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "packet.hpp"
#include "shared_types.hpp"

#include "../common/logger.hpp"
extern logger LOG;

namespace zcockpit::common {

		class IntXPDataRef
		{
		public:
			explicit IntXPDataRef(void* xplane_data_ref, int data_type_id, bool is_annun) : 
				xplane_data_ref_(xplane_data_ref), 
				xp_data_type_id_(data_type_id), 
				is_bool_annum(is_annun)
			{
				
			}
			[[nodiscard]] int value() const { return value_; }

		private:
			void update_new_value_from_xplane();
			void set_new_xplane_value(common::var_t val)
			{
				LOG() << "Set Value: int NOT IMPLEMENTED ";
			}

			[[nodiscard]] std::optional<common::var_t> get_changed_value();
			[[nodiscard]] common::var_t get_new_value() { return new_value_; }

			int value_{ -999999 };
			int new_value_{ -999999 };
			void* xplane_data_ref_{ nullptr };
			int xp_data_type_id_{ 0 };
			bool is_bool_annum{false};
			friend class XPDataRef;
		};


		class FloatXPDataRef
		{
		public:
			explicit FloatXPDataRef() = default;
			explicit FloatXPDataRef(void* xplane_data_ref, int data_type_id, bool is_annun) : 
				 xplane_data_ref_(xplane_data_ref), 
				xp_data_type_id_(data_type_id), 
				is_bool_annum(is_annun)
			{
				
			}

			[[nodiscard]] float value() const { return value_; }


		private:
			void update_new_value_from_xplane();
			[[nodiscard]] std::optional<common::var_t> get_changed_value();
			void set_new_xplane_value(common::var_t val);
			[[nodiscard]] common::var_t get_new_value() { return new_value_; }

			float value_{ -999999.0f };
			float new_value_{ -999999.0f };
			void* xplane_data_ref_{ nullptr };
			int xp_data_type_id_{ 0 };
			bool is_bool_annum{false};
			friend class XPDataRef;
		};



		class DoubleXPDataRef
		{
		public:
			explicit DoubleXPDataRef() = default;
			explicit DoubleXPDataRef(void* xplane_data_ref, int data_type_id, bool is_annun) :
				xplane_data_ref_(xplane_data_ref), 
				xp_data_type_id_(data_type_id), 
				is_bool_annum(is_annun)
			{
				
			}
			[[nodiscard]] double value() const { return value_; }

		private:
			void update_new_value_from_xplane();
			void set_new_xplane_value(common::var_t val)
			{
				LOG() << "Set Value: double NOT IMPLEMENTED ";
			}
			[[nodiscard]] std::optional<common::var_t> get_changed_value();
			[[nodiscard]] common::var_t get_new_value() { return new_value_; }

			double value_{ -999999.0 };
			double new_value_{ -999999.0 };
			void* xplane_data_ref_{ nullptr };
			int xp_data_type_id_{ 0 };
			bool is_bool_annum{false};
			friend class XPDataRef;
		};

		class VectorIntXPDataRef
		{
		public:
			explicit VectorIntXPDataRef() = default;
			explicit VectorIntXPDataRef(void* xplane_data_ref, int data_type_id, bool is_annun) :
				xplane_data_ref_(xplane_data_ref), 
				xp_data_type_id_(data_type_id),
				is_bool_annum(is_annun)
			{
				
			}

			[[nodiscard]] std::vector<int>& value() { return value_; }

		//private:
			void update_new_value_from_xplane();
			void set_new_xplane_value(common::var_t val)
			{
				LOG() << "Set Value: Vector Int NOT IMPLEMENTED";
			}
			[[nodiscard]] std::optional<common::var_t> get_changed_value();
			[[nodiscard]] common::var_t get_new_value() { return new_value_; }

			std::vector<int> value_;
			std::vector<int> new_value_;
			void* xplane_data_ref_{ nullptr };
			int size_{ 0 };
			int xp_data_type_id_{ 0 };
			bool is_bool_annum{false};
			friend class XPDataRef;
		};


		class VectorFloatXPDataRef
		{
		public:
			explicit VectorFloatXPDataRef() = default;
			explicit VectorFloatXPDataRef(void* xplane_data_ref, int data_type_id, bool is_annun) : 
				xplane_data_ref_(xplane_data_ref), 
				xp_data_type_id_(data_type_id),
				is_bool_annum(is_annun)
			{
				
			}

			[[nodiscard]] std::vector<float>& value() { return value_; }

		private:
			void update_new_value_from_xplane();
			void set_new_xplane_value(common::var_t val)
			{
				LOG() << "Set Value: Vector Float NOT IMPLEMENTED";
			}
			[[nodiscard]] std::optional<common::var_t> get_changed_value();
			[[nodiscard]] common::var_t get_new_value() { return new_value_; }

			std::vector<float> value_;
			std::vector<float> new_value_;
			void* xplane_data_ref_{ nullptr };
			int size_{ 0 };
			int xp_data_type_id_{ 0 };
			bool is_bool_annum{false};
			friend class XPDataRef;
		};


		class VectorCharXPDataRef
		{
		public:
			explicit VectorCharXPDataRef() = default;
			explicit VectorCharXPDataRef(void* xplane_data_ref, int data_type_id, bool is_annum) : 
				xplane_data_ref_(xplane_data_ref), 
				xp_data_type_id_(data_type_id),
				is_bool_annum(is_annum)
			{
				
			}

			[[nodiscard]] std::vector<char>& value() { return value_; }

		private:
			void update_new_value_from_xplane();
			void set_new_xplane_value(common::var_t val)
			{
				LOG() << "Set Value: Vector Char NOT IMPLEMENTED";
			}
			[[nodiscard]] std::optional<common::var_t> get_changed_value();
			[[nodiscard]] common::var_t get_new_value() { return new_value_; }

			std::vector<char> value_;
			std::vector<char> new_value_;
			void* xplane_data_ref_{ nullptr };
			int size_{ 0 };
			int xp_data_type_id_{ 0 };
			bool is_bool_annum{false};
			friend class XPDataRef;
		};

		class InvalidXPDataRef
		{
			[[nodiscard]] std::optional<common::var_t> get_changed_value() { return {}; }
			[[nodiscard]] common::var_t get_new_value() { return 0; }
			void update_new_value_from_xplane(bool is_bool=false) {}
			void set_new_xplane_value(common::var_t val)
			{
				LOG() << "Set Value: invalid value";
			}
			friend class XPDataRef;
		};

		// ***********************************************************************************************
		//
		//                                   D A T A R E F S
		//
		// ***********************************************************************************************


		using var_class_t = std::variant<InvalidXPDataRef, IntXPDataRef, FloatXPDataRef, DoubleXPDataRef, VectorIntXPDataRef, VectorFloatXPDataRef, VectorCharXPDataRef>;


		class XPDataRef
		{
		public:
			//XPDataRef() = delete;
			//XPDataRef(const XPDataRef&) = delete;

			XPDataRef(std::string name, common::XplaneType requested_type, bool is_annum);


			[[nodiscard]] var_class_t variant() const { return xp_dataref_; }

			int decrement_subscribers();
			void increment_subscribers() { subscriber_count_ += 1; }

			[[nodiscard]] static std::unordered_map<int, common::var_t> get_new_values(std::unordered_map<int, std::unique_ptr<XPDataRef>>& data_refs);
			[[nodiscard]] static std::unordered_map<int, var_t> get_new_values_for_requested(const std::vector<int>& requested_id, std::unordered_map<int, std::unique_ptr<XPDataRef>>& data_refs);

			[[nodiscard]] static std::unordered_map<int, common::var_t> changed_values(std::unordered_map<int, std::unique_ptr<XPDataRef>>& data_refs);
			[[nodiscard]] static std::unordered_map<int, common::var_t> values_for_requested_ids(const std::vector<int>& requested_id, std::unordered_map<int, std::unique_ptr<XPDataRef>>& data_refs);


			static void update_new_values(std::unordered_map<int, std::unique_ptr<XPDataRef>>& data_refs);
			static void update_values_for_requested_ids(const std::vector<int>& requested_id, std::unordered_map<int, std::unique_ptr<XPDataRef>>& data_refs);
			static void set_values_for_datarefs(const std::vector<std::tuple<int, var_t>>& values, std::unordered_map<int, std::unique_ptr<XPDataRef>>& data_refs);


			[[nodiscard]] XplaneType type() const { return type_; }
			[[nodiscard]] std::string name() const { return name_; }

		private:
			void create_data_ref();

			std::string name_;
			XplaneType type_{common::XplaneType::type_Unknown};
			var_class_t xp_dataref_{};
			bool value_changed_{ false };
			int subscriber_count_{ 1 };
			bool is_annun_{false};
		};
}