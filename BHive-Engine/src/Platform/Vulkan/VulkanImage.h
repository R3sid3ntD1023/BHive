#pragma once

#include "IVulkanTextureInterface.h"
#include "Platform/Vulkan/ImageViewBuilder.h"
#include "Platform/Vulkan/VulkanMemory.h"
#include "core/delegates/EventDelegate.h"
#include "gfx/resources/ImageCopyRegion.h"
#include "gfx/resources/ImageSubresourceRange.h"

namespace BHive
{
	struct ImageCreateInfo
	{
		vk::ImageCreateInfo ImageCI{};

		vk::ImageViewCreateInfo ViewCI{};

		vk::SamplerCreateInfo SamplerCI{};

		// Engine metadata
		std::string DebugName{};
		uint32_t BytesPerPixel = 0;
	};

	DECLARE_EVENT(OnDestroyed, ResourceID)

	class VulkanImage : public INativeObject
	{
	public:
		~VulkanImage();

		void Initialize(const ImageCreateInfo &info);

		// ImageCI unused
		void Initialize(const vk::Image &img, const ImageCreateInfo &info);

		void Upload(const void *data, size_t size, ImageCopyRegion region, ImageSubresourceRange range = {});

		void Transition(vk::CommandBuffer cmd, ImageState newState, ImageSubresourceRange range = {});

		void GenerateMipMaps(vk::CommandBuffer cmd);

		ImageState GetState(uint32_t mip, uint32_t layer) const;

		vk::Image GetImage() const;

		vk::ImageView GetFullView() const;

		vk::ImageView GetView(uint32_t layer, uint32_t mip) const;

		vk::Sampler GetSampler() const;

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

		ImageCreateInfo mInfo;

		bool mRawImage{0};
	};

} // namespace BHive