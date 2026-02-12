#include "gfx/RenderCommand.h"
#include "Platform/Vulkan/textures/VulkanTexture2D.h"
#include "Platform/Vulkan/textures/VulkanTexture2DArray.h"
#include "Platform/Vulkan/textures/VulkanTexture3D.h"
#include "Platform/Vulkan/textures/VulkanTextureCube.h"
#include "Platform/Vulkan/textures/VulkanTextureCubeArray.h"
#include "Texture.h"
#include "utils/texture/TextureUtils.h"

namespace BHive
{
	Ref<Texture2D> Texture2D::Create()
	{
		switch (RenderCommand::GetRendererAPI())
		{
		case RendererAPI::EAPI::Vulkan:
			return CreateRef<VulkanTexture2D>();
		default:
			break;
		}

		ASSERT(false);
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(uint32_t w, uint32_t h, const FTextureCreateInfo &info, const void *buffer, size_t size)
	{
		switch (RenderCommand::GetRendererAPI())
		{
		case RendererAPI::EAPI::Vulkan:
			return CreateRef<VulkanTexture2D>(w, h, info, buffer, size);
		default:
			break;
		}

		ASSERT(false);
		return nullptr;
	}

	Ref<Texture2DArray> Texture2DArray::Create(uint32_t width, uint32_t height, uint32_t depth, const FTextureCreateInfo &specification)
	{
		switch (RenderCommand::GetRendererAPI())
		{
		case RendererAPI::EAPI::Vulkan:
			return CreateRef<VulkanTexture2DArray>(width, height, depth, specification);
		default:
			break;
		}

		ASSERT(false);
		return nullptr;
	}

	Ref<Texture3D> Texture3D::Create(uint32_t width, uint32_t height, uint32_t depth, const FTextureCreateInfo &create_info, const void *data)
	{
		switch (RenderCommand::GetRendererAPI())
		{
		case RendererAPI::EAPI::Vulkan:
			return CreateRef<VulkanTexture3D>(width, height, depth, create_info, data);
		default:
			break;
		}

		ASSERT(false);
		return nullptr;
	}

	Ref<TextureCube> TextureCube::Create(uint32_t size, const FTextureCreateInfo &info)
	{
		switch (RenderCommand::GetRendererAPI())
		{
		case RendererAPI::EAPI::Vulkan:
			return CreateRef<VulkanTextureCube>(size, info);
		default:
			break;
		}

		ASSERT(false);
		return nullptr;
	}

	Ref<TextureCubeArray> TextureCubeArray::Create(uint32_t width, uint32_t height, uint32_t depth, const FTextureCreateInfo &specification)
	{
		switch (RenderCommand::GetRendererAPI())
		{
		case RendererAPI::EAPI::Vulkan:
			return CreateRef<VulkanTextureCubeArray>(width, height, depth, specification);
		default:
			break;
		}

		ASSERT(false);
		return nullptr;
	}

	Ref<Texture2DMultisample> Texture2DMultisample::Create(uint32_t width, uint32_t height, uint32_t samples, const FTextureCreateInfo &create_info)
	{
		switch (RenderCommand::GetRendererAPI())
		{
		case RendererAPI::EAPI::Vulkan:
			//return CreateRef<VulkanTexture2DMultisample>(width, height, samples, create_info);
			break;
		default:
			break;
		}

		ASSERT(false);
		return nullptr;
	}

	REFLECT(Texture)
	{
		BEGIN_REFLECT(Texture) REFLECT_PROPERTY_READ_ONLY("Width", GetWidth) REFLECT_PROPERTY_READ_ONLY("Height", GetHeight);
		rttr::type::register_wrapper_converter_for_base_classes<Ref<Texture>>();
	}

	REFLECT(Texture2D)
	{
		BEGIN_REFLECT(Texture2D).constructor(rttr::select_overload<Ref<Texture2D>()>(&Texture2D::Create)) REFLECT_PROPERTY("Specification", GetInfo, SetInfo);
		rttr::type::register_wrapper_converter_for_base_classes<Ref<Texture2D>>();
	}
} // namespace BHive