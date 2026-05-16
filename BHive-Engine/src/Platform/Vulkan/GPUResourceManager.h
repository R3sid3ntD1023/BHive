#pragma once

#include "VulkanMemory.h"

namespace BHive
{


	struct BufferDesc
	{
		size_t Size = 0;

		vk::BufferUsageFlags Usage{};

		vk::MemoryPropertyFlags MemoryFlags{};
	};

	struct ImageDesc
	{
		vk::ImageCreateFlags Flags{};

		uint32_t Width = 0, Height = 0, Depth = 1;

		vk::ImageType Type{};

		vk::ImageTiling Tiling{};

		vk::Format Format{};

		vk::ImageUsageFlags Usage{};

		vk::MemoryPropertyFlags MemoryFlags{};

		vk::ImageAspectFlags Aspect;

		uint32_t ArrayLayers = 1;

		uint32_t BytesPerPixel = 4;

		uint32_t MipLevels = 1;

		std::string DebugName;

		uint32_t Size() const { return Width * Height * Depth * BytesPerPixel; }
	};

	struct ImageViewDesc
	{
		vk::ImageAspectFlags Aspect;

		vk::ImageViewType Type;

		vk::Format Format;

		uint32_t BaseArrayLayer = 0;

		uint32_t LayerCount = 1;

		uint32_t BaseMipLevel = 0;

		uint32_t LevelCount = 1;
	};

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
				mResources.emplace(handle, Resource<T>{.Handle = std::move(resource)}); 
			}

			void Remove(const ResourceID &handle) override
			{ 
				if(mResources.contains(handle))
				{
					mResources.erase(handle);
				}
			}

			T& GetOrCreate(const ResourceID& handle) { return mResources.try_emplace(handle).first->second.Handle;}

			T &Get(const ResourceID &handle)
			{
				ASSERT(mResources.contains(handle))
				return mResources.at(handle).Handle;
			}

			const T& Get(const ResourceID& handle) const
			{
				ASSERT(mResources.contains(handle))
				return mResources.at(handle).Handle;
			}


			TContainer &GetContainer() { return mResources; }

		private:
			TContainer mResources;
		};

		AllocatedBuffer CreateBuffer(const BufferDesc& desc);

		GPUImage CreateImage(const ImageDesc &desc);

		ResourceID RegisterExternalImage(const vk::Image &image);

		ResourceID CreateImageView(const vk::Image &image, const ImageViewDesc &desc);

		void* MapMemory(AllocatedBuffer &buffer, vk::DeviceSize offset, vk::DeviceSize size);

		void UnmapMemory(AllocatedBuffer &buffer);

		void CreateSampler(GPUImage& image, const vk::SamplerCreateInfo &create_info);

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

	private:
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