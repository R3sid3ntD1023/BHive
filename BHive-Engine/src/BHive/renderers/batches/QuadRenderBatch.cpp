#include "QuadRenderBatch.h"
#include "gfx/ShaderManager.h"
#include "gfx/RenderCommand.h"
#include "renderers/Renderer.h"
#include "material/BackendMaterial.h"

namespace BHive
{
	
	BufferLayout QuadVertex::GetLayout()
	{
		return {{EShaderDataType::Float4}, {EShaderDataType::Float3}, {EShaderDataType::Float2}, {EShaderDataType::Float4}, {EShaderDataType::Int}, {EShaderDataType::Int}, {EShaderDataType::Int}};
	}

	QuadRenderBatch::~QuadRenderBatch()
	{
		mMaterial->Shutdown();
	}

	void QuadRenderBatch::Init(size_t vcount, size_t icount)
	{
		TRenderBatch::Init(vcount, icount);

		auto shaderProgram = ShaderManager::Get().Load(ENGINE_SHADER_PATH "/Quad.glsl");

		mPipeline = Pipeline::Create();

		Pipeline::PipelineState state = Pipeline::GetDefaultPipelineState();
		state.ShaderProgram = shaderProgram;
		state.Raster.CullEnabled = false;
		mPipeline->Init(state);

		mMaterial = IMaterialBackendInterface::Create();
		mMaterial->Init(mPipeline);
	}

	Ref<Pipeline> QuadRenderBatch::GetPipeline() const
	{
		return mPipeline;

	}


	void QuadRenderBatch::Flush()
	{
		if (mIndexCount)
		{
			TRenderBatch::Flush();

			auto& texture = mTextureBatch->GetTexture();

			mPipeline->Bind();
			mMaterial->Bind(mPipeline);
			mMaterial->BindTexture("uTextures", texture);

			RenderCommand::DrawElements(ETopologyMode::Triangles, mVertexArray, mIndexCount);

			Renderer::GetStats().DrawCalls++;
		}
	}

	void QuadRenderBatch::SetTextureBatch(TextureBatchData *texture_batch)
	{
		mTextureBatch = texture_batch;
	}

} // namespace BHive