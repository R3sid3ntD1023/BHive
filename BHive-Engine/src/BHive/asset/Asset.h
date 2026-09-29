#pragma once

#include "AssetHandle.h"
#include "core/Core.h"
#include "core/UUID.h"

namespace BHive
{
	class BHIVE_API Asset
	{
	public:
		Asset() = default;

		virtual ~Asset() = default;

		virtual void Save(cereal::BinaryOutputArchive &ar) const;

		virtual void Load(cereal::BinaryInputArchive &ar);

		const UUID &GetHandle() const { return mHandle; }

		static UUID GetHandle(const Ref<Asset> &asset) { return asset ? asset->GetHandle() : NullID; }

		REFLECTABLEV()

	private:
		UUID mHandle;
	};

	REFLECT_EXTERN(Asset)

} // namespace BHive
