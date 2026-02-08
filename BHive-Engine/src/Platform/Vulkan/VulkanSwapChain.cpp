#include "VulkanSwapChain.h"
#include "VulkanUtils.h"

namespace BHive
{

	VulkanSwapChain::VulkanSwapChain()
		: mDevice(VulkanCore::GetLogicalDevice())
	{
		
	}

	void VulkanSwapChain::Init(vk::raii::SurfaceKHR &surface, const VulkanSwapChainCreateInfo &create_info)
	{
		mExtent = VulkanUtils::ChooseSwapExtent(create_info.Capabilities, create_info.Width, create_info.Height);
		mImageFormat = VulkanUtils::ChooseSwapSurfaceFormat(create_info.Formats);
		mMinImageCount = VulkanUtils::ChooseMinImageCount(create_info.Capabilities);

		auto present_mode = VulkanUtils::ChooseSwapPresentMode(create_info.PresentModes);
		vk::SwapchainCreateInfoKHR swap_chain_create_info(
			{}, *surface, mMinImageCount, mImageFormat.format, mImageFormat.colorSpace, mExtent, 1, vk::ImageUsageFlagBits::eColorAttachment, vk::SharingMode::eExclusive, {},
			create_info.Capabilities.currentTransform, vk::CompositeAlphaFlagBitsKHR::eOpaque, present_mode, true, create_info.OldSwapChain, nullptr);

		mSwapChain = mDevice.createSwapchainKHR(swap_chain_create_info);
		mImages = mSwapChain.getImages();

		ASSERT(mImageViews.empty())

		vk::ImageViewCreateInfo view_info({}, {}, vk::ImageViewType::e2D, mImageFormat.format, {}, {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});

		for (auto &image : mImages)
		{
			view_info.image = image;
			mImageViews.emplace_back(mDevice, view_info);
		}

		// create sync objects
		for (uint32_t i = 0; i < mImages.size(); i++)
		{
			
			mRenderFinishedSemaphores.emplace_back(mDevice, vk::SemaphoreCreateInfo());
		}

		for (uint32_t i = 0; i < VulkanCore::MAX_FRAMES_IN_FLIGHT; i++)
		{
			mPresetCompleteSemaphores.emplace_back(mDevice, vk::SemaphoreCreateInfo());
			mInFlightFences.emplace_back(mDevice, vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
		}

		mImageLayouts.resize(mImages.size(), vk::ImageLayout::eUndefined);
	}

	vk::ResultValue<uint32_t> VulkanSwapChain::AquireNextImage(uint32_t frame)
	{
		while (vk::Result::eTimeout == mDevice.waitForFences(*mInFlightFences[frame], VK_TRUE, UINT64_MAX))
			;
		mDevice.resetFences(*mInFlightFences[frame]);
		return mSwapChain.acquireNextImage(UINT64_MAX, *mPresetCompleteSemaphores[frame], nullptr);
	}

	vk::Result VulkanSwapChain::Present(const vk::raii::CommandBuffer &buffers, uint32_t imageIndex, uint32_t frame)
	{
		vk::SemaphoreSubmitInfo wait_info(*mPresetCompleteSemaphores[frame], 0, vk::PipelineStageFlagBits2::eAllCommands);
		vk::CommandBufferSubmitInfo cmd_submit_info(buffers);
		vk::SemaphoreSubmitInfo signal_info(*mRenderFinishedSemaphores[imageIndex], 0, vk::PipelineStageFlagBits2::eAllCommands );

		const vk::SubmitInfo2 submitInfo2({}, wait_info, cmd_submit_info, signal_info);

		auto &graphics_queue = VulkanCore::GetQueueFamilies().GraphicsQueue;
		graphics_queue.submit2(submitInfo2, *mInFlightFences[frame]);

		const vk::PresentInfoKHR presentInfoKHR(*mRenderFinishedSemaphores[imageIndex], *mSwapChain, imageIndex);
		auto result = graphics_queue.presentKHR(presentInfoKHR);


		return result;
	}

	vk::ImageLayout &VulkanSwapChain::GetImageLayout(uint32_t imageIndex)
	{
		ASSERT(imageIndex < mImageLayouts.size());
		return mImageLayouts[imageIndex];
	}

} // namespace BHive