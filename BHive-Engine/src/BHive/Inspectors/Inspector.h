#pragma once

#include "core/Core.h"
#include "PropertyLayout.h"

namespace BHive
{
	using MetaGetter = std::function<rttr::variant(const rttr::variant &)>;

	struct Inspector
	{
		virtual ~Inspector() = default;

		virtual bool inspect(const rttr::variant &owner, rttr::variant &var, const MetaGetter &GetMetaData = {}, const bool is_read_only = false) = 0;

		void begin_inspect(const rttr::property &poperty, bool columns = true, float width = 0.0f);

		void end_inspect(const rttr::property &property);

		REFLECTABLEV()

	private:
		PropertyLayout mLayout;
	};

	REFLECT_INLINE(Inspector)
	{
		auto Factory = rttr::registration::class_<Inspector>("Inspector");
	}
} // namespace BHive

#define INSPECTED_TYPE_VAR "InspectedType"

#define REFLECT_INSPECTOR(cls, _type)                                                                                                                                            \
	REFLECT(cls)                                                                                                                                                                 \
	{                                                                                                                                                                            \
		auto Factory = rttr::registration::class_<cls>(#cls)(rttr::metadata(INSPECTED_TYPE_VAR, rttr::type::get<_type>())).constructor()(rttr::policy::ctor::as_std_shared_ptr); \
	}

#define INSPECTOR_BODY() bool inspect(const rttr::variant &owner, rttr::variant &var, const MetaGetter &GetMetaData = {}, const bool is_read_only = false) override;