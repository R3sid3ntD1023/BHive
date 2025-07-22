#pragma once

#include "Inspect.h"

namespace BHive
{
	template <typename T>
	inline bool Inspect::inspect(
		const std::string &label, T &obj, bool skip_custom, bool read_only, float width,
		const Inspector::meta_getter &get_meta_data)
	{
		ScopedPropertyLayout layout(label, true, width);
		rttr::variant var = obj;
		bool changed = inspect({}, var, skip_custom, read_only, width, get_meta_data);

		if (changed)
		{
			obj = var.get_value<T>();
		}

		return changed;
	}

	template <typename T>
	inline bool Inspect::inspect(
		const std::string &label, const T &obj, bool skip_custom, bool read_only, float width,
		const Inspector::meta_getter &get_meta_data)
	{
		ScopedPropertyLayout layout(label);
		rttr::variant var = obj;
		return inspect({}, var, skip_custom, read_only, width, get_meta_data);
	}
} // namespace BHive