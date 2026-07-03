#pragma once

#include "core/Core.h"
#include "VulkanMemory.h"

namespace BHive
{
	template <typename T>
	struct Resource
	{
		T Handle = VK_NULL_HANDLE;
	};

	struct StorageBase
	{
		virtual ~StorageBase() = default;

		virtual void Remove(ResourceID handle) = 0;
	};

	template <typename T>
	struct Storage : public StorageBase
	{
		using TContainer = std::unordered_map<ResourceID, Resource<T>>;

		void AddExternal(ResourceID handle, T &&resource)
		{
			if (!mResources.contains(handle))
			{
				mResources[handle] = Resource<T>{.Handle = std::move(resource)};
				return;
			}

			LOG_WARN("Handle already exists! -> {}", handle);
		}

		void Remove(ResourceID handle) override
		{
			if (mResources.contains(handle))
			{
				mResources.erase(handle);
				return;
			}

			LOG_WARN("Handle does not exists! -> {}", handle);
		}

		T &GetOrCreate(ResourceID handle) { return mResources.try_emplace(handle).first->second.Handle; }

		T &Get(ResourceID handle)
		{
			ASSERT(mResources.contains(handle), "Invalid resource id -> {}", handle)
			return mResources.at(handle).Handle;
		}

		const T &Get(ResourceID handle) const
		{
			ASSERT(mResources.contains(handle), "Invalid resource id -> {}", handle)
			return mResources.at(handle).Handle;
		}

		TContainer &GetContainer() { return mResources; }

	private:
		TContainer mResources;
	};

	class GPUResourceManager
	{
	public:

		ResourceID CreateBuffer(const vk::BufferCreateInfo &info, vk::MemoryPropertyFlags flags, const std::string &name = "");

		ResourceID CreateImage(const vk::ImageCreateInfo &info, vk::MemoryPropertyFlags flags, const std::string &name = "");

		ResourceID RegisterExternalImage(const vk::Image &image, const std::string &name = "");

		ResourceID CreateImageView(const vk::ImageViewCreateInfo &info, const std::string &name = "");

		ResourceID CreateSampler(const vk::SamplerCreateInfo &info, const std::string &name = "");

		void* MapMemory(ResourceID buffer, vk::DeviceSize offset, vk::DeviceSize size);

		void UnmapMemory(ResourceID buffer);

		void DestroyBuffer(ResourceID handle);

		void DestroyImage(ResourceID handle);

		void DestroyImageView(ResourceID handle);

		void DestroySampler(ResourceID handle);

		void DestroyBuffer(AllocatedBuffer buffer);

		void DestroyImage(GPUImage& image);
		//-------------------getters------------------------------

		vk::Image GetImage(ResourceID handle);

		vk::ImageView GetImageView(ResourceID handle);

		vk::Sampler GetSampler(ResourceID handle);

		vk::Buffer GetBuffer(ResourceID handle);

		template<typename T>
		Storage<T>& GetStorage()
		{
			size_t id = typeid(T).hash_code();
			if (mStorages.contains(id))
				return static_cast<Storage<T> &>(*mStorages.at(id).get());

			mStorages.emplace(id, std::move(CreateScope<Storage<T>>()));
			return static_cast<Storage<T> &>(*mStorages.at(id).get());
		}


	private:
		std::unordered_map<size_t, Scope<StorageBase>> mStorages;
		std::unordered_set<ResourceID> mExternalImages;
	};
} // namespace BHive