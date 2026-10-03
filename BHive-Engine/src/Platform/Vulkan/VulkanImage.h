#pragma once

#include "IVulkanTextureInterface.h"
#include "Platform/Vulkan/ImageViewBuilder.h"
#include "Platform/Vulkan/MemoryAllocator.h"
#include "Platform/Vulkan/VulkanMemory.h"
#include "core/delegates/EventDelegate.h"
#include "gfx/resources/ImageCopyRegion.h"
#include "gfx/resources/ImageSubresourceRange.h"

namespace BHive
{
	DECLARE_EVENT(OnDestroyed)

	class VulkanImage
	{
	public:
		VulkanImage() = default;
		VulkanImage(const VulkanImage &) = delete;
		VulkanImage &operator=(const VulkanImage &) = delete;
		VulkanImage(VulkanImage &&other) noexcept;
		VulkanImage &operator=(VulkanImage &&other) noexcept;

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

		operator bool() const { return mImage != VK_NULL_HANDLE; }

		OnDestroyedEvent OnDestroyed;

	private:
		vk::raii::Image mOwnedImage{nullptr};
		vk::Image mImage = VK_NULL_HANDLE;
		vk::raii::Sampler mSampler{nullptr};
		MemoryAllocator *mAllocator = nullptr;
		MemoryAllocation mAllocation;

		vk::ImageViewCreateInfo mViewInfo{};
		mutable ImageViews mViews;

		ImageStateTracker mStateTracker;

		uint32_t mLayers = 0, mLevels = 0;
		vk::Extent3D mExtents{0, 0, 1};
		vk::ImageAspectFlags mImageAspect = vk::ImageAspectFlagBits::eColor;
	};

} // namespace BHive