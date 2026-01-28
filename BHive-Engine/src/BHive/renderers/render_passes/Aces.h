#pragma once

#include "PostProcessRenderPass.h"

namespace BHive
{
	class Shader;
	class Image;

	class AcesRenderPass : public PostProcessRenderPass
	{

	public:
		AcesRenderPass() = default;

		void Init() override;

		void CreateResizableObjects(const glm::uvec2 &size) override;

		virtual void Process(const Ref<Texture> &texture) override;

		Ref<Texture> GetOutputTexture() const override { return mOutputTexture; }

		const char *GetName() const override { return "Aces"; }

	private:
		Ref<Texture> mOutputTexture;
		Ref<Shader> mComputeShader;
	};
} // namespace BHive