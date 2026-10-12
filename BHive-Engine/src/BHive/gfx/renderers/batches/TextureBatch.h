#pragma once

#include "core/Core.h"

namespace BHive
{
	class Texture2D;
	class Texture2DArray;

	struct BHIVE_API TextureBatchData
	{
		static inline const size_t sMaxTextureCount = 32;

		void Initialize();

		void Reset();

		bool IsFull();

		uint32_t GetTextureCount() const { return mTextureCount; }

		Ref<Texture2DArray> GetTexture() const { return mTextureArray; }

		int32_t GetTextureIndex(Ref<Texture2D> texture);

	private:
		uint32_t mTextureCount = 1;
		Ref<Texture2DArray> mTextureArray;
	};

} // namespace BHive