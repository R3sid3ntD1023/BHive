#pragma once

#include "IResourceRegistry.h"
#include "ResourceTypes.h"
#include "core/Core.h"

namespace BHive
{
	template <typename T>
	class ResourceRegistry : public IResourceRegistry
	{
	public:
		struct Entry
		{
			Ref<T> Resource;
			uint32_t Generation;
		};

		template <typename U, typename... TArgs>
			requires(std::is_base_of_v<T, U>)
		ResourceHandle CreateResource(TArgs &&...args)
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

			return ResourceHandle{index, e.Generation, TypeID<T>::value};
		}

		void Destroy(ResourceHandle h)
		{
			if (!IsValid(h))
				return;

			auto &slot = mEntries[h.Index];
			slot.Generation++;
			slot.Resource.reset();
			mFreeList.push_back(h.Index);
		}

		void *GetRaw(ResourceHandle h) const
		{
			if (!IsValid(h))
				return nullptr;

			auto &slot = mEntries[h.Index];
			return slot.Resource.get();
		}

		bool IsValid(ResourceHandle h) const
		{
			if (h.Index >= mEntries.size())
				return false;

			auto &slot = mEntries[h.Index];
			if (slot.Generation != h.Generation)
				return false;

			return true;
		}

	private:
		std::vector<Entry> mEntries;
		std::vector<uint32_t> mFreeList;
	};
} // namespace BHive
