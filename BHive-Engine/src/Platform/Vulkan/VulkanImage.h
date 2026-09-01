#pragma once

#include "Platform/Vulkan/ImageViewBuilder.h"
#include "Platform/Vulkan/VulkanMemory.h"
#include "VKInterfaces.h"
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
		EViewTopology ViewTopology{};
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

		ResourceID GetResourceID() const { return mImage.Image; }

		const GPUImage &Native() const { return mImage; }

		ImageState GetState(uint32_t mip, uint32_t layer) const;

		void DebugPrintState();

		operator bool() const { return mImage; }

		OnDestroyedEvent OnDestroyed;

	private:
		ImageState InitialStateFromUsage(vk::ImageUsageFlags usage, vk::Format format);

	private:
		GPUImage mImage{};

		ImageStateTracker mStateTracker;

		ImageCreateInfo mInfo;

		bool mRawImage{0};
	};

} // namespace BHive