#pragma once

#include "inspector/PropertyLayout.h"
#include "core/reflection/Reflection.h"

namespace BHive
{
	struct Inspector
	{
		using meta_getter = std::function<rttr::variant(const rttr::variant &)>;
		virtual bool Inspect(rttr::variant &var, bool read_only, const meta_getter &get_meta_data = {}) = 0;

		void BeginInspect(const rttr::property &poperty, bool columns = true, float width = 0.0f);
		void EndInspect(const rttr::property &property);

		REFLECTABLEV()

	private:
		PropertyLayout mLayout;
	};

	REFLECT(Inspector)
	{
		auto Factory = rttr::registration::class_<Inspector>("Inspector");
	}
} // namespace BHive

#define INSPECTED_TYPE_VAR "InspectedType"

#define DECLARE_INSPECTOR(cls)                                                                                               \
	struct Inspector_##cls : public Inspector                                                                                \
	{                                                                                                                        \
		virtual bool Inspect(rttr::variant &var, bool read_only, const Inspector::meta_getter &get_meta_data = {}) override; \
		REFLECTABLEV(Inspector)                                                                                              \
	};

#define REFLECT_INSPECTOR(cls, _type)                                                                                      \
	REFLECT(cls)                                                                                                           \
	{                                                                                                                      \
		auto Factory = rttr::registration::class_<cls>(#cls)(rttr::metadata(INSPECTED_TYPE_VAR, rttr::type::get<_type>())) \
						   .constructor()(rttr::policy::ctor::as_std_shared_ptr);                                          \
	}
