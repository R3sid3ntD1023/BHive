#include "ImageDebugger.h"
#include "gfx/Texture.h"
#include "gfx/Framebuffer.h"
#include "gfx/ShaderManager.h"
#include "gfx/material/Material.h"
#include "gfx/Pipeline.h"
#include "core/layers/ImGuiLayer.h"
#include "gfx/mesh/primitives/Quad.h"
#include "imgui.h"
#include "gfx/renderers/Renderer.h"

namespace BHive
{
	#define DEBUG_TEXTURE_PIPELINE "DebugTextureViewerPipeline"

	void ImageDebugger::Initialize(const glm::uvec2 &size)
	{
		mSize = size;

		FTextureCreateInfo color_attachment{};
		color_attachment.Format = EFormat::RGBA8;
		color_attachment.WrapMode = EWrapMode::CLAMP_TO_EDGE;

		FramebufferSpecification spec{};
		spec.Size = size;
		spec.Attachments.attach(color_attachment);
		mFB = Framebuffer::Create(spec);

		auto shader = ShaderManager::Get("DebugTextureViewer.glsl");
		auto state = Pipeline::GetDefaultGraphicsPipelineState();
		state.ShaderProgram = shader;
		state.ColorAttachmentFormats = {EFormat::RGBA8};
		PipelineRegistry::Register(DEBUG_TEXTURE_PIPELINE, state);

		mMaterial = CreateRef<Material>();
		mMaterial->SetPipeline(PipelineRegistry::Get(DEBUG_TEXTURE_PIPELINE));

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

		if (mTextures.contains(name))
		{
			auto &entry = mTextureEntries[mTextures[name]];
			entry = e;
			return;
		}

		mTextureEntries.push_back(e);
		mTextures.emplace(name, mTextureEntries.size() - 1);
	}

	void ImageDebugger::OnRender(Renderer &renderer)
	{
		if (mSelected >= 0)
		{

			renderer.BeginPass("ImageDebugger", EPassType::OffScreen);

			mFB->Bind();

			renderer.ClearColor(0, 0, 0, 1);
			renderer.Clear();

			auto &entry = mTextureEntries[mSelected];
			int type = entry.IsCube ? 1 : 0;
			int mip = mSelectedMip;
			int face = entry.IsCube ? mSelectedFace : 0;

			mMaterial->Set("u_Type", type);
			mMaterial->Set("u_Mip", mip);
			mMaterial->Set("u_Face", face);
			mMaterial->SetTexture(entry.IsCube ? "u_TexCube" : "u_Tex2D", entry.Tex);
			mMaterial->Submit();

			renderer.DrawElements(ETopologyMode::Triangles, mQuad->GetVertexArray().get());
			
			mFB->UnBind();

			renderer.EndPass();
		}
	}

	void ImageDebugger::OnGuiRender()
	{
		if (!mFB || mTextureEntries.empty())
			return;

		if (ImGui::Begin("Image View Debugger"))
		{
			if (ImGui::BeginCombo("Texture", (mSelected >= 0 ? mTextureEntries[mSelected].Name.c_str() : "<none>")))
			{
				for (int i = 0; i < (int)mTextureEntries.size(); i++)
				{
					bool selected = (i == mSelected);
					if (ImGui::Selectable(mTextureEntries[i].Name.c_str(), selected))
						mSelected = i;
					if (selected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			if (mSelected >= 0)
			{
				auto &entry = mTextureEntries[mSelected];

				mSelectedMip = std::clamp(mSelectedMip, 0, (int)entry.MipLevels - 1);
				ImGui::SliderInt("Mip", &mSelectedMip, 0, (int)entry.MipLevels - 1);

				if (entry.IsCube)
				{
					mSelectedFace = std::clamp(mSelectedFace, 0, 5);
					ImGui::SliderInt("Face", &mSelectedFace, 0, 5);
				}

				auto texID = ImGuiLayer::GetTextureID(*mFB->GetColorAttachment(0));
				ImGui::Image(texID, ImVec2(float(mSize.x), float(mSize.y)));
			}
		}

		ImGui::End();
	}

} // namespace BHive