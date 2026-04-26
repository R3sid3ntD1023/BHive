#include "GPUComponents.h"
#include "VulkanBackend.h"

namespace BHive
{
	const vk::ImageView &DefaultViewComponent::Get() const
	{
		return VulkanBackend::GetGPUResourceManager().GetImageView(View);
	}

	const vk::ImageView &MipViewComponent::Get(uint32_t layer, uint32_t mip) const
	{
		return VulkanBackend::GetGPUResourceManager().GetImageView(Views[layer][mip]);
	}

	const vk::ImageView &FaceMipViewComponent::Get(uint32_t layer, uint32_t face, uint32_t mip) const
	{
		return VulkanBackend::GetGPUResourceManager().GetImageView(Views[layer][face][mip]);
	}

	const vk::ImageView &CubeMipViewComponent::Get(uint32_t layer, uint32_t mip) const
	{
		return VulkanBackend::GetGPUResourceManager().GetImageView(Views[layer][mip]);
	}

	const vk::Sampler &SamplerComponent::Get() const
	{
		return VulkanBackend::GetGPUResourceManager().GetSampler(Sampler);
	}

	void StateTrackingComponent::Init(uint32_t layers, uint32_t mips, const ImageState &initial)
	{
		MipStates.resize(layers);
		for (uint32_t layer = 0; layer < layers; layer++)
		{
			MipStates[layer].assign(mips, initial);
		}
	}

	ImageState &StateTrackingComponent::Get(uint32_t layer, uint32_t mip)
	{
		return MipStates[layer][mip];
	}
} // namespace BHive