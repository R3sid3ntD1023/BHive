#pragma once

#include "gfx/Texture.h"


namespace BHive
{
	class BHIVE_API PostProcessAllocator
	{
	public:
		PostProcessAllocator() = default;

		void Resize(const glm::uvec2 &size);

		Ref<Texture> GetAcesOuput() const { return mAcesOutput; }

		Ref<Texture> GetBloomOuput() const { return mBloomOutput; }

		Ref<Texture> GetBloomCompositeOuput() const { return mBloomCompositeOutput; }

		Ref<Texture> GetColorGradeOuput() const { return mColorGradeOutput; }

		uint32_t GetBloomMipCount() const { return mBloomMipCount; }

		glm::uvec2 GetBloomMipSize(uint32_t mip) const
		{
			ASSERT(mip < mBloomMipSizes.size());
			return mBloomMipSizes[mip];
		}

		inline Ref<Texture> GetTemp(uint32_t index) const { return (index >= mTempTextures.size()) ? mTempTextures[index] : Ref<Texture>(0); }

		inline Ref<Texture> GetTemp(uint32_t index, EFormat format) const
		{
			if (index >= mTempTextures.size())
				return nullptr;

			if (mTempTextures.at(index)->GetInfo().Format == format)
				return mTempTextures[index];

			return nullptr;
		}

	private:
		void CreateAcesOutput();
		void CreateBloomCompositeOutput();
		void CreateColorGradeOutput();
		void CreateBloomOutput();
		void CreateTempTextures();

		static uint32_t ComputeMipCount(glm::uvec2 size);

	private:
		glm::uvec2 mSize{0, 0};

		Ref<Texture> mAcesOutput;
		Ref<Texture> mBloomOutput;
		Ref<Texture> mBloomCompositeOutput;
		Ref<Texture> mColorGradeOutput;

		std::vector<Ref<Texture>> mTempTextures;

		std::vector<glm::uvec2> mBloomMipSizes;

		uint32_t mBloomMipCount = 5;

		uint32_t mTempTextureCount = 4;
	};
}