#include "VulkanSwapChain.h"
#include "VulkanUtils.h"

namespace BHive
{
	VulkanSwapChain::~VulkanSwapChain()
	{
		mImages.clear();
		mSwapChain = nullptr;
	}

	void VulkanSwapChain::Init(vk::raii::Device& device, vk::raii::SurfaceKHR& surface, const VulkanSwapChainCreateInfo &create_info)
	{
		
		mExtent = VulkanUtils::ChooseSwapExtent(create_info.Capabilities, create_info.Width, create_info.Height);
		mImageFormat = VulkanUtils::ChooseSwapSurfaceFormat(create_info.Formats);

		auto minImageCount = VulkanUtils::ChooseMinImageCount(create_info.Capabilities);

		auto present_mode = VulkanUtils::ChooseSwapPresentMode(create_info.PresentModes);
		vk::SwapchainCreateInfoKHR swap_chain_create_info(
			{}, *surface, minImageCount, mImageFormat.format, mImageFormat.colorSpace, mExtent, 1, vk::ImageUsageFlagBits::eColorAttachment, vk::SharingMode::eExclusive,
			{}, create_info.Capabilities.currentTransform, vk::CompositeAlphaFlagBitsKHR::eOpaque, present_mode, true, nullptr);

		mSwapChain = vk::raii::SwapchainKHR(device, swap_chain_create_info);
		mImages = mSwapChain.getImages();

		ASSERT(mImageViews.empty())

		vk::ImageViewCreateInfo view_info({}, {}, vk::ImageViewType::e2D, mImageFormat.format, {}, {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});

		for (auto &image : mImages)
		{
			view_info.image = image;
			mImageViews.emplace_back(device, view_info);
		}
	}

	vk::Image& VulkanSwapChain::GetImage(uint32_t index)
	{
		return mImages[index];
	}

	vk::raii::ImageView &VulkanSwapChain::GetImageView(uint32_t index)
	{
		return mImageViews[index];
	}

	std::pair<vk::Result, uint32_t> VulkanSwapChain::AquireNextImage(vk::raii::Semaphore &semaphore)
	{
		return mSwapChain.acquireNextImage(UINT64_MAX, semaphore, nullptr);
	}
} // namespace BHive