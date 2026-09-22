#include "GPUResourceManager.h"
#include "VulkanBackend.h"
#include "VulkanRendererAPI.h"
#include "VulkanUtils.h"
#include "gfx/RenderCommand.h"

namespace BHive
{

	GPUResourceManager::~GPUResourceManager()
	{
		for (auto &[t, s] : mStorages)
		{
			auto size = s->Size();
			ASSERT(size == 0, "{} Container has size of {}", t, size);
		}
	}

	void GPUResourceManager::Shutdown()
	{
	}

	GPUBufferResourceHandle GPUResourceManager::CreateBuffer(const vk::BufferCreateInfo &info, vk::MemoryPropertyFlags flags)
	{
		GPUBufferResourceHandle id{};
		mBuffers.try_emplace(id, info, flags, info.size, &VulkanBackend::GetMemoryAllocator());
		return id;
	}

	ResourceID GPUResourceManager::CreateImage(const vk::ImageCreateInfo &info, vk::MemoryPropertyFlags flags)
	{
		ResourceID id{};
		auto &image = GetStorage<vk::raii::Image>().Create(id);

		auto &device = VulkanBackend::GetLogicalDevice();
		image = device.createImage(info);

		// bind image to memory
		auto &allocator = VulkanBackend::GetMemoryAllocator();
		auto &allocation = GetStorage<MemoryAllocation>().Create(id);
		allocation = allocator.Allocate(image, flags);
		image.bindMemory(allocation.Memory, allocation.Offset);

		return id;
	}

	ResourceID GPUResourceManager::RegisterExternalImage(const vk::Image &image)
	{
		ResourceID id{};
		auto &storage = GetStorage<vk::Image>();
		storage.Create(id) = image;
		mExternalImages.insert(id);

		return id;
	}

	ResourceID GPUResourceManager::CreateImageView(const vk::ImageViewCreateInfo &info)
	{
		ResourceID id{};
		auto &view = GetStorage<vk::raii::ImageView>().Create(id);

		auto &device = VulkanBackend::GetLogicalDevice();
		view = device.createImageView(info);

		return id;
	}

	ResourceID GPUResourceManager::CreateSampler(const vk::SamplerCreateInfo &info)
	{
		ResourceID id{};
		auto &sampler = GetStorage<vk::raii::Sampler>().Create(id);
		auto &device = VulkanBackend::GetLogicalDevice();
		sampler = device.createSampler(info);
		return id;
	}

	void GPUResourceManager::Destroy(GPUBufferResourceHandle handle)
	{
		RenderCommand::QueueDeletion(
			[this, handle](uint32_t)
			{
				auto &storage = mBuffers;
				storage.erase(handle);
			}
		);
	}

	void GPUResourceManager::DestroyImage(ResourceID handle)
	{
		RenderCommand::QueueDeletion(
			[this, handle](uint32_t)
			{
				if (mExternalImages.contains(handle))
				{
					mExternalImages.erase(handle);
					GetStorage<vk::Image>().Remove(handle);
					return;
				}

				auto &allocStorage = GetStorage<MemoryAllocation>();

				if (allocStorage.Contains(handle))
				{
					auto &imgStorage = GetStorage<vk::raii::Image>();

					auto &alloc = allocStorage.Get(handle);
					VulkanBackend::GetMemoryAllocator().Free(alloc);

					allocStorage.Remove(handle);
					imgStorage.Remove(handle);
				}

				handle.Release();
			}
		);
	}

	void GPUResourceManager::DestroyImageView(ResourceID handle)
	{
		RenderCommand::QueueDeletion(
			[this, handle](uint32_t)
			{
				auto &storage = GetStorage<vk::raii::ImageView>();
				storage.Remove(handle);

				handle.Release();
			}
		);
	}

	void GPUResourceManager::DestroySampler(ResourceID handle)
	{
		RenderCommand::QueueDeletion(
			[this, handle](uint32_t)
			{
				auto &storage = GetStorage<vk::raii::Sampler>();
				storage.Remove(handle);

				handle.Release();
			}
		);
	}

	GPUBufferResource *GPUResourceManager::ResolveBuffer(GPUBufferResourceHandle handle)
	{
		return &mBuffers.at(handle);
	}

	GPUImageResource *GPUResourceManager::ResolveImage(GPUImageResourceHandle handle)
	{
		return &mImages.at(handle);
	}

	vk::Image GPUResourceManager::GetImage(ResourceID handle)
	{
		if (mExternalImages.contains(handle))
		{
			auto &storage = GetStorage<vk::Image>();
			return storage.Get(handle);
		}
		auto &storage = GetStorage<vk::raii::Image>();
		return *storage.Get(handle);
	}

	vk::ImageView GPUResourceManager::GetImageView(ResourceID handle)
	{
		auto &storage = GetStorage<vk::raii::ImageView>();
		return *storage.Get(handle);
	}

	vk::Sampler GPUResourceManager::GetSampler(ResourceID handle)
	{
		auto &storage = GetStorage<vk::raii::Sampler>();
		return storage.Get(handle);
	}

} // namespace BHive