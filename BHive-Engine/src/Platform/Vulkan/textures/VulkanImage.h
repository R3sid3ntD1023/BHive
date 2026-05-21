#pragma once

#include "Platform/Vulkan/VulkanMemory.h"
#include "Platform/Vulkan/VulkanImageRegions.h"
#include "Platform/Vulkan/ImageViewBuilder.h"

namespace BHive
{
	struct ImageCreateInfo
	{
		vk::ImageCreateInfo ImageCI{};

		vk::ImageViewCreateInfo ViewCI{};

		vk::SamplerCreateInfo SamplerCI{};

		//Engine metadata
		EViewTopology ViewTopology{};
		std::string DebugName{};
		uint32_t BytesPerPixel = 0;
	};

	class VulkanImage
	{
		static inline const ImageState DefaultState = {vk::ImageLayout::eUndefined, vk::AccessFlagBits2::eNone, vk::PipelineStageFlagBits2::eTopOfPipe};

	public:
		VulkanImage() = default;

		void Initialize(const ImageCreateInfo &info, ImageState initial = DefaultState);

		//ImageCI unused
		void Initialize(const vk::Image& img, const ImageCreateInfo &info, ImageState initial = DefaultState);

		void Upload(const void *data, size_t size, const ImageCopyRegion &region = {}, const ImageSubresource &sub = {});

		void Transition(vk::raii::CommandBuffer &cmd, const ImageState &newState, const ImageSubresource &sub = {0, 0, 1});

		void Destroy();

		const GPUImage &Native() const { return mImage; }

	private:
		GPUImage mImage{};

		ImageStateTracker mStateTracker;

		vk::ImageAspectFlags mAspect{};

		bool mRawImage{0};
	};

}