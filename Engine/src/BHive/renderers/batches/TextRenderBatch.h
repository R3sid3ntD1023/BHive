#pragma once

#include "TRenderBatch.h"
#include "TextureBatch.h"

namespace BHive
{
	struct TextVertex
	{
		glm::vec4 Position;
		glm::vec2 TexCoord;
		glm::vec4 Color;
		uint32_t Texture;
		glm::vec2 Thickness;
		glm::vec2 Outline;
		glm::vec4 OutlineColor;
		int32_t EntityID{-1};

		static BufferLayout GetLayout();
	};

	struct TextRenderBatch : public TRenderBatch<TextVertex>
	{

		Ref<Shader> GetShader() const override;

		void Flush() override;

		void SetTextureBatch(TextureBatchData *texture_batch);

	private:
		TextureBatchData *mTextureBatch;
	};
} // namespace BHive