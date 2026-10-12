#include "gfx/RenderCommand.h"
#include "gfx/Texture.h"
#include "importers/TextureImporter.h"
#include "TextureFactory.h"

namespace BHive
{
	Ref<Texture2D> TextureFactory::Create2D(const DecodedTexture &decodedTexture)
	{
		return Create2D(decodedTexture.Size, decodedTexture.CreateInfo, decodedTexture.Data);
	}

	Ref<Texture2D> TextureFactory::Create2D()
	{
		return CreateRef<Texture2D>();
	}

	Ref<Texture2D> TextureFactory::Create2D(const glm::uvec2 &size, const FTextureCreateInfo &info, const ByteBuffer &data)
	{
		return CreateRef<Texture2D>(size, info, data);
	}

	Ref<Texture2DArray> TextureFactory::Create2DArray(const glm::uvec2 &size, const FTextureCreateInfo &info)
	{
		return CreateRef<Texture2DArray>(size, info);
	}

	Ref<Texture3D> TextureFactory::Create3D(const glm::uvec3 &size, const FTextureCreateInfo &info, const ByteBuffer &data)
	{
		return CreateRef<Texture3D>(size, info, data);
	}

	Ref<TextureCube> TextureFactory::CreateCube(uint32_t size, const FTextureCreateInfo &info)
	{
		return CreateRef<TextureCube>(size, info);
	}

	Ref<TextureCubeArray> TextureFactory::CreateCubeArray(uint32_t size, const FTextureCreateInfo &info)
	{
		return CreateRef<TextureCubeArray>(size, info);
	}

	Ref<Texture> TextureFactory::Create(ETextureType type, const glm::uvec2 &size, const FTextureCreateInfo &info)
	{
		switch (type)
		{
		case ETextureType::TEXTURE_2D:
			return Create2D({size.x, size.y}, info);
		case ETextureType::TEXTURE_CUBE:
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

} // namespace BHive