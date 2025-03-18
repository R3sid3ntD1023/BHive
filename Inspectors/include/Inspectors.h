#pragma once

#include "Inspector.h"
#include "core/InspectorCore.h"

namespace BHive
{
	void INSPECTOR_API SetCurrentContext(ImGuiContext *ctx);

	rttr::variant INSPECTOR_API meta_data_empty(const rttr::variant &);

	bool INSPECTOR_API inspect(
		rttr::variant &var, bool skip_custom = false, bool read_only = false,
		const Inspector::meta_getter &get_meta_data = meta_data_empty, float width = 0.0f);

	bool INSPECTOR_API inspect(rttr::instance &object, rttr::property &property, bool read_only = false, float width = 0.0f);

	template <typename T>
	bool INSPECTOR_API inspect(
		const std::string &label, T &obj, bool skip_custom = false, bool read_only = false,
		const Inspector::meta_getter &get_meta_data = meta_data_empty, float width = 0.0f)
	{
		ScopedPropertyLayout layout(label, true, width);
		rttr::variant var = obj;
		bool changed = inspect(var, skip_custom, read_only, get_meta_data, width);

		if (changed)
		{
			obj = var.get_value<T>();
		}

		return changed;
	}

	template <typename T>
	bool INSPECTOR_API inspect(
		const std::string &label, const T &obj, bool skip_custom = false, bool read_only = false,
		const Inspector::meta_getter &get_meta_data = meta_data_empty, float width = 0.0f)
	{
		ScopedPropertyLayout layout(label);
		rttr::variant var = obj;
		return inspect(var, skip_custom, read_only, get_meta_data, width);
	}

} // namespace BHive
