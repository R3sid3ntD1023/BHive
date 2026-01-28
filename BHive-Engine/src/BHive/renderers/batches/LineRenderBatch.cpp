#include "gfx/RenderCommand.h"
#include "gfx/Shader.h"
#include "gfx/ShaderManager.h"
#include "LineRenderBatch.h"
#include "material/Material.h"
#include "renderers/Renderer.h"

namespace BHive
{
	void LineRenderBatch::Init()
	{
		mVertexDataBuffer = new FLineVertex[sMaxVertexCount];

		mVertexBuffer = VertexBuffer::Create(sMaxVertexCount * sizeof(FLineVertex));
		mVertexBuffer->SetLayout({{EShaderDataType::Float3}, {EShaderDataType::Float4}, {EShaderDataType::Int}});

		mVertexArray = VertexArray::Create();
		mVertexArray->AddVertexBuffer(mVertexBuffer);

		Shader::FRenderOptions options{};
		options.DrawMode = EDrawMode::Lines;
		mLineShader = ShaderManager::Get().Load(ENGINE_SHADER_PATH "/Line.glsl", options);
		mLineMaterial = CreateRef<Material>(mLineShader);
	}

	void LineRenderBatch::End()
	{
		Flush();
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

			mLineMaterial->Submit();

			RenderCommand::SetLineWidth(2.0f);

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