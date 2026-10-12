#pragma once

#include "asset/AssetManager.h"
#include "core/UUID.h"

namespace BHive
{
	template <typename T>
	struct AssetHandle
	{
		AssetHandle() = default;

		AssetHandle(UUID id)
			: mID(id)
		{
		}

		AssetHandle(Ref<T> ref)
			: mRef(ref)
		{
		}

		template <typename U>
			requires(std::is_base_of_v<T, U>)
		AssetHandle(Ref<U> ref)
			: mRef(ref)
		{
		}

		Ref<T> Get() const { return mRef ? mRef : AssetManager::GetAsset<T>(mID); }

		bool IsValid() const { return mRef ? true : AssetManager::IsAssetHandleValid(mID); }

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
		std::shared_ptr<T> mRef;
	};

} // namespace BHive
