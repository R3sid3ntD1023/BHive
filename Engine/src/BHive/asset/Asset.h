#pragma once

#include "AssetHandle.h"
#include "AssetType.h"
#include "core/Core.h"
#include "reflection/Reflection.h"
#include "serialization/Serialization.h"
#include "TAssetHandler.h"

namespace BHive
{

	class BHIVE Asset
	{
	public:
		Asset() = default;

		virtual ~Asset() = default;

		virtual void Save(cereal::BinaryOutputArchive &ar) const;

		virtual void Load(cereal::BinaryInputArchive &ar);

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
