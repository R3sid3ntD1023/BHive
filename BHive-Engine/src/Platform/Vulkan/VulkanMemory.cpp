#include "VulkanMemory.h"
#include "VulkanUtils.h"
#include "VulkanBackend.h"
#include "gfx/RenderCommand.h"
#include "VulkanRendererAPI.h"

namespace BHive
{
	void Image::CreateView(const ImageViewDesc &desc)
	{
		ViewHandle = VulkanBackend::GetGPUResourceManager().CreateImageView(ImageSrc, desc);
	}

	const vk::ImageView &Image::GetView() const
	{
		return VulkanBackend::GetGPUResourceManager().GetImageView(ViewHandle);
	}

	void Image::Transition(vk::raii::CommandBuffer &cmd, const ImageState &newState, const ImageSubresource& sub)
	{
		VulkanUtils::TransitionImageLayout(cmd, ImageSrc, State.Layout, newState.Layout, State.Access, newState.Access, State.Stage, newState.Stage, Aspect, sub);
		State = newState;
	}

	const vk::Image& AllocatedImage::GetImage() const
	{
		return VulkanBackend::GetGPUResourceManager().GetImage(ImageHandle);
	}

	const vk::ImageView& AllocatedImage::GetView() const
	{
		return VulkanBackend::GetGPUResourceManager().GetImageView(ViewHandle);
	}

	const vk::Sampler& AllocatedImage::GetSampler() const
	{
		return VulkanBackend::GetGPUResourceManager().GetSampler(SamplerHandle);
	}

	void AllocatedImage::Transition(vk::raii::CommandBuffer &cmd, const ImageState &newState, const ImageSubresource &sub)
	{
		for (uint32_t layer = sub.BaseArrayLayer; layer < sub.BaseArrayLayer + sub.LayerCount; layer++)
		{
			auto &State = LayerStates[layer];
			ImageSubresource layerSub = sub;
			layerSub.BaseArrayLayer = layer;
			layerSub.LayerCount = 1;

			VulkanUtils::TransitionImageLayout(cmd, GetImage(), State.Layout, newState.Layout, State.Access, newState.Access, State.Stage, newState.Stage, Aspect, layerSub);
			State = newState;
		}
	}

	Image AllocatedImage::CreateImage()
	{
		auto &vk_image = VulkanBackend::GetGPUResourceManager().GetImage(ImageHandle);

		Image image{};
		image.SetImage(vk_image);
		image.SetAspect(Aspect);
		return image;
	}

	const vk::Buffer &AllocatedBuffer::GetBuffer() const
	{
		return VulkanBackend::GetGPUResourceManager().GetBuffer(Buffer);
	}

	AllocatedBuffer::~AllocatedBuffer()
	{
		auto api = RenderCommand::GetRendererAPI<VulkanRendererAPI>();
		api->QueueDeletion(
			[buffer = Buffer](uint32_t)
			{
				VulkanBackend::GetGPUResourceManager().DestroyBuffer(buffer);
			});
	}
} 

