#pragma once

#include "asset/AssetMetaData.h"
#include "gfx/textures/Texture2D.h"
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
		static Ref<Texture2D> Import(const std::filesystem::path &file, const FTextureImportData &import_data = {});

		static Ref<Texture2D> LoadFromMemory(const uint8_t *data, int length);
	};
} // namespace BHive