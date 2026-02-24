#include "Bloom.h"
#include "core/profiler/CPUGPUProfiler.h"
#include "gfx/Image.h"
#include "gfx/Shader.h"
#include "gfx/ShaderManager.h"
#include "gfx/Texture.h"
#include "gfx/UniformBuffer.h"

namespace BHive
{
#define BLOOM_ITERATIONS 5

	void BloomRenderPass::Init()
	{
		mPreFilterShader = ShaderManager::Get().Load("PreFilter.glsl");
		mDownSamplerShader = ShaderManager::Get().Load("DownSample.glsl");
		mUpSamplerShader = ShaderManager::Get().Load("UpSample.glsl");
		mCombineShader = ShaderManager::Get().Load("CombineTex.glsl");

		mMipMaps.resize(BLOOM_ITERATIONS);
	}

	void BloomRenderPass::CreateResizableObjects(const glm::uvec2 &size)
	{
		PostProcessRenderPass::CreateResizableObjects(size);

		FTextureCreateInfo specs{};
		specs.Format = EFormat::RGBA32F;
		specs.WrapMode = EWrapMode::CLAMP_TO_EDGE;

		mPreFilterTexture = Texture2D::Create(size, specs);

		specs.Format = EFormat::RGBA32F;
		mOutputTexture = Texture2D::Create(size, specs);

		glm::uvec2 mps = size;
		for (auto &mip : mMipMaps)
		{
			mip = Texture2D::Create(mps, specs);

			mps /= 2;
			mps = glm::max({1, 1}, mps);
		}
	}

	void BloomRenderPass::Process(const Ref<Texture> &texture)
	{
		//mPreFilterShader->Bind();
		//texture->Bind();
		//Image(mPreFilterTexture).Bind(0, EImageAccess::WRITE);
		//mPreFilterShader->SetUniform("constants.u_FilterThreshold", mSettings.mFilterThreshold);
		//mPreFilterShader->Dispatch(mPreFilterTexture->GetWidth(), mPreFilterTexture->GetHeight());

		//mPreFilterShader->UnBind();

		//// downsample image
		//mDownSamplerShader->Bind();

		//auto current_texture = mPreFilterTexture;
		//for (auto &mip : mMipMaps)
		//{
		//	glm::ivec2 size = {mip->GetWidth(), mip->GetHeight()};
		//	current_texture->Bind();
		//	Image(mip).Bind(0, EImageAccess::WRITE);
		//	mDownSamplerShader->Dispatch(size.x, size.y);

		//	current_texture = mip;
		//}
		//mDownSamplerShader->UnBind();

		//// upsample image
		//mUpSamplerShader->Bind();
		//mUpSamplerShader->SetUniform("constants.u_FilterRadius", mSettings.mFilterRadius);

		//for (size_t i = mMipMaps.size() - 1; i > 0; i--)
		//{
		//	const auto &mip = mMipMaps[i];
		//	const auto &next_mip = mMipMaps[i - 1];

		//	mip->Bind();
		//	Image(next_mip).Bind(0, EImageAccess::WRITE);
		//	mUpSamplerShader->Dispatch(next_mip->GetWidth(), next_mip->GetHeight());
		//}

		//mUpSamplerShader->UnBind();

		//mCombineShader->Bind();

		//texture->Bind(0);
		//mMipMaps[0]->Bind(1);
		//Image(mOutputTexture).Bind(0, EImageAccess::WRITE);
		//mCombineShader->Dispatch(mOutputTexture->GetWidth(), mOutputTexture->GetHeight());
		//mCombineShader->UnBind();
	}

	void BloomRenderPass::SetBloomSettings(const FBloomSettings &settings)
	{
		mSettings = settings;
	}

	Ref<Texture> BloomRenderPass::GetOutputTexture() const
	{
		return mOutputTexture;
	}

} // namespace BHive