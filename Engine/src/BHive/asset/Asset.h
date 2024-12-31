#pragma once

#include "core/Core.h"
#include "AssetHandle.h"
#include "TAssetHandler.h"
#include "AssetType.h"
#include "reflection/Reflection.h"
#include "serialization/Serialization.h"

#define STATIC_ASSET_TYPE(cls) AssetType::get<cls>();
#define ASSET_CLASS(cls)                                           \
public:                                                            \
	AssetType GetType() const override { return GetStaticType(); } \
	static AssetType GetStaticType() { return STATIC_ASSET_TYPE(cls); }

namespace BHive
{

	class BHIVE Asset
	{
	public:
		AssetHandle Handle;

		virtual ~Asset() = default;

		virtual AssetType GetType() const = 0;

		virtual void Serialize(StreamWriter &ar) const {};

		virtual void Deserialize(StreamReader &ar) {};

		static AssetHandle GetHandle(const Ref<Asset> &asset)
		{
			return asset ? asset->Handle : AssetHandle(0);
		}

		REFLECTABLEV()
	};

}
