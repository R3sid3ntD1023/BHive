#pragma once

#include <cereal/cereal.hpp>
#include "gfx/Color.h"

namespace BHive
{
	template <typename A>
	inline void Serialize(const A &ar, const Color &col)
	{
		return col.to_string();
	}

	template <typename A>
	inline void CEREAL_LOAD_MINIMAL_FUNCTION_NAME(const A &ar, Color &col, const std::string &value)
	{
		std::for
		std::stringstream ss(value);
		char token;
		ss >> token >> col.r >> token >> col.g >> token >> col.b >> token >> col.a >> token;
	}
} // namespace BHive
