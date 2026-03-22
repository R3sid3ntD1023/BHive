#include "CircleRenderBatch.h"
#include "gfx/ShaderManager.h"
#include "gfx/RenderCommand.h"
#include "gfx/renderers/Renderer.h"
#include "gfx/material/BackendMaterial.h"

namespace BHive
{
	BufferLayout CircleVertex::GetLayout()
	{
		return {{EShaderDataType::Float4}, {EShaderDataType::Float3}, {EShaderDataType::Float4}, {EShaderDataType::Float}, {EShaderDataType::Float}, {EShaderDataType::Int}};
	}

	
	CircleRenderBatch::~CircleRenderBatch()
	{
		mMaterial->Shutdown();
	}

	void CircleRenderBatch::Init(size_t vcount, size_t icount)
	{
		TRenderBatch::Init(vcount, icount);

		auto shaderProgram = ShaderManager::Get().Load(ENGINE_SHADER_PATH "/Circle.glsl");
		mPipeline = Pipeline::Create();

		auto state = Pipeline::GetDefaultPipelineState();
		state.ShaderProgram = shaderProgram;
		state.Raster.CullEnabled = false;
		state.Depth.DepthWrite = false;
	
		mPipeline->Init(state);

		mMaterial = IMaterialBackendInterface::Create();
		mMaterial->Init(mPipeline);
	}

	Ref<Pipeline> CircleRenderBatch::GetPipeline() const
	{
		return mPipeline;
	}

	void CircleRenderBatch::Flush()
	{
		if (mIndexCount)
		{
			TRenderBatch::Flush();

			mPipeline->Bind();
			mMaterial->Bind(mPipeline);

			RenderCommand::DrawElements(ETopologyMode::Triangles, mVertexArray, mIndexCount);

			Renderer::GetStats().DrawCalls++;
		}
	}
} // namespace BHive