#pragma once

#include "core/Core.h"
#include "ResourceHandle.h"
#include "IResourceRegistry.h"

namespace BHive
{

	template <typename T>
	class ResourceRegistry : public IResourceRegistry
	{
	public:
		using Handle = ResourceHandle;

		struct Entry
		{
			Ref<T> Resource;
			uint32_t Generation;
		};

		template <typename U, typename... TArgs>
			requires(std::is_base_of_v<T, U>)
		Handle CreateResource(TArgs &&...args)
		{
			uint32_t index;
			if (!mFreeList.empty())
			{
				index = mFreeList.back();
				mFreeList.pop_back();
				goto init;
			}

			index = mEntries.size();
			mEntries.emplace_back();

		init:
			auto &e = mEntries[index];
			e.Resource = CreateRef<U>(std::forward<TArgs>(args)...);
			e.Generation++;

			return Handle{index, e.Generation, typeid(T).hash_code(), e.Resource.get()};
		}

		void Destroy(Handle h)
		{
			if (!IsValid(h))
				return;

			auto &slot = mEntries[h.Index];
			slot.Generation++;
			slot.Resource.reset();
			h.Ptr = nullptr;
			mFreeList.push_back(h.Index);
		}

		void *GetRaw(Handle h) const
		{
			if (!IsValid(h))
				return nullptr;

			auto &slot = mEntries[h.Index];
			return slot.Resource.get();
		}

		bool IsValid(Handle h) const
		{
			if (!h.IsValid())
			{
				LOG_WARN("Invalid handle {}", typeid(T).name());
				return false;
			}

			if (h.Type != typeid(T).hash_code())
			{
				LOG_WARN("Invalid Type for registry {}", typeid(T).name());
				return false;
			}

			if (h.Index >= mEntries.size())
				return false;

			return true;
		}

	private:
		std::vector<Entry> mEntries;
		std::vector<uint32_t> mFreeList;
	};
} // namespace BHive
