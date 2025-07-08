#include "Bloom.h"
#include "core/profiler/CPUGPUProfiler.h"
#include "gfx/Shader.h"
#include "gfx/ShaderManager.h"
#include "gfx/textures/Texture2D.h"
#include "gfx/TextureUtils.h"
#include "gfx/UniformBuffer.h"
#include "glad/glad.h"
#include "shaders/DownSample.h"
#include "shaders/PreFilter.h"
#include "shaders/UpSample.h"

namespace BHive
{

	Bloom::Bloom(uint32_t iterations, uint32_t width, uint32_t height, const FBloomSettings &data)
		: mSettings(data),
		  mSize(width, height)
	{
		mPreFilterShader = ShaderManager::Get().Load("PreFilter", prefiler_comp);
		mDownSamplerShader = ShaderManager::Get().Load("DownSample", downsample_comp);
		mUpSamplerShader = ShaderManager::Get().Load("UpSample", upsample_comp);
		mMipMaps.resize(iterations);

		Initialize(width, height);

		auto uniform_buffer = mPreFilterShader->GetRelectionData().UniformBuffers.at("BloomSettings");
		mUniformBuffer = CreateRef<UniformBuffer>(uniform_buffer.Binding, uniform_buffer.Size);
	}

	Ref<Texture> Bloom::Process(const Ref<Texture> &texture)
	{
		mUniformBuffer->SetData(&mSettings, sizeof(FBloomSettings));

		mPreFilterShader->Bind();
		texture->Bind();
		mPreFilterTexture->BindAsImage(0, EImageAccess::WRITE);
		mPreFilterShader->Dispatch(mPreFilterTexture->GetWidth(), mPreFilterTexture->GetHeight());

		mPreFilterShader->UnBind();

		// downsample
		mDownSamplerShader->Bind();

		auto current_texture = mPreFilterTexture;
		for (auto &mip : mMipMaps)
		{
			glm::ivec2 size = {mip->GetWidth(), mip->GetHeight()};

			current_texture->Bind();
			mip->BindAsImage(0, EImageAccess::WRITE);
			mDownSamplerShader->Dispatch(size.x, size.y);

			current_texture = mip;
		}
		mDownSamplerShader->UnBind();

		mUpSamplerShader->Bind();

		for (size_t i = mMipMaps.size() - 1; i > 0; i--)
		{
			const auto &mip = mMipMaps[i];
			const auto &next_mip = mMipMaps[i - 1];

			mip->Bind();
			next_mip->BindAsImage(0, EImageAccess::WRITE);
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
		specs.InternalFormat = EFormat::R11_G11_B10;
		specs.WrapMode = EWrapMode::CLAMP_TO_BORDER;
		specs.ImageAccess = EImageAccess::READ_WRITE;

		mPreFilterTexture = CreateRef<Texture2D>(width, height, specs);

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
} // namespace BHive