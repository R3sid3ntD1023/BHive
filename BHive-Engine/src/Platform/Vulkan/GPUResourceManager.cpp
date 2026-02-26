#include "GPUResourceManager.h"
#include "VulkanUtils.h"
#include "VulkanBackend.h"

namespace BHive
{
	struct GPUBuffer
	{
		vk::raii::Buffer Buffer = VK_NULL_HANDLE;
	};

	struct GPUImage
	{
		vk::raii::Image Image = VK_NULL_HANDLE;
		vk::raii::ImageView View = VK_NULL_HANDLE;
		vk::raii::Sampler Sampler = VK_NULL_HANDLE;
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

	AllocatedBuffer GPUResourceManager::CreateBuffer(const BufferDesc &desc)
	{
		auto handle = BufferHandle();
		auto& buffer = GPUStorage::Buffers[handle];
		VulkanUtils::CreateBuffer(desc.Size, desc.Usage, desc.MemoryFlags, buffer.Buffer);

		auto& allocator = VulkanBackend::GetMemoryAllocator();
		MemoryAllocation allocation = allocator.Allocate(buffer.Buffer, desc.MemoryFlags);

		buffer.Buffer.bindMemory(allocation.Memory, allocation.Offset);

		return {buffer.Buffer, std::move(allocation), handle};
	}

	AllocatedImage GPUResourceManager::CreateImage(const ImageDesc &desc)
	{
		auto handle = ImageHandle();
		auto &image = GPUStorage::Images[handle];
		VulkanUtils::CreateImage(desc.Width, desc.Height, desc.Depth, desc.ArrayLayers, desc.Type, desc.Format, desc.Tiling, desc.Usage, desc.MemoryFlags, image.Image);

		auto &allocator = VulkanBackend::GetMemoryAllocator();
		MemoryAllocation allocation = allocator.Allocate(image.Image, desc.MemoryFlags);

		image.Image.bindMemory(allocation.Memory, allocation.Offset);

		AllocatedImage out{};
		out.Image = image.Image;
		out.Handle = handle;
		out.ArrayLayers = desc.ArrayLayers;
		out.LayerStates.resize(desc.ArrayLayers, {vk::ImageLayout::eUndefined, vk::AccessFlagBits2::eNone, vk::PipelineStageFlagBits2::eTopOfPipe});
		out.Allocation = std::move(allocation);
		return out;
	}

	void* GPUResourceManager::MapMemory(AllocatedBuffer &buffer, vk::DeviceSize offset, vk::DeviceSize size)
	{
		if (!buffer.Handle)
			ASSERT(false, "Buffer is not valid");

		auto &allocation = buffer.Allocation;
		if (allocation.MappedPtr)
		{
			return static_cast<char *>(allocation.MappedPtr) + offset;
		}

		MemoryAllocator &allocator = VulkanBackend::GetMemoryAllocator();
		return allocator.Map(allocation);
	}

	void GPUResourceManager::UnmapMemory(AllocatedBuffer &buffer)
	{
		if (!buffer.Handle)
			ASSERT(false, "Buffer is not valid");

		MemoryAllocator &allocator = VulkanBackend::GetMemoryAllocator();
		allocator.UnMap(buffer.Allocation);
	}

	void GPUResourceManager::CreateImageView(Image &image, const ImageViewDesc &desc)
	{
		auto &gpu_image = GPUStorage::Images[UUID()];
		VulkanUtils::CreateImageView(image.ImageSrc, gpu_image.View, desc.Type, desc.Format, desc.Aspect, desc.ArrayLayers);
		image.View = gpu_image.View;
		image.Aspect = desc.Aspect;
	}

	void GPUResourceManager::CreateImageView(AllocatedImage &image, const ImageViewDesc &desc)
	{
		if (!GPUStorage::Images.contains(image.Handle))
			return;

		auto &gpu_image = GPUStorage::Images[image.Handle];
		VulkanUtils::CreateImageView(gpu_image.Image, gpu_image.View, desc.Type, desc.Format, desc.Aspect, desc.ArrayLayers);
		image.View = gpu_image.View;
		image.Aspect = desc.Aspect;
	}

	void GPUResourceManager::CreateSampler(AllocatedImage &image, const vk::SamplerCreateInfo &create_info)
	{
		if (!GPUStorage::Images.contains(image.Handle))
			return;

		auto &gpu_image = GPUStorage::Images[image.Handle];
		VulkanUtils::CreateImageSampler(gpu_image.Sampler, create_info);
		image.Sampler = gpu_image.Sampler;
	}

	void GPUResourceManager::DestroyBuffer(AllocatedBuffer buffer)
	{
		auto h = buffer.Handle;
		if (GPUStorage::Buffers.contains(h))
		{
			auto &gpu_buffer = GPUStorage::Buffers.at(h);
			GPUStorage::Buffers.erase(h);	
			VulkanBackend::GetMemoryAllocator().Free(buffer.Allocation);
		}
	}

	void GPUResourceManager::DestroyImage(AllocatedImage image)
	{
		auto h = image.Handle;
		if (GPUStorage::Images.contains(h))
		{
			GPUStorage::Images.erase(h);
			VulkanBackend::GetMemoryAllocator().Free(image.Allocation);
		}
	}

} // namespace BHive