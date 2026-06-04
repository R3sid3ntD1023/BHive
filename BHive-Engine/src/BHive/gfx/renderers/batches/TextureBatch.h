#pragma once

#include "core/Core.h"
#include "gfx/Texture.h"

namespace BHive
{
	struct TextureBatchData
	{
		static inline const size_t sMaxTextureCount = 32;

		void Initialize();

		void Reset() ;

		bool IsFull();

		uint32_t GetTextureCount() const { return mTextureCount; }

		const Ref<Texture2DArray>& GetTexture() const { return mTextureArray; }

		int32_t GetTextureIndex(const Ref<Texture2D> &texture);

	private:
		uint32_t mTextureCount = 1;
		Ref<Texture2DArray> mTextureArray;
	};

} // namespace BHive