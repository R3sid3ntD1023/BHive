#include "GPUResourceManager.h"
#include "VulkanUtils.h"
#include "VulkanBackend.h"
#include "gfx/RenderCommand.h"
#include "VulkanRendererAPI.h"

namespace BHive
{
	AllocatedBuffer GPUResourceManager::CreateBuffer(const BufferDesc &desc)
	{
		AllocatedBuffer out{};

		auto handle = UUID();
		auto &buffer = GetStorage<vk::raii::Buffer>().GetOrCreate(handle);

		VulkanUtils::CreateBuffer(desc.Size, desc.Usage, desc.MemoryFlags, buffer);

		auto& allocator = VulkanBackend::GetMemoryAllocator();
		MemoryAllocation allocation = allocator.Allocate(buffer, desc.MemoryFlags, desc.Size);

		buffer.bindMemory(allocation.Memory, allocation.Offset);

		out.Buffer = handle;
		out.Allocation = allocation;
		out.Size = desc.Size;
		return out;
	}

	AllocatedImage GPUResourceManager::CreateImage(const ImageDesc &desc, const ImageViewDesc &viewDesc)
	{
		AllocatedImage out{};

		auto handle = UUID();
		auto &image = GetStorage<vk::raii::Image>().GetOrCreate(handle);

		VulkanUtils::CreateImage(desc.Flags, desc.Levels, desc.Width, desc.Height, desc.Depth, desc.ArrayLayers, desc.Type, desc.Format, desc.Tiling, desc.Usage, desc.MemoryFlags, image);

		auto &allocator = VulkanBackend::GetMemoryAllocator();
		MemoryAllocation allocation = allocator.Allocate(image, desc.MemoryFlags, desc.Size());

		image.bindMemory(allocation.Memory, allocation.Offset);

		out.ImageHandle = handle;
		out.ViewHandle = CreateImageView(image, viewDesc);
		out.ArrayLayers = desc.ArrayLayers;
		out.LayerStates.resize(desc.ArrayLayers, {vk::ImageLayout::eUndefined, vk::AccessFlagBits2::eNone, vk::PipelineStageFlagBits2::eTopOfPipe});
		out.Allocation = std::move(allocation);
		out.Aspect = desc.Aspect;
		return out;
	}


	void GPUResourceManager::CreateImageView(AllocatedImage &image, const ImageViewDesc &desc)
	{
		auto& vk_image = GetStorage<vk::raii::Image>().Get(image.ImageHandle);
		image.ViewHandle = CreateImageView(vk_image, desc);
	}

	UUID GPUResourceManager::CreateImageView(const vk::Image &image, const ImageViewDesc &desc)
	{
		auto handle = UUID();
		auto &view = GetStorage<vk::raii::ImageView>().GetOrCreate(handle);
		VulkanUtils::CreateImageView(image, view, desc.Type, desc.Format, desc.Aspect, desc.BaseMipLevel, desc.LevelCount, desc.BaseArrayLayer, desc.LayerCount);
		return handle;
	}

	void* GPUResourceManager::MapMemory(AllocatedBuffer &buffer, vk::DeviceSize offset, vk::DeviceSize size)
	{
		if (!buffer.Buffer )
			return nullptr;

		auto &allocation = buffer.Allocation;
		if (allocation.IsMapped)
		{
			return static_cast<char *>(allocation.MappedPtr) + offset;
		}

		MemoryAllocator &allocator = VulkanBackend::GetMemoryAllocator();
		return allocator.Map(allocation);
	}

	void GPUResourceManager::UnmapMemory(AllocatedBuffer &buffer)
	{
		if (!buffer.Buffer || !buffer.Allocation.IsMapped || buffer.Allocation.IsDedicated)
			return;

		MemoryAllocator &allocator = VulkanBackend::GetMemoryAllocator();
		allocator.UnMap(buffer.Allocation);
	}

	void GPUResourceManager::CreateSampler(AllocatedImage &image, const vk::SamplerCreateInfo &create_info)
	{
		auto handle = UUID();
		auto &sampler = GetStorage<vk::raii::Sampler>().GetOrCreate(handle);

		VulkanUtils::CreateImageSampler(sampler, create_info);
		image.SamplerHandle = handle;
	}

	void GPUResourceManager::DestroyBuffer(const UUID& handle)
	{
		auto api = RenderCommand::GetRendererAPI<VulkanRendererAPI>();
		api->QueueDeletion([this, handle](uint32_t) {
				auto &storage = GetStorage<vk::raii::Buffer>();
				storage.Remove(handle);
			});
		
	}

	void GPUResourceManager::DestroyImage(const UUID& handle)
	{
		auto api = RenderCommand::GetRendererAPI<VulkanRendererAPI>();
		api->QueueDeletion(
			[this, handle](uint32_t)
			{
				auto &storage = GetStorage<vk::raii::Image>();
				storage.Remove(handle);
			});
	}

	void GPUResourceManager::DestroyImageView(const UUID& handle)
	{
		auto api = RenderCommand::GetRendererAPI<VulkanRendererAPI>();
		api->QueueDeletion(
			[this, handle](uint32_t)
			{
				auto &storage = GetStorage<vk::raii::ImageView>();
				storage.Remove(handle);
			});
	}

	void GPUResourceManager::DestroySampler(const UUID& handle)
	{
		auto api = RenderCommand::GetRendererAPI<VulkanRendererAPI>();
		api->QueueDeletion(
			[this, handle](uint32_t)
			{
				auto &storage = GetStorage<vk::raii::Sampler>();
				storage.Remove(handle);
			});
	}

	void GPUResourceManager::DestroyBuffer(AllocatedBuffer buffer)
	{
		DestroyBuffer(buffer.Buffer);

		auto api = RenderCommand::GetRendererAPI<VulkanRendererAPI>();
		api->QueueDeletion(
			[this, alloc = buffer.Allocation](uint32_t)
			{
				VulkanBackend::GetMemoryAllocator().Free(alloc);
			});
	}

	void GPUResourceManager::DestroyImage(AllocatedImage image)
	{
		DestroySampler(image.SamplerHandle);
		DestroyImageView(image.ViewHandle);
		DestroyImage(image.ImageHandle);

		auto api = RenderCommand::GetRendererAPI<VulkanRendererAPI>();
		api->QueueDeletion([this, alloc = image.Allocation](uint32_t) {
			
			VulkanBackend::GetMemoryAllocator().Free(alloc);
		});
	}

	void GPUResourceManager::DestroyImage(Image image)
	{
		DestroyImageView(image.ViewHandle);
	}

	const vk::Image &GPUResourceManager::GetImage(const UUID &handle)
	{
		auto &storage = GetStorage<vk::raii::Image>();
		return *storage.Get(handle);
	}

	const vk::ImageView &GPUResourceManager::GetImageView(const UUID &handle)
	{
		auto &storage = GetStorage<vk::raii::ImageView>();
		return *storage.Get(handle);
	}

	const vk::Sampler &GPUResourceManager::GetSampler(const UUID &handle)
	{
		auto &storage = GetStorage<vk::raii::Sampler>();
		return *storage.Get(handle);
	}

	const vk::Buffer &GPUResourceManager::GetBuffer(const UUID &handle)
	{
		auto &storage = GetStorage<vk::raii::Buffer>();
		return *storage.Get(handle);
	}

} // namespace BHive