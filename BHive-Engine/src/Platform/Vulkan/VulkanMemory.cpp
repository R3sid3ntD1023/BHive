#include "VulkanMemory.h"
#include "VulkanBackend.h"
#include "VulkanUtils.h"

namespace BHive
{
	GPUImage::~GPUImage()
	{
		if (IsValid())
			VulkanBackend::GetGPUResourceManager().DestroyImage(*this);
	}

	const vk::Image GPUImage::GetImage() const
	{

		return VulkanBackend::GetGPUResourceManager().GetImage(Image);
	}

	const vk::Sampler GPUImage::GetSampler() const
	{
		if (!Sampler)
			return nullptr;
		return VulkanBackend::GetGPUResourceManager().GetSampler(Sampler);
	}

	vk::ImageView GPUImage::GetView(uint32_t layer, uint32_t face, uint32_t mip) const
	{
		auto &rm = VulkanBackend::GetGPUResourceManager();

		ViewKey key{layer, face, mip};
		return rm.GetImageView(Views.Views.at(key));
	}

	void ImageStateTracker::Initialize(uint32_t layers, uint32_t mips, const ImageState &initial)
	{
		ASSERT(layers > 0 && mips > 0, "layers and levels must be 1 or greater!")

		MipStates.resize(layers);
		for (uint32_t layer = 0; layer < layers; layer++)
		{
			MipStates[layer].assign(mips, initial);
		}
	}

	ImageState &ImageStateTracker::Get(uint32_t layer, uint32_t mip)
	{
		return MipStates[layer][mip];
	}

	const ImageState &ImageStateTracker::Get(uint32_t layer, uint32_t mip) const
	{
		return MipStates[layer][mip];
	}
} // namespace BHive
