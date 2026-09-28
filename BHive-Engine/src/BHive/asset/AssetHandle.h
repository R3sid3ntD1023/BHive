#pragma once

#include "asset/AssetManager.h"
#include "core/UUID.h"

namespace BHive
{
	template <typename T>
	struct BHIVE_API AssetHandle
	{
		AssetHandle() = default;

		AssetHandle(UUID id)
			: mID(id)
		{
		}

		Ref<T> Get() const { return AssetManager::GetAsset<T>(mID); }

		bool IsValid() const { return AssetManager::IsAssetHandleValid(mID); }

		template <typename A>
		inline std::string SaveMinimal(const A &ar) const
		{
			return mID.ToString();
		}

		template <typename A>
		inline void LoadMinimal(const A &ar, const std::string &value)
		{
			mID.FromString(value);
		}

		operator bool() const { return IsValid(); }

		Ref<T> operator->() const { return Get(); }

	private:
		UUID mID{NullID};
	};

} // namespace BHive
