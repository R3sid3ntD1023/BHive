#pragma once

#include "core/UUID.h"

namespace BHive
{

	template <typename TArchive>
	void Serialize(TArchive& ar, const UUID& id)
	{
		ar((uint64_t)id);
	}

	template <typename TArchive>
	void Deserialize(TArchive& ar, UUID& id)
	{
		uint64_t value;
		ar(value);
	}

}