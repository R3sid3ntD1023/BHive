#include "VulkanMemory.h"
#include "VulkanUtils.h"

namespace BHive
{
	namespace Vulkan
	{
		void Image::Transition(vk::raii::CommandBuffer &cmd, const ImageState &newState)
		{
			VulkanUtils::TransitionImageLayout(cmd, ImageSrc, State.Layout, newState.Layout, State.Access, newState.Access, State.Stage, newState.Stage, Aspect);
			State = newState;
		}

		void AllocatedImage::Transition(vk::raii::CommandBuffer &cmd, const ImageState &newState)
		{
			VulkanUtils::TransitionImageLayout(cmd, Image, State.Layout, newState.Layout, State.Access, newState.Access, State.Stage, newState.Stage, Aspect);
			State = newState;
		}

	} // namespace Vulkan
}

