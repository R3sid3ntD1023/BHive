#pragma once

#include "AssetType.h"
#include "core/Core.h"
#include "core/serialization/Serialization.h"

namespace BHive
{

	struct BHIVE FAssetMetaData
	{
		AssetType Type = InvalidType;

		std::filesystem::path Path;

		std::string Name;

		operator bool() const { return Type != InvalidType; }

		template <typename A>
		inline void Serialize(A &ar)
		{
			ar(MAKE_NVP("Type", Type), MAKE_NVP("Path", Path), MAKE_NVP("Name", Name));
		}

		REFLECTABLE()
	};

	REFLECT_EXTERN(FAssetMetaData)

	REFLECT(FAssetMetaData)
	{
		BEGIN_REFLECT(FAssetMetaData)
		REFLECT_PROPERTY("Type", Type)
		REFLECT_PROPERTY("Path", Path)
		REFLECT_PROPERTY("Name", Name);
	}
} // namespace BHive