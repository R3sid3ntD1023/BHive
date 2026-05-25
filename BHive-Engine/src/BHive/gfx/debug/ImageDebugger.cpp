#include "ImageDebugger.h"
#include "gfx/Texture.h"
#include "gfx/Framebuffer.h"
#include "gfx/ShaderManager.h"
#include "gfx/RenderCommand.h"
#include "gfx/material/Material.h"
#include "gfx/Pipeline.h"
#include "core/layers/ImGuiLayer.h"
#include "gfx/mesh/primitives/Quad.h"
#include "imgui.h"

namespace BHive
{
	void ImageDebugger::Initialize(const glm::uvec2 &size)
	{
		mSize = size;

		FramebufferSpecification spec{};
		spec.Size = size;
		spec.Attachments.attach(FTextureCreateInfo{.Format = EFormat::RGBA8, .WrapMode = EWrapMode::CLAMP_TO_EDGE});
		mFB = Framebuffer::Create(spec);

		auto shader = ShaderManager::Get().Load(ENGINE_SHADER_PATH "/debug/DebugTextureViewer.glsl");
		auto state = Pipeline::GetDefaultGraphicsPipelineState();
		state.ShaderProgram = shader;
		state.ColorAttachmentFormats = {EFormat::RGBA8};

		mPipeline = Pipeline::Create();
		mPipeline->Init(state);

		mMaterial = CreateRef<Material>(mPipeline);

		mQuad = CreateRef<PQuad>();
	}

	void ImageDebugger::RegisterTexture(const std::string &name, const Ref<Texture> &tex)
	{
		FDebugTextureEntry e{};
		e.Name = name;
		e.Tex = tex;
		e.IsCube = Cast<TextureCube>(tex) != nullptr;
		e.MipLevels = tex->GetInfo().MipLevels;
		e.Layers = tex->GetInfo().ArrayLayers;
		mTextures.push_back(e);
	}

	void ImageDebugger::OnGuiRender()
	{
		if (!mFB || mTextures.empty())
			return;

		if (ImGui::Begin("Image View Debugger"))
		{
			if (ImGui::BeginCombo("Texture", (mSelected >= 0 ? mTextures[mSelected].Name.c_str() : "<none>")))
			{
				for (int i = 0; i < (int)mTextures.size(); i++)
				{
					bool selected = (i == mSelected);
					if (ImGui::Selectable(mTextures[i].Name.c_str(), selected))
						mSelected = i;
					if (selected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			if (mSelected >= 0)
			{
				auto &entry = mTextures[mSelected];

				mSelectedMip = std::clamp(mSelectedMip, 0, (int)entry.MipLevels - 1);
				ImGui::SliderInt("Mip", &mSelectedMip, 0, (int)entry.MipLevels - 1);

				if (entry.IsCube)
				{
					mSelectedFace = std::clamp(mSelectedFace, 0, 5);
					ImGui::SliderInt("Face", &mSelectedFace, 0, 5);
				}

				mFB->Bind();

				RenderCommand::ClearColor(0, 0, 0, 1);
				RenderCommand::Clear();
				RenderCommand::SetViewport(0, 0, mSize.x, mSize.y);

				int type = entry.IsCube ? 1 : 0;
				int mip = mSelectedMip;
				int face = entry.IsCube ? mSelectedFace : 0;

				mMaterial->Set("u_Type", type);
				mMaterial->Set("u_Mip", mip);
				mMaterial->Set("u_Face", face);
				mMaterial->SetTexture(entry.IsCube ? "u_TexCube" : "u_Tex2D", entry.Tex);
				mMaterial->Submit();

				RenderCommand::DrawElements(ETopologyMode::Triangles, mQuad->GetVertexArray());

				mFB->UnBind();

				auto texID = ImGuiLayer::GetTextureID(*mFB->GetColorAttachment(0));
				ImGui::Image(texID, ImVec2(float(mSize.x), float(mSize.y)),ImVec2(0,1), ImVec2(1,0));
			}
		}

		ImGui::End();
	}

} // namespace BHive