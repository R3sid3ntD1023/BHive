#include "TextureFactory.h"
#include "Platform/Vulkan/textures/VulkanTexture2D.h"
#include "Platform/Vulkan/textures/VulkanTexture2DArray.h"
#include "Platform/Vulkan/textures/VulkanTexture3D.h"
#include "Platform/Vulkan/textures/VulkanTextureCube.h"
#include "Platform/Vulkan/textures/VulkanTextureCubeArray.h"
#include "gfx/RenderCommand.h"
#include "importers/TextureImporter.h"

namespace BHive
{
	TexturePtr TextureFactory::Create2D(const DecodedTexture &decodedTexture)
	{
		return Create2D(decodedTexture.Size, decodedTexture.CreateInfo, decodedTexture.Data);
	}

	TexturePtr TextureFactory::Create2D()
	{
		switch (RenderCommand::GetAPI())
		{
		case RendererAPI::EAPI::Vulkan:
			return CreateResource<VulkanTexture2D>();
		default:
			break;
		}

		ASSERT(false);
		return {};
	}

	TexturePtr TextureFactory::Create2D(const glm::uvec2 &size, const FTextureCreateInfo &info, const Buffer &data)
	{
		switch (RenderCommand::GetAPI())
		{
		case RendererAPI::EAPI::Vulkan:
			return CreateResource<VulkanTexture2D>(size, info, data);
		default:
			break;
		}

		ASSERT(false);
		return {};
	}

	TexturePtr TextureFactory::Create2DArray(const glm::uvec2 &size, const FTextureCreateInfo &info)
	{
		switch (RenderCommand::GetAPI())
		{
		case RendererAPI::EAPI::Vulkan:
			return CreateResource<VulkanTexture2DArray>(size, info);
		default:
			break;
		}

		ASSERT(false);
		return {};
	}

	TexturePtr TextureFactory::Create3D(const glm::uvec3 &size, const FTextureCreateInfo &info, const Buffer &data)
	{
		switch (RenderCommand::GetAPI())
		{
		case RendererAPI::EAPI::Vulkan:
			return CreateResource<VulkanTexture3D>(size, info, data);
		default:
			break;
		}

		ASSERT(false);
		return {};
	}

	TexturePtr TextureFactory::CreateCube(uint32_t size, const FTextureCreateInfo &info)
	{
		switch (RenderCommand::GetAPI())
		{
		case RendererAPI::EAPI::Vulkan:
			return CreateResource<VulkanTextureCube>(size, info);
		default:
			break;
		}

		ASSERT(false);
		return {};
	}

	TexturePtr TextureFactory::CreateCubeArray(uint32_t size, const FTextureCreateInfo &info)
	{
		switch (RenderCommand::GetAPI())
		{
		case RendererAPI::EAPI::Vulkan:
			return CreateResource<VulkanTextureCubeArray>(size, info);
		default:
			break;
		}

		ASSERT(false);
		return {};
	}

	TexturePtr TextureFactory::Create(ETextureType type, const glm::uvec2 &size, const FTextureCreateInfo &info)
	{
		switch (type)
		{
		case ETextureType::TEXTURE_2D:
			return Create2D({size.x, size.y}, info);
		case ETextureType::TEXTURE_CUBE_MAP:
			return CreateCube(size.x, info);
		case ETextureType::TEXTURE_2D_ARRAY:
			return Create2DArray({size.x, size.y}, info);
		case ETextureType::TEXTURE_CUBE_MAP_ARRAY:
			return CreateCubeArray(size.x, info);
		default:
			break;
		}

		ASSERT(false);
		return {};
	}

	FontPtr FontFactory::Create(const std::filesystem::path &path, uint32_t fontSize)
	{
		return CreateResource<Font>(path, fontSize);
	}

} // namespace BHive