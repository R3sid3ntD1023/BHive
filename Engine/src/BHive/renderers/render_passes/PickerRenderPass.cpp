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
		mShader = ShaderManager::Get().Load("Picker.glsl");
		ASSERT(mShader);
	}

	void PickerRenderPass::Render(const FMeshRenderDatas &data)
	{
		mFrambuffer->Bind();

		RenderCommand::Clear();

		static int clear_id = -1;
		mFrambuffer->ClearAttachment(0, &clear_id);

		mShader->Bind();
		for (const auto &[dist, obj] : data)
		{
			mShader->SetUniform("constants.uEntityID", obj->ObjectInfo.EntityID);
			Renderer::Draw(obj);
		}
		mShader->UnBind();

		int pixel_id = -1;
		mFrambuffer->ReadPixel(0, mMousePos.x, mMousePos.y, 1, 1, &pixel_id);

		LOG_INFO("Picked ID: {}", pixel_id);
		if (pixel_id != -1)
		{
			auto lambda = [pixel_id](const std::pair<float, Ref<FMeshRenderData>> &pair) { return pair.second->ObjectInfo.EntityID == pixel_id; };
			auto picked = std::find_if(data.begin(), data.end(), lambda);

			auto render_data = picked != data.end() ? picked->second : nullptr;
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
		spec.Width = mSize.x;
		spec.Height = mSize.y;

		spec.Attachments.attach({.InternalFormat = EFormat::RED_INTEGER, .WrapMode = EWrapMode::CLAMP_TO_EDGE}).attach({EFormat::DEPTH24_STENCIL8});
		mFrambuffer = CreateRef<Framebuffer>(spec);
	}

} // namespace BHive