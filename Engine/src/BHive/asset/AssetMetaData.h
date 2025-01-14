#pragma once

#include "core/Core.h"
#include "AssetType.h"
#include "serialization/Serialization.h"

namespace BHive
{

	struct BHIVE FAssetMetaData
	{
		AssetType Type = InvalidType;

		std::filesystem::path Path;

		std::string Name;

		operator bool() const { return Type != InvalidType; }

		void Serialize(StreamWriter &ar) const;
		void Deserialize(StreamReader &ar);

		REFLECTABLE()
	};

	REFLECT_EXTERN(FAssetMetaData)

}