#pragma once

#include "asset/AssetMetaData.h"
#include "core/Buffer.h"

namespace BHive
{
	class Texture2D;

	struct BHIVE_API FTextureOverride
	{
		int32_t Width = 0, Height = 0;

		bool Resize() const { return Width != 0 && Height != 0; }
	};

	struct BHIVE_API TextureLoader
	{
		static bool LoadImageData(const std::filesystem::path &file, int32_t &w, int32_t &h, int32_t &c, uint8_t *&data, int32_t flip = 1);

		static Ref<Texture2D> Import(const std::filesystem::path &file, const FTextureOverride &override = {});

		static Ref<Texture2D> LoadFromMemory(const uint8_t *data, int length);

	private:
		static Ref<Texture2D> CreateOrResizeTexture(int32_t w, int32_t h, int32_t c, uint8_t *data, size_t size, bool hdr, const FTextureOverride &override);

	};
} // namespace BHive