#pragma once

#include "asset/AssetMetaData.h"
#include "core/Buffer.h"

namespace BHive
{
	class Texture2D;

	struct BHIVE_API FTextureImportData
	{
		int mWidth = 0, mHeight = 0;
		bool mFlip = true;
	};

	struct BHIVE_API TextureLoader
	{
		static bool LoadImageData(const std::filesystem::path &file, int32_t &w, int32_t &h, int32_t &c, uint8_t *&data, int32_t flip = 1);

		static Ref<Texture2D> Import(const std::filesystem::path &file, const FTextureImportData &import_data = {});

		static Ref<Texture2D> LoadFromMemory(const uint8_t *data, int length);
	};
} // namespace BHive