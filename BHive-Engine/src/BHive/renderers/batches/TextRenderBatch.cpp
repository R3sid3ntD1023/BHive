#include "TextRenderBatch.h"
#include "gfx/ShaderManager.h"
#include "gfx/RenderCommand.h"
#include "renderers/Renderer.h"

namespace BHive
{
	BufferLayout TextVertex::GetLayout()
	{
		return {{EShaderDataType::Float4}, {EShaderDataType::Float2}, {EShaderDataType::Float4}, {EShaderDataType::Int},
				{EShaderDataType::Float2}, {EShaderDataType::Float2}, {EShaderDataType::Float4}, {EShaderDataType::Int}};
	}

	Ref<Shader> TextRenderBatch::GetShader() const
	{
		return ShaderManager::Get().Load(ENGINE_SHADER_PATH "/Text.glsl");
	}

	void TextRenderBatch::Flush()
	{
		if (mIndexCount)
		{
			TRenderBatch::Flush();

			if (mTextureBatch)
				mTextureBatch->Flush();

			RenderCommand::EnableDepthMask(false);
			RenderCommand::DrawElements(ETopologyMode::Triangles, mVertexArray, mIndexCount);
			RenderCommand::EnableDepthMask(true);

			Renderer::GetStats().DrawCalls++;
		}
	}

	void TextRenderBatch::SetTextureBatch(TextureBatchData *texture_batch)
	{
		mTextureBatch = texture_batch;
	}
} // namespace BHive