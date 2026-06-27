#include "GPUResourceManager.h"
#include "VulkanUtils.h"
#include "VulkanBackend.h"
#include "gfx/RenderCommand.h"
#include "VulkanRendererAPI.h"

namespace BHive
{
	ResourceID GPUResourceManager::CreateBuffer(const vk::BufferCreateInfo &info, vk::MemoryPropertyFlags flags,  const std::string &name)
	{
		
		auto &device = VulkanBackend::GetLogicalDevice();
		auto &physical_device = VulkanBackend::GetPhysicalDevice();

		ResourceID id{};
		auto &buffer = GetStorage<vk::raii::Buffer>().GetOrCreate(id);
		buffer = device.createBuffer(info);

		auto& allocator = VulkanBackend::GetMemoryAllocator();
		auto &allocation = GetStorage<MemoryAllocation>().GetOrCreate(id);
		allocation = allocator.Allocate(buffer, flags);

		buffer.bindMemory(allocation.Memory, allocation.Offset);

		VulkanBackend::SetObjectName(*buffer, name);

		return id;
	}

	ResourceID GPUResourceManager::CreateImage(const vk::ImageCreateInfo &info, vk::MemoryPropertyFlags flags, const std::string &name)
	{
		ResourceID id{};
		auto &image = GetStorage<vk::raii::Image>().GetOrCreate(id);

		auto &device = VulkanBackend::GetLogicalDevice();
		image = device.createImage(info);

		//bind image to memory
		auto &allocator = VulkanBackend::GetMemoryAllocator();
		auto &allocation = GetStorage<MemoryAllocation>().GetOrCreate(id);
		allocation = allocator.Allocate(image, flags);
		image.bindMemory(allocation.Memory, allocation.Offset);

		VulkanBackend::SetObjectName(*image, name);

		return id;
	}

	ResourceID GPUResourceManager::RegisterExternalImage(const vk::Image &image, const std::string &name)
	{
		ResourceID id{};
		auto img = vk::raii::Image(VulkanBackend::GetLogicalDevice(), image);
		GetStorage<vk::raii::Image>().AddExternal(id, std::move(img));
		mExternalImages.insert(id);

		VulkanBackend::SetObjectName(image, name);

		return id;
	}

	ResourceID GPUResourceManager::CreateImageView(const vk::ImageViewCreateInfo &info, const std::string &name)
	{
		ResourceID id{};
		auto &view = GetStorage<vk::raii::ImageView>().GetOrCreate(id);

		auto &device = VulkanBackend::GetLogicalDevice();
		view = device.createImageView(info);

		VulkanBackend::SetObjectName(*view, name);

		return id;
	}

	ResourceID GPUResourceManager::CreateSampler(const vk::SamplerCreateInfo &info, const std::string &name)
	{
		ResourceID id{};
		auto &sampler = GetStorage<vk::raii::Sampler>().GetOrCreate(id);
		auto &device = VulkanBackend::GetLogicalDevice();
		sampler = device.createSampler(info);
		VulkanBackend::SetObjectName(*sampler, name);
		return id;
	}

	void* GPUResourceManager::MapMemory( ResourceID buffer, vk::DeviceSize offset, vk::DeviceSize size)
	{
		if (!buffer)
			return nullptr;

		auto &allocation = GetStorage<MemoryAllocation>().Get(buffer);
		if (allocation.IsMapped)
		{
			return static_cast<char *>(allocation.MappedPtr) + offset;
		}

		MemoryAllocator &allocator = VulkanBackend::GetMemoryAllocator();
		return allocator.Map(allocation);
	}

	void GPUResourceManager::UnmapMemory( ResourceID buffer)
	{
		
		if (!buffer)
			return;

		auto &allocation = GetStorage<MemoryAllocation>().Get(buffer);
		if (!allocation.IsMapped || allocation.IsDedicated)
			return;

		MemoryAllocator &allocator = VulkanBackend::GetMemoryAllocator();
		allocator.UnMap(allocation);
	}



	void GPUResourceManager::DestroyBuffer( ResourceID handle)
	{
		RenderCommand::QueueDeletion(
			[this, handle](uint32_t)
			{
				GetStorage<vk::raii::Buffer>().Remove(handle);
				GetStorage<MemoryAllocation>().Remove(handle);
			});
		
	}

	void GPUResourceManager::DestroyImage( ResourceID  handle)
	{
		RenderCommand::QueueDeletion(
			[this, handle](uint32_t)
			{
				if (mExternalImages.contains(handle))
				{
					mExternalImages.erase(handle);
					GetStorage<vk::raii::Image>().Remove(handle);
					return;
				}

				GetStorage<vk::raii::Image>().Remove(handle);
				GetStorage<MemoryAllocation>().Remove(handle);

			});
	}

	void GPUResourceManager::DestroyImageView( ResourceID  handle)
	{
		RenderCommand::QueueDeletion(
			[this, handle](uint32_t)
			{
				auto &storage = GetStorage<vk::raii::ImageView>();
				storage.Remove(handle);
			});
	}

	void GPUResourceManager::DestroySampler( ResourceID  handle)
	{
		RenderCommand::QueueDeletion(
			[this, handle](uint32_t)
			{
				auto &storage = GetStorage<vk::raii::Sampler>();
				storage.Remove(handle);
			});
	}

	void GPUResourceManager::DestroyBuffer(AllocatedBuffer buffer)
	{
		DestroyBuffer(buffer.Buffer);

		RenderCommand::QueueDeletion(
			[this, handle = buffer.Buffer](uint32_t)
			{
				auto &alloc = GetStorage<MemoryAllocation>().Get(handle);
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

		auto &alloc = GetStorage<MemoryAllocation>().Get(image.Image);
		if (alloc.IsDedicated)
			 return;

		RenderCommand::QueueDeletion([this, alloc = alloc](uint32_t) {
			VulkanBackend::GetMemoryAllocator().Free(alloc);
		});
	}


	vk::Image GPUResourceManager::GetImage( ResourceID handle)
	{
		auto &storage = GetStorage<vk::raii::Image>();
		return *storage.Get(handle);
	}

	vk::ImageView GPUResourceManager::GetImageView( ResourceID handle)
	{
		auto &storage = GetStorage<vk::raii::ImageView>();
		return *storage.Get(handle);
	}

	vk::Sampler GPUResourceManager::GetSampler( ResourceID handle)
	{
		auto &storage = GetStorage<vk::raii::Sampler>();
		return storage.Get(handle);
	}

	vk::Buffer GPUResourceManager::GetBuffer( ResourceID handle)
	{
		auto &storage = GetStorage<vk::raii::Buffer>();
		return *storage.Get(handle);
	}

} // namespace BHive