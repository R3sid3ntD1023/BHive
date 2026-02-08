#include "QuadRenderBatch.h"
#include "gfx/ShaderManager.h"
#include "gfx/RenderCommand.h"
#include "renderers/Renderer.h"

namespace BHive
{
	BufferLayout QuadVertex::GetLayout()
	{
		return {{EShaderDataType::Float4}, {EShaderDataType::Float3}, {EShaderDataType::Float2}, {EShaderDataType::Float4}, {EShaderDataType::Int}, {EShaderDataType::Int}, {EShaderDataType::Int}};
	}

	Ref<Shader> QuadRenderBatch::GetShader() const
	{
		return ShaderManager::Get().Load(ENGINE_SHADER_PATH "/Quad.glsl");
	}

	void QuadRenderBatch::Flush()
	{
		if (mIndexCount)
		{
			TRenderBatch::Flush();

			if (mTextureBatch)
				mTextureBatch->Flush();

			RenderCommand::DrawElements(Triangles, mVertexArray, mIndexCount);

			Renderer::GetStats().DrawCalls++;
		}
	}

	void QuadRenderBatch::SetTextureBatch(TextureBatchData *texture_batch)
	{
		mTextureBatch = texture_batch;
	}
} // namespace BHive