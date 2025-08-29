#include "Bloom.h"
#include "core/profiler/CPUGPUProfiler.h"
#include "gfx/Image.h"
#include "gfx/Shader.h"
#include "gfx/ShaderManager.h"
#include "gfx/textures/Texture2D.h"
#include "gfx/UniformBuffer.h"

namespace BHive
{

	Bloom::Bloom(uint32_t iterations, uint32_t width, uint32_t height, const FBloomSettings &data)
		: mSettings(data),
		  mSize(width, height)
	{
		mPreFilterShader = ShaderManager::Get().Load("PreFilter.glsl");
		mDownSamplerShader = ShaderManager::Get().Load("DownSample.glsl");
		mUpSamplerShader = ShaderManager::Get().Load("UpSample.glsl");
		mCombineShader = ShaderManager::Get().Load("CombineTex.glsl");

		mMipMaps.resize(iterations);

		Initialize(width, height);
	}

	Ref<Texture> Bloom::Process(const Ref<Texture> &texture)
	{
		Image output_image;
		output_image.SetTexture(mPreFilterTexture);

		mPreFilterShader->Bind();
		texture->Bind();
		output_image.Bind(0, EImageAccess::WRITE);
		mPreFilterShader->SetUniform("constants.u_FilterThreshold", mSettings.mFilterThreshold);
		mPreFilterShader->Dispatch(mPreFilterTexture->GetWidth(), mPreFilterTexture->GetHeight());

		mPreFilterShader->UnBind();

		// downsample
		mDownSamplerShader->Bind();

		auto current_texture = mPreFilterTexture;
		for (auto &mip : mMipMaps)
		{
			glm::ivec2 size = {mip->GetWidth(), mip->GetHeight()};
			output_image.SetTexture(mip);

			current_texture->Bind();
			output_image.Bind(0, EImageAccess::WRITE);
			mDownSamplerShader->Dispatch(size.x, size.y);

			current_texture = mip;
		}
		mDownSamplerShader->UnBind();

		mUpSamplerShader->Bind();
		mUpSamplerShader->SetUniform("constants.u_FilterRadius", mSettings.mFilterRadius);

		for (size_t i = mMipMaps.size() - 1; i > 0; i--)
		{
			const auto &mip = mMipMaps[i];
			const auto &next_mip = mMipMaps[i - 1];
			output_image.SetTexture(next_mip);

			mip->Bind();
			output_image.Bind(0, EImageAccess::WRITE);
			mUpSamplerShader->Dispatch(next_mip->GetWidth(), next_mip->GetHeight());
		}

		mUpSamplerShader->UnBind();

		mCombineShader->Bind();

		texture->Bind(0);
		mMipMaps[0]->Bind(1);

		output_image.SetTexture(mOutputTexture);
		output_image.Bind(0, EImageAccess::WRITE);
		mCombineShader->Dispatch(mSize.x, mSize.y);
		mCombineShader->UnBind();

		return mOutputTexture;
	}

	void Bloom::Resize(uint32_t width, uint32_t height)
	{
		mSize = {width, height};

		Initialize(width, height);
	}

	void Bloom::Initialize(uint32_t width, uint32_t height)
	{

		Reset();

		FTextureCreateInfo specs{};
		specs.InternalFormat = EFormat::R11_G11_B10;
		specs.WrapMode = EWrapMode::CLAMP_TO_BORDER;

		mPreFilterTexture = CreateRef<Texture2D>(width, height, specs);

		specs.InternalFormat = EFormat::RGBA32F;
		mOutputTexture = CreateRef<Texture2D>(width, height, specs);

		glm::uvec2 mps = mSize;
		for (auto &mip : mMipMaps)
		{
			mip = CreateRef<Texture2D>(mps.x, mps.y, specs);

			mps /= 2;
			if (mps.x < 1)
				mps.x = 1;
			if (mps.y < 1)
				mps.y = 1;
		}
	}

	void Bloom::Reset()
	{
		for (auto &mip : mMipMaps)
			mip.reset();

		mPreFilterTexture.reset();
	}

	REFLECT(FBloomSettings)
	{
		BEGIN_REFLECT(FBloomSettings)
		REFLECT_PROPERTY("FilterThreshold", mFilterThreshold)
		REFLECT_PROPERTY("FilterRadius", mFilterRadius);
	}

	REFLECT(Bloom)
	{
		BEGIN_REFLECT(Bloom)
		REFLECT_PROPERTY("Settings", mSettings);
	}
} // namespace BHive