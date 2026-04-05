#include "TextRenderBatch.h"
#include "gfx/ShaderManager.h"
#include "gfx/RenderCommand.h"
#include "gfx/renderers/Renderer.h"
#include "gfx/material/BackendMaterial.h"

namespace BHive
{
	BufferLayout TextVertex::GetLayout()
	{
		return {{EShaderDataType::Float4}, {EShaderDataType::Float2}, {EShaderDataType::Float4}, {EShaderDataType::Int},
				{EShaderDataType::Float2}, {EShaderDataType::Float2}, {EShaderDataType::Float4}, {EShaderDataType::Int}};
	}

	TextRenderBatch::~TextRenderBatch()
	{
	}

	void TextRenderBatch::Init(size_t vcount, size_t icount)
	{
		TRenderBatch::Init(vcount, icount);

		auto shaderProgram = ShaderManager::Get().Load(ENGINE_SHADER_PATH "/Text.glsl");

		mPipeline = Pipeline::Create();

		auto state = Pipeline::GetDefaultGraphicsPipelineState();
		state.ShaderProgram = shaderProgram;
		state.Raster.CullEnabled = false;
		state.Depth.DepthWrite = false;

		mPipeline->Init(state);

		mMaterial = IMaterialBackendInterface::Create();
		mMaterial->Init(mPipeline);
	}

	Ref<Pipeline> TextRenderBatch::GetPipeline() const
	{
		return mPipeline;
	}

	void TextRenderBatch::Flush()
	{
		if (mIndexCount)
		{
			TRenderBatch::Flush();

			auto& textures = mTextureBatch->GetTexture();

			mPipeline->Bind();
			mMaterial->Bind(mPipeline);
			mMaterial->BindTexture("uTexture", textures);

			RenderCommand::DrawElements(ETopologyMode::Triangles, mVertexArray, mIndexCount);

			Renderer::GetStats().DrawCalls++;
		}
	}

	void TextRenderBatch::SetTextureBatch(TextureBatchData *texture_batch)
	{
		mTextureBatch = texture_batch;
	}
} // namespace BHive