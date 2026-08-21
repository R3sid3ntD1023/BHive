#include "Query.h"
#include "gfx/RenderCommand.h"
#include "Platform/Vulkan/VulkanQuery.h"

namespace BHive
{
	Ref<Query> Query::Create(uint32_t count)
	{
		switch (RenderCommand::GetAPI())
		{
		case RendererAPI::EAPI::Vulkan:
			return CreateRef<VulkanQuery>(count);
		}

		ASSERT(false)
		return nullptr;
	}
} // namespace BHive