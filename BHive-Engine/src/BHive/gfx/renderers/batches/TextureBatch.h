#pragma once

#include "core/Core.h"
#include "gfx/registries/Handles.h"

namespace BHive
{
	struct TextureBatchData
	{
		static inline const size_t sMaxTextureCount = 32;

		void Initialize();

		void Reset();

		bool IsFull();

		uint32_t GetTextureCount() const { return mTextureCount; }

		TexturePtr GetTexture() const { return mTextureArray; }

		int32_t GetTextureIndex(TexturePtr texture);

	private:
		uint32_t mTextureCount = 1;
		TexturePtr mTextureArray;
	};

} // namespace BHive