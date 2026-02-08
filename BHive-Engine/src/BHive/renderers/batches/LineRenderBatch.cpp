#include "gfx/RenderCommand.h"
#include "gfx/Shader.h"
#include "gfx/ShaderManager.h"
#include "LineRenderBatch.h"
#include "material/Material.h"
#include "renderers/Renderer.h"

namespace BHive
{
	LineRenderBatch::~LineRenderBatch()
	{
		mVertexDataPtr = nullptr;
		delete[] mVertexDataBuffer;
	}

	void LineRenderBatch::Init()
	{
		mVertexDataBuffer = new FLineVertex[sMaxVertexCount];

		mVertexBuffer = VertexBuffer::Create(sMaxVertexCount * sizeof(FLineVertex));
		mVertexBuffer->SetLayout({{EShaderDataType::Float3}, {EShaderDataType::Float4}, {EShaderDataType::Int}});

		mVertexArray = VertexArray::Create();
		mVertexArray->AddVertexBuffer(mVertexBuffer);

		Shader::FRenderOptions options{};
		options.DrawMode = EDrawMode::Lines;
		options.CullMode = ECullMode::Cull_None;
		options.EnableDepthTest = 0;
		options.EnableDepthWrite = 0;

		mLineShader = ShaderManager::Get().Load(ENGINE_SHADER_PATH "/Line.glsl", options);
		mLineMaterial = CreateRef<Material>(mLineShader);
	}



	void LineRenderBatch::End()
	{
		Flush();
	}

	void LineRenderBatch::NextBatch()
	{
		if (mVertexCount + 2 >= sMaxVertexCount)
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
		if (mVertexCount == 0)
			return;

		mVertexBuffer->SetData(mVertexDataBuffer, mVertexCount * sizeof(FLineVertex));

		mLineMaterial->Submit();

		RenderCommand::SetLineWidth(2.0f);

		RenderCommand::DrawArrays(Lines, mVertexArray, mVertexCount);

		Renderer::GetStats().DrawCalls++;
	}



	FLineVertex *LineRenderBatch::operator->()
	{
		return mVertexDataPtr;
	}

	LineRenderBatch &LineRenderBatch::operator++(int)
	{
		ASSERT(mVertexCount < sMaxVertexCount);
		ASSERT(mVertexDataPtr < mVertexDataBuffer + sMaxVertexCount);

		mVertexDataPtr++;
		mVertexCount++;
		return *this;
	}
} // namespace BHive