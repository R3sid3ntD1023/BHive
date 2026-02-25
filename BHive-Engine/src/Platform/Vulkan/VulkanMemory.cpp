#include "VulkanMemory.h"
#include "VulkanUtils.h"

namespace BHive
{
	namespace Vulkan
	{
		void Image::Transition(vk::raii::CommandBuffer &cmd, const ImageState &newState)
		{
			VulkanUtils::TransitionImageLayout(cmd, ImageSrc, State.Layout, newState.Layout, State.Access, newState.Access, State.Stage, newState.Stage, Aspect, {});
			State = newState;
		}


		const vk::DescriptorImageInfo AllocatedImage::GetDescriptor() const
		{
			return vk::DescriptorImageInfo(Sampler, View, vk::ImageLayout::eShaderReadOnlyOptimal);
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

	} // namespace Vulkan
}

