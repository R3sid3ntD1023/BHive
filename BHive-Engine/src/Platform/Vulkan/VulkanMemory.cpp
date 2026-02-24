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


		void AllocatedImage::Transition(vk::raii::CommandBuffer &cmd, const ImageState &newState, const ImageSubresource &sub)
		{
			ImageSubresource s = sub;
			if (s.LayerCount == 1 && s.BaseArrayLayer == 0 && State.Layout != vk::ImageLayout::eUndefined)
			{
				s.LayerCount = ArrayLayers;
			}

				s.LayerCount = UINT32_MAX;
			VulkanUtils::TransitionImageLayout(cmd, Image, State.Layout, newState.Layout, State.Access, newState.Access, State.Stage, newState.Stage, Aspect, sub);
			State = newState;
		}

	} // namespace Vulkan
}

