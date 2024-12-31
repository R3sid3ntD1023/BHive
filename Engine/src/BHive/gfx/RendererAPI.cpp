#include "RendererAPI.h"
#include "Platform/GL/GLRendererAPI.h"

namespace BHive
{
	Scope<RendererAPI> BHive::RendererAPI::Create()
	{
		switch (sAPI)
		{
		case BHive::RendererAPI::Opengl:
			return CreateScope<GLRendererAPI>();
		case BHive::RendererAPI::Vulkan:
			break;
		default:
			break;
		}
		
		ASSERT(false);
		return nullptr;
	}
}
