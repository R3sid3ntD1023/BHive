#pragma once

#include "Platform/Vulkan/VulkanMemory.h"
#include "Platform/Vulkan/ImageViewBuilder.h"
#include "gfx/resources/ImageCopyRegion.h"
#include "gfx/resources/ImageSubresourceRange.h"

namespace BHive
{
	struct ImageCreateInfo
	{
		vk::ImageCreateInfo ImageCI{};

		vk::ImageViewCreateInfo ViewCI{};

		vk::SamplerCreateInfo SamplerCI{};

		//Engine metadata
		EViewTopology ViewTopology{};
		ETextureRole Role;
		std::string DebugName{};
		uint32_t BytesPerPixel = 0;
	};

	class VulkanImage
	{
	public:
		VulkanImage() = default;

		void Initialize(const ImageCreateInfo &info);

		//ImageCI unused
		void Initialize(const vk::Image &img, const ImageCreateInfo &info);

		void Upload(const void *data, size_t size, ImageCopyRegion region, ImageSubresourceRange range = {});

		void Transition(vk::raii::CommandBuffer &cmd, ImageState newState, ImageSubresourceRange range = {});

		void GenerateMipMaps(vk::raii::CommandBuffer& cmd);

		void Destroy();

		const GPUImage &Native() const { return mImage; }

		ImageState GetState(uint32_t mip, uint32_t layer) const; 

		void DebugPrintState();

	private:
		ImageState InitialStateFromUsage(vk::ImageUsageFlags usage, vk::Format format);

	private:
		GPUImage mImage{};

		ImageStateTracker mStateTracker;

		ImageCreateInfo mInfo;

		bool mRawImage{0};
	};

}