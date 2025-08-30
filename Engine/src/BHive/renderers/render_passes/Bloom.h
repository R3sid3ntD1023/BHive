#pragma once

#include "PostProcessRenderPass.h"

namespace BHive
{
	class Shader;

	struct FBloomSettings
	{
		glm::vec4 mFilterThreshold{0.2126, 0.7152, 0.0722, 1.0};
		float mFilterRadius{0.001f};
	};

	class BloomRenderPass : public PostProcessRenderPass
	{
		using MipMaps = std::vector<Ref<Texture>>;

	public:
		BloomRenderPass() = default;

		void Init() override;

		void CreateResizableObjects(const glm::uvec2 &size) override;

		virtual void Process(const Ref<Texture> &textures) override;

		FBloomSettings &GetSettings() { return mSettings; }

		void SetBloomSettings(const FBloomSettings &settings);

		Ref<Texture> GetOutputTexture() const override;

	private:
		FBloomSettings mSettings;
		MipMaps mMipMaps;

		Ref<Texture> mPreFilterTexture;
		Ref<Texture> mOutputTexture;

		Ref<Shader> mPreFilterShader;
		Ref<Shader> mDownSamplerShader;
		Ref<Shader> mUpSamplerShader;
		Ref<Shader> mCombineShader;
	};
} // namespace BHive