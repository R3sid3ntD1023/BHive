#include "VulkanMemory.h"
#include "VulkanUtils.h"
#include "VulkanBackend.h"
#include "gfx/RenderCommand.h"
#include "VulkanRendererAPI.h"
#include "GPUComponents.h"

namespace BHive
{
	
	const vk::Image& GPUImage::GetImage() const
	{
		return VulkanBackend::GetGPUResourceManager().GetImage(ImageHandle);
	}

	void GPUImage::Transition(vk::raii::CommandBuffer &cmd, const ImageState &newState, const ImageSubresource &sub)
	{
		auto *stateComponent = GetComponent<StateTrackingComponent>();
		if (!stateComponent)
		{
			LOG_ERROR("StateTrackingComponent is required for image transitions - {}", DebugName);
			return;
		}

		for (uint32_t layer = sub.BaseArrayLayer; layer < sub.BaseArrayLayer + sub.LayerCount; layer++)
		{
			for (uint32_t mip = sub.MipLevel; mip < sub.MipLevel + sub.LevelCount; mip++)
			{
				auto &oldState = stateComponent->MipStates[layer][mip];

				ImageSubresource layerSub = sub;
				layerSub.BaseArrayLayer = layer;
				layerSub.LayerCount = 1;
				layerSub.MipLevel = mip;
				layerSub.LevelCount = 1;

				VulkanUtils::TransitionImageLayout(cmd, GetImage(), oldState.Layout, newState.Layout, oldState.Access, newState.Access, oldState.Stage, newState.Stage, Aspect, layerSub);
				oldState = newState;
			}
			
		}
	}

	vk::ImageView GPUImage::GetView(uint32_t layer, uint32_t face, uint32_t mip)
	{
		// 1. Face+mip views (cube or cube array)
		if (auto *faceMips = GetComponent<FaceMipViewComponent>())
			return faceMips->Get(layer, face, mip);

		// 2. Cube mip views (cube or cube array)
		if (auto *cubeMips = GetComponent<CubeMipViewComponent>())
			return cubeMips->Get(layer, mip);

		// 3. 2D / 2D array mip views
		if (auto *mips = GetComponent<MipViewComponent>())
			return mips->Get(layer, mip);

		// 4. Fallback: default view
		return GetComponent<DefaultViewComponent>()->Get();
	}

	const vk::Buffer &AllocatedBuffer::GetBuffer() const
	{
		return VulkanBackend::GetGPUResourceManager().GetBuffer(Buffer);
	}
} 

