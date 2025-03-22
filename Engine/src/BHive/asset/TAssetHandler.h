#pragma once

#include "asset/AssetManager.h"
#include "core/Core.h"
#include "core/reflection/Reflection.h"

namespace BHive
{
	template <typename T>
	struct TAssetHandle
	{
		TAssetHandle(const Ref<T> &ptr)
			: mPtr(const_cast<Ref<T> &>(ptr))
		{
		}

		TAssetHandle(Ref<T> &ptr)
			: mPtr(ptr)
		{
		}

		TAssetHandle &operator=(Ref<T> &ptr)
		{
			mPtr = ptr;
			return *this;
		}

		TAssetHandle &operator=(const TAssetHandle &rhs)
		{
			mPtr = rhs.mPtr;
			return *this;
		}

		template <typename A>
		inline std::string SaveMinimal(const A &ar) const
		{
			return mPtr ? mPtr->GetHandle() : UUID(NullID);
		}

		template <typename A>
		inline void LoadMinimal(const A &ar, const std::string &value)
		{
			mPtr = AssetManager::GetAsset<T>(value);
		}

	private:
		Ref<T> &mPtr;
	};

} // namespace BHive
