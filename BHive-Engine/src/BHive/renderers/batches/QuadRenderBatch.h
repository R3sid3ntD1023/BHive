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
		uint32_t Texture;
		int32_t Flags;
		int32_t EntityID{-1};

		static BufferLayout GetLayout();
	};

	struct QuadRenderBatch : public TRenderBatch<QuadVertex>
	{

		Ref<Shader> GetShader() const override;

		void Flush() override;

		void SetTextureBatch(TextureBatchData *texture_batch);

	private:
		TextureBatchData *mTextureBatch;
	};
} // namespace BHive