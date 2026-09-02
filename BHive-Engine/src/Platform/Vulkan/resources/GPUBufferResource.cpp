#include "GPUBufferResource.h"
#include "Platform/Vulkan/VulkanBackend.h"

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

} // namespace BHive