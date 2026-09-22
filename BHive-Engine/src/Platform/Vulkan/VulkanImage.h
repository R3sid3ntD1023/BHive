#pragma once

#include "IVulkanTextureInterface.h"
#include "Platform/Vulkan/ImageViewBuilder.h"
#include "Platform/Vulkan/VulkanMemory.h"
#include "core/delegates/EventDelegate.h"
#include "gfx/resources/ImageCopyRegion.h"
#include "gfx/resources/ImageSubresourceRange.h"

namespace BHive
{
	DECLARE_EVENT(OnDestroyed, ResourceID)

	class VulkanImage : public INativeObject
	{
	public:
		~VulkanImage();

		void Initialize(const vk::ImageCreateInfo &imgInfo, const vk::ImageViewCreateInfo &viewInfo, const vk::SamplerCreateInfo &smpInfo);

		// ImageCI unused
		void Initialize(
			const vk::Image &img, uint32_t layers, uint32_t levels, vk::ImageUsageFlags usage, const vk::ImageViewCreateInfo &viewInfo, const vk::SamplerCreateInfo &smpInfo
		);

		void Upload(const void *data, size_t size, ImageCopyRegion region, ImageSubresourceRange range = {});

		void Transition(vk::CommandBuffer cmd, ImageState newState, ImageSubresourceRange range = {});

		void GenerateMipMaps(vk::CommandBuffer cmd);

		ImageState GetState(uint32_t mip, uint32_t layer) const;

		vk::Image GetImage() const;

		vk::ImageView GetFullView() const;

		vk::ImageView GetView(uint32_t layer, uint32_t mip) const;

		vk::Sampler GetSampler() const;

		void SetDebugName(const std::string &dbgName);

		void Clear();

		operator bool() const { return mImage; }

		OnDestroyedEvent OnDestroyed;

	private:
		ImageState InitialStateFromUsage(vk::ImageUsageFlags usage, vk::Format format);

	private:
		ResourceID mImage;

		ImageViews mViews;

		ResourceID mSampler;

		ImageStateTracker mStateTracker;

		uint32_t mLayers = 0, mLevels = 0;

		vk::Extent3D mExtents{0, 0, 1};

		vk::ImageAspectFlags mImageAspect = vk::ImageAspectFlagBits::eColor;
	};

} // namespace BHive