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
		spec.Attachments.AddColorAttachment(FFramebufferTexture{color_attachment});
		spec.DebugName = "ImageDebugger";
		mFB = Framebuffer::Create(spec);

		auto shader = ShaderManager::Get("DebugTextureViewer.glsl");
		auto state = Pipeline::GetDefaultGraphicsPipelineState();
		state.ShaderProgram = shader;
		state.ColorAttachmentFormats = {EFormat::RGBA8};
		PipelineRegistry::Register(DEBUG_TEXTURE_PIPELINE, state);

		mMaterial = CreateRef<Material>();
		mMaterial->SetPipeline(PipelineRegistry::Get(DEBUG_TEXTURE_PIPELINE));
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
			auto &entry = mTextureEntries[mSelected];
			int type = entry.IsCube ? 1 : 0;
			int mip = mSelectedMip;
			int mipLevels = entry.MipLevels;
			int face = entry.IsCube ? mSelectedFace : 0;
			int layers = entry.IsCube ? 1 : entry.Layers;
			const char *uniform = entry.IsCube ? "u_TexCube" : "u_Tex2D";
			const auto& tex = entry.Tex;

			mMaterial->Set("u_Type", type);
			mMaterial->Set("u_Mip", mip);
			mMaterial->Set("u_Face", face);
			mMaterial->SetTexture(uniform, tex);
			mMaterial->Submit();

			ImageSubresourceRange range{mip, mipLevels, face, layers};

			auto& pass = renderer.BeginPass("ImageDebugger", EPassType::OffScreen);

			pass.BeginPhase("ImageDebugger : Render To Qaud", EPhaseType::Graphics);
			pass.Push(mFB);
			pass.Push(tex, EImageAccess::ColorRead, range);
			pass.Emplace<CmdSetClearColor>()(0.f, 0.0f, .0f, 1.0f);
			pass.Emplace<CmdBindMaterial>()(mMaterial.get());
			pass.Emplace<CmdDrawFullScreen>()();
			pass.EndPhase();

			pass.BeginPhase("ImageDebugger : Transition to Read", EPhaseType::Transfer);
			pass.Push(mFB->GetColorAttachment(), EImageAccess::ColorRead);
			pass.EndPhase();

			renderer.EndPass();

			mHasRendered = true;
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
					if(ImGui::Selectable(mTextureEntries[i].Name.c_str(), selected))
					{
						mSelected = i;
						mHasRendered = false;
					}
					if (selected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			if (mSelected >= 0 && mHasRendered)
			{
				auto &entry = mTextureEntries[mSelected];

				mSelectedMip = std::clamp(mSelectedMip, 0, (int)entry.MipLevels - 1);
				ImGui::SliderInt("Mip", &mSelectedMip, 0, (int)entry.MipLevels - 1);

				if (entry.IsCube)
				{
					mSelectedFace = std::clamp(mSelectedFace, 0, 5);
					ImGui::SliderInt("Face", &mSelectedFace, 0, 5);
				}

				auto texID = ImGuiLayer::GetTextureID(*mFB->GetColorAttachment());
				ImGui::Image(texID, ImVec2(float(mSize.x), float(mSize.y)));
			}
		}

		ImGui::End();
	}

} // namespace BHive