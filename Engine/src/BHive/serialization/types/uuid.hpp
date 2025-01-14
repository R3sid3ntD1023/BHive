#pragma once

#include <cereal/cereal.hpp>
#include "core/UUID.h"

namespace BHive
{

	template <typename A>
	inline uint64_t CEREAL_SAVE_MINIMAL_FUNCTION_NAME(const A &ar, const UUID &id)
	{
		return (uint64_t)id;
	}

	template <typename A>
	inline void CEREAL_LOAD_MINIMAL_FUNCTION_NAME(const A &ar, UUID &id, const uint64_t &value)
	{
		id = value;
	}

} // namespace BHive