#include "PostProcessAllocator.h"

namespace BHive
{
	void PostProcessAllocator::Resize(const glm::uvec2 &size)
	{
		if (size == mSize)
			return;

		mSize = size;

		CreateAcesOutput();
		CreateBloomCompositeOutput();
		CreateBloomOutput();
		CreateColorGradeOutput();
		CreateTempTextures();
	}

	void PostProcessAllocator::CreateAcesOutput()
	{
		FTextureCreateInfo info{};
		info.WrapMode = EWrapMode::CLAMP_TO_EDGE;
		info.Format = EFormat::RGBA8;
		info.Roles |= ETextureRole::RenderTarget;
		info.DebugName = "AcesOutput";
		mAcesOutput = Texture2D::Create(mSize, info);
	}

	void PostProcessAllocator::CreateBloomCompositeOutput()
	{
		FTextureCreateInfo info{};
		info.WrapMode = EWrapMode::CLAMP_TO_EDGE;
		info.Format = EFormat::RGBA32F;
		info.Roles |= ETextureRole::ComputeWrite;
		info.DebugName = "SceneBloomComposite";
		mBloomCompositeOutput = Texture2D::Create(mSize, info);
	}

	void PostProcessAllocator::CreateColorGradeOutput()
	{
		FTextureCreateInfo info{};
		info.Format = EFormat::RGBA8;
		info.Roles |= ETextureRole::RenderTarget;
		info.WrapMode = EWrapMode::CLAMP_TO_EDGE;
		info.DebugName = "ColorGradingTex";

		mColorGradeOutput = Texture2D::Create(mSize, info);
	}

	void PostProcessAllocator::CreateBloomOutput()
	{
		mBloomMipSizes.clear();
		mBloomMipSizes.reserve(mBloomMipCount);

		glm::uvec2 halfSize = glm::max(mSize / 2u, glm::uvec2(1u));

		glm::uvec2 mipSize = halfSize;

		for (uint32_t i = 0; i < mBloomMipCount; i++)
		{
			mBloomMipSizes.push_back(mipSize);
			mipSize = glm::max(mipSize / 2u, glm::uvec2(1u));
		}

		FTextureCreateInfo info{};
		info.WrapMode = EWrapMode::CLAMP_TO_EDGE;
		info.Format = EFormat::RGBA32F;
		info.Roles |= ETextureRole::ComputeWrite;
		info.MipLevels = std::min(ComputeMipCount(halfSize), mBloomMipCount);
		info.DebugName = "BloomMipChain";

		mBloomOutput = Texture2D::Create(halfSize, info);
	}

	void PostProcessAllocator::CreateTempTextures()
	{
		mTempTextures.clear();
		mTempTextures.reserve(mTempTextureCount);

		for (uint32_t i = 0; i < mTempTextureCount; i++)
		{
			FTextureCreateInfo info{};
			info.Format = EFormat::RGBA16F;
			info.WrapMode = EWrapMode::CLAMP_TO_EDGE;
			info.Roles |= ETextureRole::ComputeWrite;
			info.DebugName = "TempTexture_" + std::to_string(i);

			mTempTextures.push_back(Texture2D::Create(mSize, info));
		}
	}

	uint32_t PostProcessAllocator::ComputeMipCount(glm::uvec2 size)
	{
		uint32_t levels = 1;
		while (size.x > 1 || size.y > 1)
		{
			size = glm::max(size / 2u, glm::uvec2(1u));
			levels++;
		}

		return levels;
	}
} // namespace BHive