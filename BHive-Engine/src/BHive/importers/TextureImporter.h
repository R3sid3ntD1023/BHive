#pragma once

#include "asset/AssetMetaData.h"
#include "core/Buffer.h"

namespace BHive
{
	class Texture2D;

	struct BHIVE_API DecodedTexture
	{
		glm::uvec3 Size{0, 0, 0}; // w, h, c
		Buffer Data;			  // data
		FTextureCreateInfo CreateInfo{};
	};

	struct BHIVE_API TextureLoader
	{
		static bool LoadImageData(const std::filesystem::path &file, int32_t &w, int32_t &h, int32_t &c, Buffer &buf, int32_t flip = 1);

		static DecodedTexture FromFile(const std::filesystem::path &file);

		static DecodedTexture LoadFromMemory(const uint8_t *data, int length);

		static void Resize(DecodedTexture &decodedTexture, const glm::uvec2 &requestedSize);

	private:
		static DecodedTexture CreateDecodedTexture(const std::string &name, const glm::uvec3 &size, const Buffer &buf, bool hdr);
	};
} // namespace BHive