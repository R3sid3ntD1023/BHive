#include "VulkanMemory.h"
#include "VulkanUtils.h"
#include "VulkanBackend.h"

namespace BHive
{
	
	const vk::Image GPUImage::GetImage() const
	{

		return VulkanBackend::GetGPUResourceManager().GetImage(Image);
	}

	const vk::Sampler GPUImage::GetSampler() const
	{
		if (!Sampler)
			return nullptr;
		return VulkanBackend::GetGPUResourceManager().GetSampler(*Sampler);
	}

	void GPUImage::Transition(vk::raii::CommandBuffer &cmd, const ImageState &newState, const ImageSubresource &sub)
	{
		ASSERT(State.MipStates.size(), "{}", DebugName);

		for (uint32_t layer = sub.BaseArrayLayer; layer < sub.BaseArrayLayer + sub.LayerCount; layer++)
		{
			for (uint32_t mip = sub.MipLevel; mip < sub.MipLevel + sub.LevelCount; mip++)
			{
				ASSERT(State.MipStates[layer].size(), "{}", DebugName);

				auto &oldState = State.MipStates[layer][mip];

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

	vk::ImageView GPUImage::GetView(uint32_t layer, uint32_t face, uint32_t mip) const
	{
		auto &rm = VulkanBackend::GetGPUResourceManager();

		if (!Views.Faces.empty())
		{
			return rm.GetImageView(Views.Faces[layer][face][mip]);
		}

		if (!Views.CubeMips.empty())
		{
			return rm.GetImageView(Views.CubeMips[layer][mip]);
		}

		if (!Views.Mips.empty())
		{
			return rm.GetImageView(Views.Mips[layer][mip]);
		}

		return rm.GetImageView(Views.Default);
	}

	const vk::Buffer &AllocatedBuffer::GetBuffer() const
	{
		return VulkanBackend::GetGPUResourceManager().GetBuffer(Buffer);
	}

	void ImageStateTracker::Initialize(uint32_t layers, uint32_t mips, const ImageState &initial)
	{
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
} 

