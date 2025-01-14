#pragma once

#include "core/Core.h"
#include "AssetType.h"

namespace BHive
{

	struct BHIVE FAssetMetaData
	{
		AssetType Type = InvalidType;

		std::filesystem::path Path;

		std::string Name;

		operator bool() const { return Type != InvalidType; }

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