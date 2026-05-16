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

		auto handle = ResourceID();
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

	GPUImage GPUResourceManager::CreateImage(const ImageDesc &desc)
	{
		//LOG_INFO("GPUResourceManager::CreateImage {} usage = 0x{:X}", desc.DebugName, (uint32_t)desc.Usage);

		GPUImage out{};

		auto handle = ResourceID();
		auto &image = GetStorage<vk::raii::Image>().GetOrCreate(handle);

		VulkanUtils::CreateImage(desc.Flags, desc.MipLevels, desc.Width, desc.Height, desc.Depth, desc.ArrayLayers, desc.Type, desc.Format, desc.Tiling, desc.Usage, desc.MemoryFlags, image);

		auto &allocator = VulkanBackend::GetMemoryAllocator();
		MemoryAllocation allocation = allocator.Allocate(image, desc.MemoryFlags, desc.Size());

		image.bindMemory(allocation.Memory, allocation.Offset);

		out.Image = handle;
		out.ArrayLayers = desc.ArrayLayers;
		out.Allocation = std::move(allocation);
		out.Aspect = desc.Aspect;
		out.DebugName = desc.DebugName;
		out.Usage = desc.Usage;
		out.State.Initialize(desc.ArrayLayers, desc.MipLevels, {vk::ImageLayout::eUndefined, vk::AccessFlagBits2::eNone, vk::PipelineStageFlagBits2::eTopOfPipe});
		return out;
	}

	ResourceID GPUResourceManager::RegisterExternalImage(const vk::Image &image)
	{
		ResourceID id = ResourceID();
		auto img = vk::raii::Image(VulkanBackend::GetLogicalDevice(), image);
		GetStorage<vk::raii::Image>().AddExternal(id, std::move(img));
		mExternalImages.insert(id);

		return id;
	}

	ResourceID GPUResourceManager::CreateImageView(const vk::Image &image, const ImageViewDesc &desc)
	{
		auto handle = ResourceID();
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

	void GPUResourceManager::CreateSampler(GPUImage &image, const vk::SamplerCreateInfo &create_info)
	{
		auto handle = ResourceID();
		auto &sampler = GetStorage<vk::raii::Sampler>().GetOrCreate(handle);

		VulkanUtils::CreateImageSampler(sampler, create_info);

		image.Sampler = handle;
	}

	void GPUResourceManager::DestroyBuffer(const ResourceID& handle)
	{
		auto api = RenderCommand::GetRendererAPI<VulkanRendererAPI>();
		api->QueueDeletion([this, handle](uint32_t) {
				auto &storage = GetStorage<vk::raii::Buffer>();
				storage.Remove(handle);
			});
		
	}

	void GPUResourceManager::DestroyImage(const ResourceID& handle)
	{
		auto api = RenderCommand::GetRendererAPI<VulkanRendererAPI>();
		api->QueueDeletion(
			[this, handle](uint32_t)
			{
				if (mExternalImages.contains(handle))
				{
					mExternalImages.erase(handle);
					GetStorage<vk::raii::Image>().Remove(handle);
					return;
				}

				GetStorage<vk::raii::Image>().Remove(handle);
			});
	}

	void GPUResourceManager::DestroyImageView(const ResourceID& handle)
	{
		auto api = RenderCommand::GetRendererAPI<VulkanRendererAPI>();
		api->QueueDeletion(
			[this, handle](uint32_t)
			{
				auto &storage = GetStorage<vk::raii::ImageView>();
				storage.Remove(handle);
			});
	}

	void GPUResourceManager::DestroySampler(const ResourceID& handle)
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

	void GPUResourceManager::DestroyImage(GPUImage& image)
	{
		if (auto smp = image.Sampler)
			DestroySampler(*smp);

		if (auto def = image.Views.Default)
			DestroyImageView(def);

		for (auto &layer : image.Views.Mips)
			for (auto &mip : layer)
				DestroyImageView(mip);

		for (auto &perCube : image.Views.CubeMips)
			for (auto &mip : perCube)
				DestroyImageView(mip);

		for (auto &faceMips : image.Views.Faces)
		{
			for (auto &perFace : faceMips)
				for (auto &mip : perFace)
					DestroyImageView(mip);
		}

		DestroyImage(image.Image);

		if (image.Allocation.IsDedicated)
			 return;

		auto api = RenderCommand::GetRendererAPI<VulkanRendererAPI>();
		api->QueueDeletion([this, alloc = image.Allocation](uint32_t) {
			
			VulkanBackend::GetMemoryAllocator().Free(alloc);
		});
	}


	const vk::Image &GPUResourceManager::GetImage(const ResourceID &handle)
	{
		auto &storage = GetStorage<vk::raii::Image>();
		return *storage.Get(handle);
	}

	const vk::ImageView &GPUResourceManager::GetImageView(const ResourceID &handle)
	{
		auto &storage = GetStorage<vk::raii::ImageView>();
		return *storage.Get(handle);
	}

	const vk::Sampler &GPUResourceManager::GetSampler(const ResourceID &handle)
	{
		auto &storage = GetStorage<vk::raii::Sampler>();
		return *storage.Get(handle);
	}

	const vk::Buffer &GPUResourceManager::GetBuffer(const ResourceID &handle)
	{
		auto &storage = GetStorage<vk::raii::Buffer>();
		return *storage.Get(handle);
	}

} // namespace BHive