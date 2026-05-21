#pragma once

#include "VulkanMemory.h"

namespace BHive
{
	
	class GPUResourceManager
	{
	public:
		template<typename T>
		struct Resource
		{
			T Handle = VK_NULL_HANDLE;
		};

		struct StorageBase
		{
			virtual void Remove(const ResourceID& handle) = 0;
		};

		template<typename T>
		struct Storage : public StorageBase
		{
			using TContainer = std::unordered_map<ResourceID, Resource<T>>;

			void AddExternal(const ResourceID &handle, T && resource) 
			{
				if (mResources.contains(handle))
				{
					LOG_WARN("Handle already exists! -> {}", handle);
					return;
				}

				mResources.emplace(handle, Resource<T>{.Handle = std::move(resource)}); 
			}

			void Remove(const ResourceID &handle) override
			{ 
				if (!mResources.contains(handle))
				{
					LOG_WARN("Handle does not exists! -> {}", handle);
					return;
				}

				if(mResources.contains(handle))
				{
					mResources.erase(handle);
				}
			}

			T& GetOrCreate(const ResourceID& handle) { return mResources.try_emplace(handle).first->second.Handle;}

			T &Get(const ResourceID &handle)
			{
				ASSERT(mResources.contains(handle), "Invalid resource id -> {}", handle)
				return mResources.at(handle).Handle;
			}

			const T& Get(const ResourceID& handle) const
			{
				ASSERT(mResources.contains(handle), "Invalid resource id -> {}", handle)
				return mResources.at(handle).Handle;
			}


			TContainer &GetContainer() { return mResources; }

		private:
			TContainer mResources;
		};

		ResourceID CreateBuffer(const vk::BufferCreateInfo &info, vk::MemoryPropertyFlags flags, size_t reqSize, const std::string &name = "");

		ResourceID CreateImage(const vk::ImageCreateInfo &info, vk::MemoryPropertyFlags flags, size_t reqSize, const std::string &name = "");

		ResourceID RegisterExternalImage(const vk::Image &image, const std::string &name = "");

		ResourceID CreateImageView(const vk::ImageViewCreateInfo &info, const std::string &name = "");

		ResourceID CreateSampler(const vk::SamplerCreateInfo &info, const std::string &name = "");

		void* MapMemory(ResourceID &buffer, vk::DeviceSize offset, vk::DeviceSize size);

		void UnmapMemory(ResourceID &buffer);

		void DestroyBuffer(const ResourceID& handle);

		void DestroyImage(const ResourceID &handle);

		void DestroyImageView(const ResourceID &handle);

		void DestroySampler(const ResourceID &handle);

		void DestroyBuffer(AllocatedBuffer buffer);

		void DestroyImage(GPUImage& image);
		//-------------------getters------------------------------

		const vk::Image& GetImage(const ResourceID &handle);

		const vk::ImageView& GetImageView(const ResourceID &handle);

		const vk::Sampler& GetSampler(const ResourceID &handle);

		const vk::Buffer& GetBuffer(const ResourceID &handle);

		template<typename T>
		Storage<T>& GetStorage()
		{
			size_t id = typeid(T).hash_code();
			if (mStorages.contains(id))
				return static_cast<Storage<T> &>(*mStorages.at(id).get());

			mStorages.emplace(id, std::move(CreateScope<Storage<T>>()));
			return static_cast<Storage<T> &>(*mStorages.at(id).get());
		}

		class IDPool
		{
		public:
			uint32_t Aquire()
			{
				if (!mFreeList.empty())
				{
					uint32_t id = mFreeList.back();
					mFreeList.pop_back();
					return id;
				}

				return mCounter.fetch_add(1, std::memory_order_relaxed);
			}

			void Release(const uint32_t &id)
			{
				mFreeList.push_back(id);
			}

		private:
			std::atomic<uint32_t> mCounter{1};
			std::vector<uint32_t> mFreeList;
		};

	private:
		std::unordered_map<size_t, Scope<StorageBase>> mStorages;
		std::unordered_set<ResourceID> mExternalImages;
		IDPool mIDPool;
	};
} // namespace BHive