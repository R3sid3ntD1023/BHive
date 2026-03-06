#include "VulkanMemory.h"
#include "VulkanUtils.h"
#include "VulkanBackend.h"

namespace BHive
{
	void Image::CreateView(const ImageViewDesc &desc)
	{
		VulkanBackend::GetGPUResourceManager().CreateImageView(*this, desc);
	}

	void Image::Transition(vk::raii::CommandBuffer &cmd, const ImageState &newState)
	{
		VulkanUtils::TransitionImageLayout(cmd, ImageSrc, State.Layout, newState.Layout, State.Access, newState.Access, State.Stage, newState.Stage, Aspect, {});
		State = newState;
	}

	void AllocatedImage::Transition(vk::raii::CommandBuffer &cmd, const ImageState &newState, const ImageSubresource &sub)
	{
		for (uint32_t layer = sub.BaseArrayLayer; layer < sub.BaseArrayLayer + sub.LayerCount; layer++)
		{
			auto &State = LayerStates[layer];
			VulkanUtils::TransitionImageLayout(cmd, Image, State.Layout, newState.Layout, State.Access, newState.Access, State.Stage, newState.Stage, Aspect, sub);
			State = newState;
		}
	}
} 

