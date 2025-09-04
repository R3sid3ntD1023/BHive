#pragma once

#include "core/Core.h"
#include "PostProcessRenderPass.h"
#include "RenderPass.h"

namespace BHive
{
	class Shader;
	class Texture;

	class OutlineRenderPass : public RenderPass
	{
	public:
		void Init() override;

		// Inherited via RenderPass
		void Render(const FMeshRenderDatas &data) override;

		void CreateFramebuffer() override;

		bool IsEnabled() const override { return RenderPass::IsEnabled() && mSelectedRenderData; }

		void SetSelected(const Ref<FMeshRenderData> &data);

		Ref<Texture> GetOutputTetxure() const;

	private:
		Ref<FMeshRenderData> mSelectedRenderData;
		Ref<Shader> mOutlineMeshShader;
		Ref<Shader> mOutlineQuadShader;
	};

	class OutlinePostProcessRenderPass : public PostProcessRenderPass
	{
	public:
		void Init() override;

		void CreateResizableObjects(const glm::uvec2 &size) override;

		void Process(const Ref<Texture> &texture) override;

		Ref<Texture> GetOutputTexture() const override { return mOutputTexture; }

		bool IsEnabled() const override { return PostProcessRenderPass::IsEnabled() && mIsSelected; }

		void SetSelected(bool selected);

		void SetOutlineTexture(const Ref<Texture> &outline_texture);

		const char *GetName() const override { return "Outline"; }

	private:
		bool mIsSelected{false};
		Ref<Texture> mOutlineTexture;
		Ref<Texture> mColorGradingLUTTexture;
		Ref<Texture> mOutputTexture;
		Ref<Texture> mOutlineOutput;
		Ref<Shader> mOutlineColorGradingShader;
		Ref<Shader> mBoxBlurShader;
	};
} // namespace BHive