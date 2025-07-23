#pragma once

#include "Inspector.h"

namespace BHive
{
	struct BHIVE_API Inspect
	{
		static rttr::variant meta_data_empty(const rttr::variant &);

		static bool inspect(
			const rttr::variant &instance, rttr::variant &var, bool skip_custom = false, bool read_only = false,
			float width = 0.0f, const Inspector::meta_getter &get_meta_data = meta_data_empty);

		static bool inspect(
			const rttr::variant &instance, rttr::instance &object, rttr::property &property, bool read_only = false,
			float width = 0.0f);

		template <typename T>
		static bool inspect(
			const std::string &label, T &obj, bool skip_custom = false, bool read_only = false, float width = 0.0f,
			const Inspector::meta_getter &get_meta_data = meta_data_empty);

		template <typename T>
		static bool inspect(
			const std::string &label, const T &obj, bool skip_custom = false, bool read_only = false, float width = 0.0f,
			const Inspector::meta_getter &get_meta_data = meta_data_empty);
	};

} // namespace BHive

#include "Inspect.inl"