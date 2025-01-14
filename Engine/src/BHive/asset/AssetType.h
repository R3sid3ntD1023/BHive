#pragma once

#include <stdint.h>
#include "reflection/Reflection.h"

namespace BHive
{
	using AssetType = rttr::type;
	static const AssetType InvalidType = rttr::type::get<rttr::detail::invalid_type>();
} // namespace BHive
