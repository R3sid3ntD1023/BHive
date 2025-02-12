#include "Bloom.h"
#include "gfx/Shader.h"
#include "gfx/Texture.h"
#include "glad/glad.h"
#include "gfx/BindlessTexture.h"
#include "gfx/TextureUtils.h"

namespace BHive
{
	Bloom::Bloom(uint32_t iterations, uint32_t width, uint32_t height, const FBloomSettings &data)
		: mSettings(data),
		  mSize(width, height)
	{
		mPreFilterShader = ShaderLibrary::Load(ENGINE_PATH "/data/shaders/PreFilter.glsl");
		mDownSamplerShader = ShaderLibrary::Load(ENGINE_PATH "/data/shaders/DownSample.glsl");
		mUpSamplerShader = ShaderLibrary::Load(ENGINE_PATH "/data/shaders/UpSample.glsl");
		mMipMaps.resize(iterations);

		Initialize(width, height);
	}

	Ref<Texture> Bloom::Process(const Ref<Texture> &texture)
	{
		mPreFilterShader->Bind();
		mPreFilterShader->SetUniform("u_threshold", mSettings.mFilterThreshold);

		mPreFilterShader->SetBindlessTexture("u_src_texture", texture->GetResourceHandle());
		mPreFilterShader->SetBindlessTexture("uImgOutput", mPreFilterTexture->GetResourceHandle());

		mPreFilterShader->Dispatch(mPreFilterTexture->GetWidth(), mPreFilterTexture->GetHeight());

		mPreFilterShader->UnBind();

		// downsample
		mDownSamplerShader->Bind();

		auto current_texture = mPreFilterTexture;
		for (auto &mip : mMipMaps)
		{
			mDownSamplerShader->SetBindlessTexture("u_src_texture", current_texture->GetResourceHandle());
			mDownSamplerShader->SetBindlessTexture("uImgOutput", mip->GetImageHandle());

			glm::ivec2 size = {mip->GetWidth(), mip->GetHeight()};
			glm::ivec2 src_size = {current_texture->GetWidth(), current_texture->GetHeight()};
			mDownSamplerShader->SetUniform("u_src_resolution", src_size);
			mDownSamplerShader->Dispatch(size.x, size.y);

			current_texture = mip;
		}
		mDownSamplerShader->UnBind();

		mUpSamplerShader->Bind();
		mUpSamplerShader->SetUniform("u_filterRadius", mSettings.mFilterRadius);

		for (size_t i = mMipMaps.size() - 1; i > 0; i--)
		{
			const auto &mip = mMipMaps[i];
			const auto &next_mip = mMipMaps[i - 1];

			mUpSamplerShader->SetBindlessTexture("u_src_texture", mip->GetResourceHandle());
			mUpSamplerShader->SetBindlessTexture("uImgOutput", next_mip->GetImageHandle());

			mUpSamplerShader->Dispatch(next_mip->GetWidth(), next_mip->GetHeight());
		}

		mUpSamplerShader->UnBind();

		return mMipMaps[0];
	}

	void Bloom::Resize(uint32_t width, uint32_t height)
	{
		mSize = {width, height};

		Initialize(width, height);
	}

	void Bloom::Initialize(uint32_t width, uint32_t height)
	{

		Reset();

		FTextureSpecification specs{};
		specs.mFormat = EFormat::R11_G11_B10;
		specs.mWrapMode = EWrapMode::CLAMP_TO_BORDER;
		specs.mAccess = EAccess::READ_WRITE;

		mPreFilterTexture = Texture2D::Create(nullptr, width, height, specs);

		glm::uvec2 mps = mSize;
		for (auto &mip : mMipMaps)
		{
			mip = Texture2D::Create(nullptr, mps.x, mps.y, specs);

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
} // namespace BHive