#include "TextRenderBatch.h"
#include "gfx/ShaderManager.h"
#include "gfx/RenderCommand.h"
#include "renderers/Renderer.h"
#include "material/BackendMaterial.h"

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

		Pipeline::PipelineState state = Pipeline::GetDefaultPipelineState();
		state.ShaderProgram = shaderProgram;
		state.Raster.CullEnabled = false;

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
			mMaterial->Bind(mPipeline);
			mMaterial->BindTexture("uTextures", textures);

		//	RenderCommand::EnableDepthMask(false);
			RenderCommand::DrawElements(ETopologyMode::Triangles, mVertexArray, mIndexCount);
			//RenderCommand::EnableDepthMask(true);

			Renderer::GetStats().DrawCalls++;
		}
	}

	void TextRenderBatch::SetTextureBatch(TextureBatchData *texture_batch)
	{
		mTextureBatch = texture_batch;
	}
} // namespace BHive