#pragma once

#include "core/Core.h"
#include "core/UUID.h"
#include "TAssetHandler.h"

namespace BHive
{
	class BHIVE_API Asset
	{
	public:
		Asset() = default;

		virtual ~Asset() = default;

		virtual void Save(cereal::BinaryOutputArchive &ar) const;

		virtual void Load(cereal::BinaryInputArchive &ar);

		void SetName(const std::string &name) { mName = name; }

		const std::string &GetName() const { return mName; }

		static UUID GetHandle(const Ref<Asset> &asset) { return GetHandle(asset.get()); }

		static UUID GetHandle(const Asset *asset) { return asset ? asset->GetHandle() : NullID; }

		const UUID &GetHandle() const { return mHandle; }

		REFLECTABLEV()

	private:
		std::string mName;
		UUID mHandle;
	};

	REFLECT_EXTERN(Asset)

} // namespace BHive
