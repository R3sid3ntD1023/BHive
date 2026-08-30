#pragma once

#include "IResourceFactory.h"
#include "gfx/Enumerations.h"
#include "gfx/Texture.h"
#include "gfx/registries/Handles.h"
#include "importers/TextureImporter.h"

namespace BHive
{
	struct BHIVE_API TextureFactory : public IResourceFactory<Texture>
	{
		static TexturePtr Create2D();

		static TexturePtr Create2D(const DecodedTexture &decodedTexture);

		static TexturePtr Create2D(const glm::uvec2 &size, const FTextureCreateInfo &info = {}, const Buffer &data = {});

		static TexturePtr Create2DArray(const glm::uvec2 &size, const FTextureCreateInfo &info);

		static TexturePtr Create3D(const glm::uvec3 &size, const FTextureCreateInfo &info, const Buffer &data = {});

		static TexturePtr CreateCube(uint32_t size, const FTextureCreateInfo &info);

		static TexturePtr CreateCubeArray(uint32_t size, const FTextureCreateInfo &info);

		static TexturePtr Create(ETextureType type, const glm::uvec2 &size, const FTextureCreateInfo &info);
	};
} // namespace BHive