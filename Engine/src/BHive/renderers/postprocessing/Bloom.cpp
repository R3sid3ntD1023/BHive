#include "Bloom.h"
#include "gfx/Shader.h"
#include "gfx/Texture.h"
#include "glad/glad.h"
#include "gfx/BindlessTexture.h"
#include "gfx/TextureUtils.h"
#include "core/profiler/CPUGPUProfiler.h"
#include "shaders/DownSample.h"
#include "shaders/UpSample.h"
#include "shaders/PreFilter.h"
#include "gfx/UniformBuffer.h"
#include "gfx/ShaderManager.h"

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

		{
			auto &reflection_data = mUpSamplerShader->GetRelectionData();
			auto &buffer = reflection_data.UniformBuffers.at("UpSampler");
			mUpSampleBuffer = UniformBuffer::Create(buffer.Binding, buffer.Size);
		}

		{
			auto &reflection_data = mDownSamplerShader->GetRelectionData();
			auto &buffer = reflection_data.UniformBuffers.at("DownSampler");
			mDownSampleBuffer = UniformBuffer::Create(buffer.Binding, buffer.Size);
		}

		{
			auto &reflection_data = mPreFilterShader->GetRelectionData();
			auto &buffer = reflection_data.UniformBuffers.at("PreFilter");
			mPreFilterBuffer = UniformBuffer::Create(buffer.Binding, buffer.Size);
		}

		Initialize(width, height);
	}

	Ref<Texture> Bloom::Process(const Ref<Texture> &texture)
	{
		CPU_PROFILER_SCOPED("Bloom::Process");

		mPreFilterShader->Bind();

		struct PreFilter
		{
			uint64_t Src;
			uint64_t Out;
			glm::vec4 Threshold;
		};

		PreFilter data{
			.Src = texture->GetResourceHandle(),
			.Out = mPreFilterTexture->GetImageHandle(),
			.Threshold = mSettings.mFilterThreshold};

		mPreFilterBuffer->SetData(data);
		mPreFilterShader->Dispatch(mPreFilterTexture->GetWidth(), mPreFilterTexture->GetHeight());

		mPreFilterShader->UnBind();

		// downsample
		mDownSamplerShader->Bind();

		struct FDownSamplerData
		{
			uint64_t Src;
			uint64_t Out;
		};

		auto current_texture = mPreFilterTexture;
		for (auto &mip : mMipMaps)
		{
			glm::ivec2 size = {mip->GetWidth(), mip->GetHeight()};

			FDownSamplerData data{.Src = current_texture->GetResourceHandle(), .Out = mip->GetImageHandle()};
			mDownSampleBuffer->SetData(data);
			mDownSamplerShader->Dispatch(size.x, size.y);

			current_texture = mip;
		}
		mDownSamplerShader->UnBind();

		mUpSamplerShader->Bind();

		struct FUpSamplerData
		{
			uint64_t Src;
			uint64_t Out;
			alignas(16) float Filter;
		};

		for (size_t i = mMipMaps.size() - 1; i > 0; i--)
		{
			const auto &mip = mMipMaps[i];
			const auto &next_mip = mMipMaps[i - 1];

			FUpSamplerData data = {
				.Src = mip->GetResourceHandle(), .Out = next_mip->GetImageHandle(), .Filter = mSettings.mFilterRadius};

			mUpSampleBuffer->SetData(data);
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
		specs.ImageAccess = EImageAccess::READ_WRITE;

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