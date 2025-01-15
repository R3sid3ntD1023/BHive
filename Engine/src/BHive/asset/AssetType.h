#pragma once

#include "reflection/Reflection.h"

namespace BHive
{
	using AssetType = rttr::type;
	static const AssetType InvalidType = rttr::type::get<rttr::detail::invalid_type>();

} // namespace BHive

namespace rttr
{
	template <typename A>
	inline std::string SaveMinimal(const A &ar, const rttr::type &obj)
	{
		return obj.get_name().data();
	}

	template <typename A>
	inline void LoadMinimal(const A &ar, rttr::type &obj, const std::string &value)
	{
		obj = rttr::type::get_by_name(value);
	}
} // namespace rttr