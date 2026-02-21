#include "gfx/Framebuffer.h"
#include "gfx/RenderCommand.h"
#include "gfx/Shader.h"
#include "gfx/ShaderManager.h"
#include "PickerRenderPass.h"
#include "renderers/Renderer.h"
#include "renderers/QuadRenderer.h"
#include <glad/glad.h>

namespace BHive
{
	void PickerRenderPass::Init()
	{
		mShaders[0] = ShaderManager::Get().Load("PickMesh.glsl");
		mShaders[1] = ShaderManager::Get().Load("PickQuad.glsl");
		mShaders[2] = ShaderManager::Get().Load("PickLine.glsl");

		ASSERT(mShaders);
	}

	void PickerRenderPass::Render(const FMeshRenderDatas &data)
	{
		mFrambuffer->Bind();

		RenderCommand::Clear();

		static int clear_id = -1;
		mFrambuffer->ClearAttachment(0, &clear_id);

		/*mShaders[0]->Bind();
		for (const auto &obj : data)
		{
			mShaders[0]->SetUniform("constants.uEntityID", obj->EntityID);
			Renderer::Draw(obj);
		}
		mShaders[0]->UnBind();

		mShaders[1]->Bind();
		QuadRenderer::Flush();
		mShaders[1]->UnBind();

		mShaders[2]->Bind();
		LineRenderer::Flush();
		mShaders[2]->UnBind();*/

		int pixel_id = -1;
		mFrambuffer->ReadPixel(0, mMousePos.x, mMousePos.y, 1, 1, &pixel_id);

		LOG_INFO("Picked ID: {}", pixel_id);
		if (pixel_id != -1)
		{
			auto lambda = [pixel_id](const auto &pair) { return pair->EntityID == pixel_id; };
			auto picked = std::find_if(data.begin(), data.end(), lambda);

			auto render_data = picked != data.end() ? *picked : nullptr;
			OnEntityPicked.invoke(pixel_id, render_data);
		}
		else
		{
			OnEntityPicked.invoke(-1, nullptr);
		}

		mFrambuffer->UnBind();

		mEnabled = false;
	}

	void PickerRenderPass::Pick(const glm::uvec2 mousePos)
	{
		mMousePos = mousePos;

		if (mMousePos.x < mSize.x && mMousePos.y < mSize.y)
		{
			mEnabled = true;
		}
	}

	void PickerRenderPass::CreateFramebuffer()
	{
		FramebufferSpecification spec{};
		spec.Size = mSize;

		spec.Attachments.attach({.Format = EFormat::RED_INTEGER, .WrapMode = EWrapMode::CLAMP_TO_EDGE}).attach({EFormat::DEPTH24_STENCIL8});
		mFrambuffer = Framebuffer::Create(spec);
	}

} // namespace BHive