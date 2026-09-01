#include "GPUResourceManager.h"
#include "VulkanBackend.h"
#include "VulkanRendererAPI.h"
#include "VulkanUtils.h"
#include "gfx/RenderCommand.h"

namespace BHive
{

	GPUBufferResource::GPUBufferResource(const std::string &name, vk::BufferCreateInfo info, vk::MemoryPropertyFlags flags, size_t size, MemoryAllocator *allocator)
		: Size(size),
		  mAllocator(allocator)
	{
		auto &device = VulkanBackend::GetLogicalDevice();
		Buffer = device.createBuffer(info);
		Allocation = allocator->Allocate(Buffer, flags);

		Buffer.bindMemory(Allocation.Memory, Allocation.Offset);

#if BHIVE_ENABLE_OBJECT_NAMES
		VulkanBackend::SetObjectName(*Buffer, name);
#endif
	}

	GPUBufferResource::~GPUBufferResource()
	{

		unmap();
		mAllocator->Free(Allocation);
	}

	void *GPUBufferResource::map(vk::DeviceSize offset, vk::DeviceSize size)
	{
		if (!Allocation.IsMapped)
		{
			Allocation.MappedPtr = mAllocator->Map(Allocation);
			Allocation.IsMapped = true;
		}
		return static_cast<char *>(Allocation.MappedPtr) + offset;
	}

	void GPUBufferResource::unmap()
	{
		if (Allocation.IsMapped && !Allocation.IsDedicated)
		{
			mAllocator->UnMap(Allocation);
			Allocation.IsMapped = false;
		}
	}

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

	GPUBufferResourceHandle GPUResourceManager::CreateBuffer(const vk::BufferCreateInfo &info, vk::MemoryPropertyFlags flags, const std::string &name)
	{
		GPUBufferResourceHandle id{};
		mBuffers.try_emplace(id, name, info, flags, info.size, &VulkanBackend::GetMemoryAllocator());
		return id;
	}

	ResourceID GPUResourceManager::CreateImage(const vk::ImageCreateInfo &info, vk::MemoryPropertyFlags flags, const std::string &name)
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

		VulkanBackend::SetObjectName(*image, name);

		return id;
	}

	ResourceID GPUResourceManager::RegisterExternalImage(const vk::Image &image, const std::string &name)
	{
		ResourceID id{};
		auto &storage = GetStorage<vk::Image>();
		storage.Create(id) = image;
		mExternalImages.insert(id);

		VulkanBackend::SetObjectName(image, name);

		return id;
	}

	ResourceID GPUResourceManager::CreateImageView(const vk::ImageViewCreateInfo &info, const std::string &name)
	{
		ResourceID id{};
		auto &view = GetStorage<vk::raii::ImageView>().Create(id);

		auto &device = VulkanBackend::GetLogicalDevice();
		view = device.createImageView(info);

		VulkanBackend::SetObjectName(*view, name);

		return id;
	}

	ResourceID GPUResourceManager::CreateSampler(const vk::SamplerCreateInfo &info, const std::string &name)
	{
		ResourceID id{};
		auto &sampler = GetStorage<vk::raii::Sampler>().Create(id);
		auto &device = VulkanBackend::GetLogicalDevice();
		sampler = device.createSampler(info);
		VulkanBackend::SetObjectName(*sampler, name);

		return id;
	}

	void GPUResourceManager::Destroy(GPUBufferResourceHandle handle)
	{
		RenderCommand::QueueDeletion(
			[this, handle](uint32_t)
			{
				auto &storage = mBuffers;
				storage.erase(handle);
			});
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

				GetStorage<vk::raii::Image>().Remove(handle);
				GetStorage<MemoryAllocation>().Remove(handle);
			});
	}

	void GPUResourceManager::DestroyImageView(ResourceID handle)
	{
		RenderCommand::QueueDeletion(
			[this, handle](uint32_t)
			{
				auto &storage = GetStorage<vk::raii::ImageView>();
				storage.Remove(handle);
			});
	}

	void GPUResourceManager::DestroySampler(ResourceID handle)
	{
		RenderCommand::QueueDeletion(
			[this, handle](uint32_t)
			{
				auto &storage = GetStorage<vk::raii::Sampler>();
				storage.Remove(handle);
			});
	}

	void GPUResourceManager::DestroyImage(GPUImage &image)
	{
		if (auto smp = image.Sampler)
		{
			DestroySampler(smp);
		}

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

		image.Sampler.Release();
		image.Views.Default.Release();
		image.Views.Mips.clear();
		image.Views.CubeMips.clear();
		image.Views.Faces.clear();

		auto handle = image.Image;
		auto name = image.DebugName;

		RenderCommand::QueueDeletion(
			[this, handle, name](uint32_t)
			{
				if (mExternalImages.contains(handle))
				{
					auto &imgStorage = GetStorage<vk::Image>();
					mExternalImages.erase(handle);
					imgStorage.Remove(handle);
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
			});
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