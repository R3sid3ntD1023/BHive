#pragma once

#include <cstdint>
#include <optional>
#include <queue>
#include <utility>
#include <vector>

namespace BHive
{
	template <typename T>
	class VulkanResourceList
	{
	public:
		template <typename... TArgs>
		int32_t Create(TArgs &&...args)
		{
			uint32_t id;
			if (!mFreeList.empty())
			{
				id = mFreeList.front();
				mFreeList.pop();
				mResources[id].emplace(std::forward<TArgs>(args)...);
			}
			else
			{
				id = static_cast<uint32_t>(mResources.size());
				mResources.emplace_back(std::in_place, std::forward<TArgs>(args)...);
			}

			return static_cast<int32_t>(id);
		}

		void Delete(int32_t &id)
		{
			if (!Get(id))
				return;

			mDeletionQueue.push(static_cast<uint32_t>(id));
			id = -1;
		}

		T *Get(int32_t id)
		{
			if (id < 0 || static_cast<size_t>(id) >= mResources.size() || !mResources[id])
				return nullptr;

			return &*mResources[id];
		}

		bool HasPendingDeletions() const { return !mDeletionQueue.empty(); }

		void ProcessDeletions()
		{
			while (!mDeletionQueue.empty())
			{
				auto id = mDeletionQueue.front();
				mDeletionQueue.pop();

				if (id >= mResources.size() || !mResources[id])
					continue;

				mResources[id].reset();
				mFreeList.push(id);
			}
		}

		void Clear()
		{
			while (!mDeletionQueue.empty())
				mDeletionQueue.pop();
			while (!mFreeList.empty())
				mFreeList.pop();
			mResources.clear();
		}

	private:
		std::vector<std::optional<T>> mResources;
		std::queue<uint32_t> mFreeList;
		std::queue<uint32_t> mDeletionQueue;
	};
} // namespace BHive