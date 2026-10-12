#pragma once

#include "gfx/Enumerations.h"
#include "gfx/font/Font.h"
#include "gfx/Texture.h"
#include "IResourceFactory.h"

namespace BHive
{
	struct DecodedTexture;

	struct BHIVE_API TextureFactory
	{
		static Ref<Texture2D> Create2D();

		static Ref<Texture2D> Create2D(const DecodedTexture &decodedTexture);

		static Ref<Texture2D> Create2D(const glm::uvec2 &size, const FTextureCreateInfo &info = {}, const ByteBuffer &data = {});

		static Ref<Texture2DArray> Create2DArray(const glm::uvec2 &size, const FTextureCreateInfo &info);

		static Ref<Texture3D> Create3D(const glm::uvec3 &size, const FTextureCreateInfo &info, const ByteBuffer &data = {});

		static Ref<TextureCube> CreateCube(uint32_t size, const FTextureCreateInfo &info);

		static Ref<TextureCubeArray> CreateCubeArray(uint32_t size, const FTextureCreateInfo &info);

		static Ref<Texture> Create(ETextureType type, const glm::uvec2 &size, const FTextureCreateInfo &info);
	};
} // namespace BHive