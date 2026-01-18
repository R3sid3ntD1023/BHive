#include "Platform/Vulkan/VulkanStorageBuffer.h"
#include "RenderCommand.h"
#include "StorageBuffer.h"

namespace BHive
{

	Ref<StorageBuffer> StorageBuffer::Create(uint32_t binding, size_t size, const void *data)
	{
		switch (RenderCommand::GetRendererAPI())
		{
		case RendererAPI::Vulkan:
			return CreateRef<VulkanStorageBuffer>(binding, size, data);
		default:
			break;
		}

		ASSERT(false)
		return nullptr;
	}

	Ref<StorageBuffer> StorageBuffer::Create(size_t size)
	{
		switch (RenderCommand::GetRendererAPI())
		{
		case RendererAPI::Vulkan:
			return CreateRef<VulkanStorageBuffer>(size);
		default:
			break;
		}

		ASSERT(false)
		return nullptr;
	}

} // namespace BHive