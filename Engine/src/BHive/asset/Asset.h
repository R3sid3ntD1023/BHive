#pragma once

#include "core/Core.h"
#include "AssetHandle.h"
#include "TAssetHandler.h"
#include "AssetType.h"
#include "reflection/Reflection.h"
#include "serialization/Serialization.h"

namespace BHive
{

	class BHIVE Asset
	{
	public:
		Asset() = default;

		virtual ~Asset() = default;

		virtual void Save(cereal::JSONOutputArchive &ar) const;

		virtual void Load(cereal::JSONInputArchive &ar);

		void SetName(const std::string &name) { mName = name; }

		const std::string &GetName() const { return mName; }

		static AssetHandle GetHandle(const Ref<Asset> &asset)
		{
			return asset ? asset->GetHandle() : AssetHandle(0);
		}

		const AssetHandle &GetHandle() const { return mHandle; }

		REFLECTABLEV()

	private:
		std::string mName;
		AssetHandle mHandle;
	};

} // namespace BHive
