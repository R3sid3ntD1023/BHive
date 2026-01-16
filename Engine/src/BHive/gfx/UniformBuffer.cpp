#include "DescriptorBuilder.h"
#include "Platform/Vulkan/VulkanUniformBuffer.h"
#include "RenderCommand.h"
#include "UniformBuffer.h"

namespace BHive
{
	Ref<UniformBuffer> UniformBuffer::Create(uint32_t binding, uint64_t size, const void *data)
	{
		switch (RenderCommand::GetRendererAPI())
		{
		case BHive::RendererAPI::Opengl:
			break;
		case BHive::RendererAPI::Vulkan:
			return CreateRef<VulkanUniformBuffer>(binding, size, data);
		}

		ASSERT(false);
		return nullptr;
	}

} // namespace BHive