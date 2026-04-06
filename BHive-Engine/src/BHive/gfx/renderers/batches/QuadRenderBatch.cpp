#include "QuadRenderBatch.h"
#include "gfx/ShaderManager.h"
#include "gfx/RenderCommand.h"
#include "gfx/renderers/Renderer.h"
#include "gfx/material/BackendMaterial.h"

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

		auto state = Pipeline::GetDefaultGraphicsPipelineState();
		state.ShaderProgram = shaderProgram;
		state.Raster.CullEnabled = false;
		state.Depth.DepthWrite = false;
	
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
			mMaterial->BindTexture("uTexture", texture, 0, mPipeline);

			RenderCommand::DrawElements(ETopologyMode::Triangles, mVertexArray, mIndexCount);

			Renderer::GetStats().DrawCalls++;
		}
	}

	void QuadRenderBatch::SetTextureBatch(TextureBatchData *texture_batch)
	{
		mTextureBatch = texture_batch;
	}

} // namespace BHive