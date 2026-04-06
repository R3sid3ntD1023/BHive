#pragma once

#include "core/UUID.h"
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
			virtual void Remove(const UUID& handle) = 0;
		};

		template<typename T>
		struct Storage : public StorageBase
		{
			using TContainer = std::unordered_map<UUID, Resource<T>>;

			void Remove(const UUID &handle) override
			{ 
				if(mResources.contains(handle))
				{
					mResources.erase(handle);
				}
			}

			T& GetOrCreate(const UUID& handle) { return mResources.try_emplace(handle).first->second.Handle;}

			T &Get(const UUID &handle)
			{
				ASSERT(mResources.contains(handle))
				return mResources.at(handle).Handle;
			}

			const T& Get(const UUID& handle) const
			{
				ASSERT(mResources.contains(handle))
				return mResources.at(handle).Handle;
			}


			TContainer &GetContainer() { return mResources; }

		private:
			TContainer mResources;
		};

		AllocatedBuffer CreateBuffer(const BufferDesc& desc);

		AllocatedImage CreateImage(const ImageDesc &desc, const ImageViewDesc &viewDesc);

		void CreateImageView(AllocatedImage &image, const ImageViewDesc &desc);

		UUID CreateImageView(const vk::Image &image, const ImageViewDesc &desc);

		void* MapMemory(AllocatedBuffer &buffer, vk::DeviceSize offset, vk::DeviceSize size);

		void UnmapMemory(AllocatedBuffer &buffer);

		void CreateSampler(AllocatedImage& image, const vk::SamplerCreateInfo &create_info);

		void DestroyBuffer(const UUID& handle);

		void DestroyImage(const UUID &handle);

		void DestroyImageView(const UUID &handle);

		void DestroySampler(const UUID &handle);

		void DestroyBuffer(AllocatedBuffer buffer);

		void DestroyImage(AllocatedImage image);

		void DestroyImage(Image image);

		//-------------------getters------------------------------

		const vk::Image& GetImage(const UUID &handle);

		const vk::ImageView& GetImageView(const UUID &handle);

		const vk::Sampler& GetSampler(const UUID &handle);

		const vk::Buffer& GetBuffer(const UUID &handle);

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
	};
} // namespace BHive