#pragma once

#include "IResourceFactory.h"
#include "gfx/Enumerations.h"
#include "gfx/Texture.h"

namespace BHive
{
	struct BHIVE_API TextureFactory : public IResourceFactory<Texture>
	{
		static ResourceHandle Import(const std::filesystem::path &path);

		static ResourceHandle CreateFromMemory(const uint8_t *data, size_t size);

		static ResourceHandle Create2D();

		static ResourceHandle Create2D(const glm::uvec2 &size, const FTextureCreateInfo &info = {}, const Buffer &data = {});

		static ResourceHandle Create2DArray(const glm::uvec2 &size, const FTextureCreateInfo &info);

		static ResourceHandle Create3D(const glm::uvec3 &size, const FTextureCreateInfo &info, const Buffer &data = {});

		static ResourceHandle CreateCube(uint32_t size, const FTextureCreateInfo &info);

		static ResourceHandle CreateCubeArray(uint32_t size, const FTextureCreateInfo &info);

		static ResourceHandle Create(ETextureType type, const glm::uvec2 &size, const FTextureCreateInfo &info);
	};
} // namespace BHive