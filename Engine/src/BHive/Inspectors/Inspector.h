#pragma once

#include "PropertyLayout.h"
#include "core/reflection/Reflection.h"

namespace BHive
{
	using MetaGetter = std::function<rttr::variant(const rttr::variant &)>;

	struct Inspector
	{

		virtual bool Inspect(
			const rttr::variant &owner, rttr::variant &var, const MetaGetter &GetMetaData = {},
			const bool is_read_only = false) = 0;

		void BeginInspect(const rttr::property &poperty, bool columns = true, float width = 0.0f);

		void EndInspect(const rttr::property &property);

		virtual bool IsFinishedEditing() const { return true; }

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

#define REFLECT_INSPECTOR(cls, _type)                                                                                      \
	REFLECT(cls)                                                                                                           \
	{                                                                                                                      \
		auto Factory = rttr::registration::class_<cls>(#cls)(rttr::metadata(INSPECTED_TYPE_VAR, rttr::type::get<_type>())) \
						   .constructor()(rttr::policy::ctor::as_std_shared_ptr);                                          \
	}

#define INSPECTOR_BODY()                                                                    \
	bool Inspect(                                                                           \
		const rttr::variant &owner, rttr::variant &var, const MetaGetter &GetMetaData = {}, \
		const bool is_read_only = false) override;