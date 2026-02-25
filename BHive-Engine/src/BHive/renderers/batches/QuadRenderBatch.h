#pragma once

#include "TRenderBatch.h"
#include "TextureBatch.h"

namespace BHive
{
	struct QuadVertex
	{
		glm::vec4 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoord;
		glm::vec4 Color;
		int32_t TextureIndex;
		int32_t Flags;
		int32_t EntityID{-1};

		static BufferLayout GetLayout();
	};

	class IMaterialBackendInterface;

	struct QuadRenderBatch : public TRenderBatch<QuadVertex>
	{

		~QuadRenderBatch();

		void Init(size_t vcount, size_t icount) override;

		Ref<Pipeline> GetPipeline() const override;

		void Flush() override;

		void SetTextureBatch(TextureBatchData *texture_batch);

	private:
		TextureBatchData *mTextureBatch;

		Ref<Pipeline> mPipeline;

		Ref<IMaterialBackendInterface> mMaterial;

	};
} // namespace BHive