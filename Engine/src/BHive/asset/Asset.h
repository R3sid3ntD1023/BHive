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
		

		Asset() = default;
		
		virtual ~Asset() = default;

		virtual AssetType GetType() const = 0;

		virtual void Serialize(StreamWriter &ar) const 
		{
			ar(mHandle, mName);
		};

		virtual void Deserialize(StreamReader &ar) 
		{
			ar(mHandle, mName);
		};

		void SetName(const std::string& name)
		{
			mName = name;
		}

		const std::string& GetName() const { return mName; }

		static AssetHandle GetHandle(const Ref<Asset> &asset)
		{
			return asset ? asset->GetHandle() : AssetHandle(0);
		}

		const AssetHandle& GetHandle() const
		{
			return mHandle;
		}

		REFLECTABLEV()

	private:
		std::string mName;
		AssetHandle mHandle;
	};

}
