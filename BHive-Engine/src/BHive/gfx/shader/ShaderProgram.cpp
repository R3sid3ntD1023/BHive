#include "ShaderProgram.h"
#include "Platform/Vulkan/VulkanShader.h"
#include "gfx/RenderCommand.h"

namespace BHive
{
	ShaderProgram::ShaderProgram(const Ref<ShaderAsset> &asset)
		: mAsset(asset)
	{
	}

	Ref<ShaderProgram> ShaderProgram::Create(const Ref<ShaderAsset> &asset)
	{
		switch (RenderCommand::GetAPI())
		{
		case RendererAPI::Vulkan:
			return CreateRef<VulkanShader>(asset);
		default:
			break;
		}

		ASSERT(false)
		return nullptr;
	}
} // namespace BHive