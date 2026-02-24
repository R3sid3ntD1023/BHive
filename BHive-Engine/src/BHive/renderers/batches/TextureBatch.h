#pragma once

#include "core/Core.h"
#include "RenderBatch.h"
#include "gfx/Texture.h"

namespace BHive
{
	struct TextureBatchData : public IRenderBatch
	{
		static inline const size_t sMaxTextureCount = 32;

		void Init();

		virtual void End() override;

		virtual void StartBatch() override;

		virtual void NextBatch() override;

		virtual void Flush() override;

		uint32_t GetTextureCount() const { return mTextureCount; }

		const Ref<Texture2DArray>& GetTexture() const { return mTextureArray; }

		uint32_t GetTextureIndex(IRenderBatch &data, const Ref<Texture> &texture);

	private:
		uint32_t mTextureCount = 1;

		Ref<Texture2DArray> mTextureArray;
	};

} // namespace BHive