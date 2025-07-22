#pragma once

#include "Inspector.h"

namespace BHive
{
	struct Inspector_String : public Inspector
	{
		virtual bool Inspect(
			const rttr::variant &instance, rttr::variant &var, bool read_only,
			const Inspector::meta_getter &get_meta_data = {}) override;

		REFLECTABLEV(Inspector)
	};

	struct Inspector_Float : public Inspector
	{
		virtual bool Inspect(
			const rttr::variant &instance, rttr::variant &var, bool read_only,
			const Inspector::meta_getter &get_meta_data = {}) override;

		REFLECTABLEV(Inspector)
	};

	struct Inspector_Bool : public Inspector
	{
		virtual bool Inspect(
			const rttr::variant &instance, rttr::variant &var, bool read_only,
			const Inspector::meta_getter &get_meta_data = {}) override;

		REFLECTABLEV(Inspector)
	};

	template <typename TIntegerType>
	struct Inspector_Int : public Inspector
	{
		virtual bool Inspect(
			const rttr::variant &instance, rttr::variant &var, bool read_only,
			const Inspector::meta_getter &get_meta_data = {}) override;

		REFLECTABLEV(Inspector)
	};

} // namespace BHive
