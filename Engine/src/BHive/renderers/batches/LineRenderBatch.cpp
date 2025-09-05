#include "LineRenderBatch.h"
#include "gfx/ShaderManager.h"
#include "gfx/Shader.h"
#include "gfx/RenderCommand.h"
#include "renderers/Renderer.h"

namespace BHive
{
	void LineRenderBatch::Init()
	{
		mVertexDataBuffer = new FLineVertex[sMaxVertexCount];

		mVertexBuffer = CreateRef<VertexBuffer>(sMaxVertexCount * sizeof(FLineVertex));
		mVertexBuffer->SetLayout({{EShaderDataType::Float3}, {EShaderDataType::Float4}, {EShaderDataType::Int}});

		mVertexArray = CreateRef<VertexArray>();
		mVertexArray->AddVertexBuffer(mVertexBuffer);

		mLineShader = ShaderManager::Get().Load(ENGINE_SHADER_PATH "/Line.glsl");
	}

	void LineRenderBatch::End()
	{
		mLineShader->Bind();
		Flush();
		mLineShader->UnBind();
	}

	void LineRenderBatch::NextBatch()
	{
		if (mVertexCount >= sMaxVertexCount)
		{
			End();
			StartBatch();
		}
	}

	void LineRenderBatch::StartBatch()
	{
		mVertexDataPtr = mVertexDataBuffer;
		mVertexCount = 0;
	}

	void LineRenderBatch::Flush()
	{
		if (mVertexCount > 0)
		{

			uint32_t size = (uint32_t)((uint8_t *)mVertexDataPtr - (uint8_t *)mVertexDataBuffer);
			mVertexBuffer->SetData(mVertexDataBuffer, size);

			RenderCommand::DrawArrays(Lines, *mVertexArray, mVertexCount);
			Renderer::GetStats().DrawCalls++;
		}
	}

	LineRenderBatch::~LineRenderBatch()
	{
		mVertexDataPtr = nullptr;
		delete[] mVertexDataBuffer;
	}
} // namespace BHive