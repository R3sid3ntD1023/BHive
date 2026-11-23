#include "Platform/Vulkan/VulkanRendererAPI.h"
#include "Platform/Opengl/GLRendererAPI.h"
#include "RendererAPI.h"
#include "RenderCommand.h"

namespace BHive
{
	Scope<RendererAPI> RendererAPI::Create()
	{
		switch (RenderCommand::GetAPI())
		{
		case RendererAPI::EAPI::Opengl:
			return CreateScope<GLRendererAPI>();
		case RendererAPI::EAPI::Vulkan:
			return CreateScope<VulkanRendererAPI>();
		default:
			break;
		}

		ASSERT(false);
		return nullptr;
	}

} // namespace BHive
