#include "ImageDebugger.h"
#include "gfx/Framebuffer.h"
#include "gfx/Pipeline.h"
#include "gfx/ShaderManager.h"
#include "gfx/Texture.h"
#include "gfx/factories/GFXFactories.h"
#include "gfx/imgui/IImGuiProvider.h"
#include "gfx/material/Material.h"
#include "gfx/renderers/Renderer.h"
#include "imgui.h"

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
		mFB = FramebufferFactory::Create(spec);

		mMaterial = MaterialFactory::Create("DebugTextureViewer.glsl");
	}

	void ImageDebugger::Shutdown()
	{
		mTextureEntries.clear();
	}

	void ImageDebugger::RegisterTexture(const std::string &name, TexturePtr handle)
	{
		auto tex = handle.As<Texture>();

		FDebugTextureEntry e{};
		e.Name = name;
		e.Tex = handle;
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
			const auto &tex = entry.Tex;

			auto material = mMaterial.As<Material>();
			material->SetParam("u_Type", MaterialParam(type)).SetParam("u_Mip", MaterialParam(mip)).SetParam("u_Face", MaterialParam(face)).SetTexture(uniform, TextureBinding(tex));

			ImageSubresourceRange range{mip, mipLevels, face, layers};

			auto &pass = renderer.BeginPass("ImageDebugger", EPassType::OffScreen);

			pass.BeginPhase("ImageDebugger : Render To Qaud", EPhaseType::Graphics);
			pass.UseFramebuffer(mFB);
			pass.UseTexture(tex, EImageUsage::ColorRead, range);
			pass.Emplace<CmdBindMaterial>()(mMaterial.As<Material>());
			pass.Emplace<CmdDrawFullScreen>()();
			pass.EndPhase();

			pass.BeginPhase("ImageDebugger : Transition to Read", EPhaseType::Transfer);
			pass.UseTexture(mFB.As<Framebuffer>()->GetColorAttachment(), EImageUsage::ColorRead);
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
					if (ImGui::Selectable(mTextureEntries[i].Name.c_str(), selected))
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

				auto id = IImGuiTextureProvider::GetID(*mFB.As<Framebuffer>()->GetColorAttachment().As<Texture>());
				ImGui::Image(id, ImVec2(float(mSize.x), float(mSize.y)));
			}
		}

		ImGui::End();
	}

} // namespace BHive