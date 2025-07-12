#pragma once

#include "gfx/Color.h"
#include "math/Math.h"
#include "PostProcessor.h"

namespace BHive
{
	class Shader;

	struct FBloomSettings
	{
		glm::vec4 mFilterThreshold{0.2126, 0.7152, 0.0722, 1.0};
		float mFilterRadius{0.001f};
	};

	class Bloom : public PostProcessor
	{
		using MipMaps = std::vector<Ref<Texture>>;

	public:
		Bloom(uint32_t iterations, uint32_t width, uint32_t height, const FBloomSettings &data);

		virtual Ref<Texture> Process(const Ref<Texture> &textures) override;

		virtual void Resize(uint32_t width, uint32_t height) override;

		FBloomSettings &GetSettings() { return mSettings; }

		const MipMaps &GetMipMaps() const { return mMipMaps; }

		REFLECTABLEV(PostProcessor)

	private:
		void Initialize(uint32_t width, uint32_t height);
		void Reset();

	private:
		FBloomSettings mSettings;
		MipMaps mMipMaps;

		Ref<Texture> mPreFilterTexture;
		Ref<Texture> mOutputTexture;

		Ref<Shader> mPreFilterShader;
		Ref<Shader> mDownSamplerShader;
		Ref<Shader> mUpSamplerShader;
		Ref<Shader> mCombineShader;

		glm::uvec2 mSize{};
	};

	REFLECT_EXTERN(Bloom)
} // namespace BHive