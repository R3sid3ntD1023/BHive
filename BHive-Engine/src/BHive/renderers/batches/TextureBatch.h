#pragma once

#include "core/Core.h"
#include "RenderBatch.h"

namespace BHive
{
	class Texture;

	struct TextureBatchData : public IRenderBatch
	{
		static inline const size_t sMaxTextureCount = 512;

		void Init();

		virtual void End() override;

		virtual void StartBatch() override;

		virtual void NextBatch() override;

		virtual void Flush() override;

		uint32_t GetTextureIndex(IRenderBatch &data, const Ref<Texture> &texture);

	private:
		uint32_t mTextureCount = 1;

		std::array<Ref<Texture>, sMaxTextureCount> mTextures;
	};

} // namespace BHive