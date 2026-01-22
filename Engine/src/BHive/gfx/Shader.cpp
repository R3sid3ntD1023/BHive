#include "Platform/Vulkan/VulkanShader.h"
#include "RenderCommand.h"
#include "Shader.h"

namespace BHive
{
	Ref<Shader> Shader::Create(const std::filesystem::path &path, const FRenderOptions &options)
	{
		switch (RenderCommand::GetRendererAPI())
		{
		case RendererAPI::Vulkan:
			return CreateRef<VulkanShader>(path, options);
		}

		ASSERT(false);
		return nullptr;
	}

	Ref<Shader> Shader::Create(const std::string &name, const std::string &vert, const std::string &frag, const FRenderOptions &options)
	{
		switch (RenderCommand::GetRendererAPI())
		{
		case RendererAPI::Vulkan:
			return CreateRef<VulkanShader>(name, vert, frag, options);
		}

		ASSERT(false);
		return nullptr;
	}

} // namespace BHive