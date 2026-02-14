#include "CircleRenderBatch.h"
#include "gfx/ShaderManager.h"
#include "gfx/RenderCommand.h"
#include "renderers/Renderer.h"

namespace BHive
{
	BufferLayout CircleVertex::GetLayout()
	{
		return {{EShaderDataType::Float4}, {EShaderDataType::Float3}, {EShaderDataType::Float4}, {EShaderDataType::Float}, {EShaderDataType::Float}, {EShaderDataType::Int}};
	}

	Ref<Shader> CircleRenderBatch::GetShader() const
	{
		return ShaderManager::Get().Load(ENGINE_SHADER_PATH "/Circle.glsl");
	}

	void CircleRenderBatch::Flush()
	{
		if (mIndexCount)
		{
			TRenderBatch::Flush();

			RenderCommand::DrawElements(ETopologyMode::Triangles, mVertexArray, mIndexCount);

			Renderer::GetStats().DrawCalls++;
		}
	}
} // namespace BHive