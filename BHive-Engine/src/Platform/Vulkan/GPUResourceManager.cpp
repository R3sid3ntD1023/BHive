#include "GPUResourceManager.h"
#include "VulkanUtils.h"

namespace BHive
{
	struct GPUBuffer
	{
		vk::raii::Buffer Buffer = VK_NULL_HANDLE;
		vk::raii::DeviceMemory Memory = VK_NULL_HANDLE;
		void *MappedMemory = nullptr;
	};

	struct GPUImage
	{
		vk::raii::Image Image = VK_NULL_HANDLE;
		vk::raii::ImageView View = VK_NULL_HANDLE;
		vk::raii::Sampler Sampler = VK_NULL_HANDLE;
		vk::raii::DeviceMemory Memory = VK_NULL_HANDLE;
	};

	struct GPUStorage
	{
		static inline std::unordered_map<BufferHandle, GPUBuffer> Buffers;
		static inline std::unordered_map<ImageHandle, GPUImage> Images;
	};

	void GPUResourceManager::Shutdown()
	{
		GPUStorage::Buffers.clear();
		GPUStorage::Images.clear();
	}

	Vulkan::AllocatedBuffer GPUResourceManager::CreateBuffer(const BufferDesc &desc)
	{
		auto handle = BufferHandle();
		auto& buffer = GPUStorage::Buffers[handle];
		VulkanUtils::CreateBuffer(desc.Size, desc.Usage, desc.MemoryFlags, buffer.Buffer, buffer.Memory);
		return {buffer.Buffer, buffer.Memory, handle};
	}

	Vulkan::AllocatedImage GPUResourceManager::CreateImage(const ImageDesc &desc)
	{
		auto handle = ImageHandle();
		auto &image = GPUStorage::Images[handle];
		VulkanUtils::CreateImage(desc.Width, desc.Height, desc.Depth, desc.ArrayLayers, desc.Type, desc.Format, desc.Tiling, desc.Usage, desc.MemoryFlags, image.Image, image.Memory);

		auto out = Vulkan::AllocatedImage();
		out.Image = image.Image;
		out.Memory = image.Memory;
		out.Handle = handle;
		out.ArrayLayers = desc.ArrayLayers;
		out.LayerStates.resize(desc.ArrayLayers, {vk::ImageLayout::eUndefined, vk::AccessFlagBits2::eNone, vk::PipelineStageFlagBits2::eTopOfPipe});
		return out;
	}

	void *GPUResourceManager::MapMemory(const Vulkan::AllocatedBuffer &buffer, vk::DeviceSize offset, vk::DeviceSize size)
	{
		if (!buffer.Handle)
			ASSERT(false, "Buffer is not valid");

		auto &gpu_buffer = GPUStorage::Buffers[buffer.Handle];
		return gpu_buffer.MappedMemory = gpu_buffer.Memory.mapMemory(offset, size);
	}

	void GPUResourceManager::CreateImageView(Vulkan::Image &image, const ImageViewDesc &desc)
	{
		auto &gpu_image = GPUStorage::Images[UUID()];
		VulkanUtils::CreateImageView(image.ImageSrc, gpu_image.View, desc.Type, desc.Format, desc.Aspect);
		image.View = gpu_image.View;
		image.Aspect = desc.Aspect;
	}

	void GPUResourceManager::CreateImageView(Vulkan::AllocatedImage &image, const ImageViewDesc &desc)
	{
		if (!GPUStorage::Images.contains(image.Handle))
			return;

		auto &gpu_image = GPUStorage::Images[image.Handle];
		VulkanUtils::CreateImageView(gpu_image.Image, gpu_image.View, desc.Type, desc.Format, desc.Aspect);
		image.View = gpu_image.View;
		image.Aspect = desc.Aspect;
	}

	void GPUResourceManager::CreateSampler(Vulkan::AllocatedImage &image, const vk::SamplerCreateInfo &create_info)
	{
		if (!GPUStorage::Images.contains(image.Handle))
			return;

		auto &gpu_image = GPUStorage::Images[image.Handle];
		VulkanUtils::CreateImageSampler(gpu_image.Sampler, create_info);
		image.Sampler = gpu_image.Sampler;
	}

	void GPUResourceManager::DestroyBuffer(Vulkan::AllocatedBuffer buffer)
	{
		auto h = buffer.Handle;
		if (GPUStorage::Buffers.contains(h))
		{
			auto &gpu_buffer = GPUStorage::Buffers.at(h);
			if (gpu_buffer.MappedMemory)
			{
				gpu_buffer.MappedMemory = nullptr;
				gpu_buffer.Memory.unmapMemory();
			}
			
			GPUStorage::Buffers.erase(h);	
		}
	}

	void GPUResourceManager::DestroyImage(Vulkan::AllocatedImage image)
	{
		auto h = image.Handle;
		if (GPUStorage::Images.contains(h))
		{
			GPUStorage::Images.erase(h);
		}
	}

	GPUResourceManager &GPUResourceManager::Get()
	{
		static GPUResourceManager gpu_manager;
		return gpu_manager;
	}
} // namespace BHive