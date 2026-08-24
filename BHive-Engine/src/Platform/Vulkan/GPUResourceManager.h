#pragma once

#include "core/Core.h"
#include "VulkanMemory.h"

namespace BHive
{
	struct StorageBase
	{
		virtual ~StorageBase() = default;

		virtual void Remove(ResourceID handle) = 0;

		virtual bool Contains(ResourceID handle) const = 0;

		virtual void Clear() = 0;

		virtual size_t Size() const = 0;
	};

	template <typename T>
	struct Storage : public StorageBase
	{
		using TContainer = std::unordered_map<ResourceID, T>;

		~Storage() { Clear(); }

		void AddExternal(ResourceID handle, T &&resource)
		{
			if (!Contains(handle))
			{
				mResources[handle] = std::move(resource);
				return;
			}
		}

		void Remove(ResourceID handle) override
		{
			if (Contains(handle))
			{
				mResources.erase(handle);
				return;
			}
		}

		bool Contains(ResourceID handle) const override { return mResources.contains(handle); }

		T &Create(ResourceID handle)
		{
			ASSERT(!Contains(handle), "handle already exists {}", handle)
			mResources.emplace(handle, VK_NULL_HANDLE);
			return mResources.at(handle);
		}

		T &Get(ResourceID handle)
		{
			ASSERT(Contains(handle), "Invalid resource id -> {}", handle)
			return mResources.at(handle);
		}

		const T &Get(ResourceID handle) const
		{
			ASSERT(Contains(handle), "Invalid resource id -> {}", handle)
			return mResources.at(handle);
		}

		TContainer &GetContainer() { return mResources; }

		size_t Size() const override { return mResources.size(); }

		void Clear() override { mResources.clear(); }

	private:
		TContainer mResources{};
	};

	class GPUResourceManager
	{
	public:
		~GPUResourceManager();

		void Shutdown();

		ResourceID CreateBuffer(const vk::BufferCreateInfo &info, vk::MemoryPropertyFlags flags, const std::string &name = "");

		ResourceID CreateImage(const vk::ImageCreateInfo &info, vk::MemoryPropertyFlags flags, const std::string &name = "");

		ResourceID RegisterExternalImage(const vk::Image &image, const std::string &name = "");

		ResourceID CreateImageView(const vk::ImageViewCreateInfo &info, const std::string &name = "");

		ResourceID CreateSampler(const vk::SamplerCreateInfo &info, const std::string &name = "");

		void *MapMemory(ResourceID buffer, vk::DeviceSize offset, vk::DeviceSize size);

		void UnmapMemory(ResourceID buffer);

		void DestroyBuffer(ResourceID handle);

		void DestroyImage(ResourceID handle);

		void DestroyImageView(ResourceID handle);

		void DestroySampler(ResourceID handle);

		void DestroyBuffer(AllocatedBuffer buffer);

		void DestroyImage(GPUImage &image);
		//-------------------getters------------------------------

		vk::Image GetImage(ResourceID handle);

		vk::ImageView GetImageView(ResourceID handle);

		vk::Sampler GetSampler(ResourceID handle);

		vk::Buffer GetBuffer(ResourceID handle);

		template <typename T>
		Storage<T> &GetStorage()
		{
			auto id = typeid(T).name();
			if (!mStorages.contains(id))
				mStorages.emplace(id, CreateRef<Storage<T>>());

			return *std::dynamic_pointer_cast<Storage<T>>(mStorages.at(id));
		}

	private:
		std::unordered_map<const char *, Ref<StorageBase>> mStorages;
		std::unordered_set<ResourceID> mExternalImages;
	};
} // namespace BHive