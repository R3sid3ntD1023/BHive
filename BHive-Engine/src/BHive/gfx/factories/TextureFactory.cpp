#include "TextureFactory.h"
#include "gfx/RenderCommand.h"
#include "gfx/Texture.h"
#include "importers/TextureImporter.h"

namespace BHive
{
	TexturePtr TextureFactory::Create2D(const DecodedTexture &decodedTexture)
	{
		return Create2D(decodedTexture.Size, decodedTexture.CreateInfo, decodedTexture.Data);
	}

	TexturePtr TextureFactory::Create2D()
	{
		return CreateResource<Texture2D>();
	}

	TexturePtr TextureFactory::Create2D(const glm::uvec2 &size, const FTextureCreateInfo &info, const ByteBuffer &data)
	{
		return CreateResource<Texture2D>(size, info, data);
	}

	TexturePtr TextureFactory::Create2DArray(const glm::uvec2 &size, const FTextureCreateInfo &info)
	{
		return CreateResource<Texture2DArray>(size, info);
	}

	TexturePtr TextureFactory::Create3D(const glm::uvec3 &size, const FTextureCreateInfo &info, const ByteBuffer &data)
	{
		return CreateResource<Texture3D>(size, info, data);
	}

	TexturePtr TextureFactory::CreateCube(uint32_t size, const FTextureCreateInfo &info)
	{
		return CreateResource<TextureCube>(size, info);
	}

	TexturePtr TextureFactory::CreateCubeArray(uint32_t size, const FTextureCreateInfo &info)
	{
		return CreateResource<TextureCubeArray>(size, info);
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