#include "VulkanSwapChain.h"
#include "VulkanUtils.h"

namespace BHive
{

	VulkanSwapChain::VulkanSwapChain()
		: mDevice(VulkanCore::GetLogicalDevice())
	{
		
	}

	VulkanSwapChain::~VulkanSwapChain()
	{
		LOG_TRACE("SwapChain Destructor Called")

		/*mSwapChain.clear();
		mPresentSemaphores.clear();
		mRenderFinishedSemaphores.clear();
		mInFlightFences.clear();
		mImages.clear();
		mImageViews.clear();*/
		
	}

	void VulkanSwapChain::Init(vk::raii::SurfaceKHR &surface, const VulkanSwapChainCreateInfo &create_info)
	{
		mImages.clear();
		mImageViews.clear();
		mPresentSemaphores.clear();
		mRenderFinishedSemaphores.clear();
		mInFlightFences.clear();

		mImageLayouts.clear();

		mExtent = VulkanUtils::ChooseSwapExtent(create_info.Capabilities, create_info.Width, create_info.Height);
		mImageFormat = VulkanUtils::ChooseSwapSurfaceFormat(create_info.Formats);
		mMinImageCount = VulkanUtils::ChooseMinImageCount(create_info.Capabilities);

		//LOG_TRACE("Swapchain Format: {} ; ColorSpace: {}", vk::to_string(mImageFormat.format), vk::to_string(mImageFormat.colorSpace));

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

		mImageLayouts.resize(mImages.size(), vk::ImageLayout::eUndefined);

		for (uint32_t i = 0; i < mImages.size(); i++)
		{
			mRenderFinishedSemaphores.emplace_back(mDevice, vk::SemaphoreCreateInfo());			
		}

		for (uint32_t i = 0; i < VulkanCore::MAX_FRAMES_IN_FLIGHT; i++)
		{
			mPresentSemaphores.emplace_back(mDevice, vk::SemaphoreCreateInfo());
			mInFlightFences.emplace_back(mDevice, vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
		}
	}

	void VulkanSwapChain::WaitForFence(uint32_t frame)
	{
		vk::Fence fence = mInFlightFences[frame];
		while (vk::Result::eTimeout == mDevice.waitForFences(fence, VK_TRUE, UINT64_MAX))
			;
		mDevice.resetFences(fence);
	}

	vk::ResultValue<uint32_t> VulkanSwapChain::AquireNextImage(uint32_t frame)
	{
		vk::Semaphore present = mPresentSemaphores[frame];
		return mSwapChain.acquireNextImage(UINT64_MAX, present, VK_NULL_HANDLE);
	}

	vk::Result VulkanSwapChain::Present(const vk::raii::CommandBuffer &buffers, uint32_t imageIndex, uint32_t frame)
	{
		
		vk::Fence fence = mInFlightFences[frame];
		vk::Semaphore wait_semaphore = mPresentSemaphores[frame];
		vk::Semaphore signal_semaphore = mRenderFinishedSemaphores[imageIndex];

		vk::SemaphoreSubmitInfo wait_info(wait_semaphore, 0, vk::PipelineStageFlagBits2::eAllCommands);
		vk::CommandBufferSubmitInfo cmd_submit_info(buffers);
		vk::SemaphoreSubmitInfo signal_info(signal_semaphore, 0, vk::PipelineStageFlagBits2::eAllCommands);

		const vk::SubmitInfo2 submitInfo2({}, wait_info, cmd_submit_info, signal_info);

		auto &graphics_queue = VulkanCore::GetQueueFamilies().GraphicsQueue;
		graphics_queue.submit2(submitInfo2, fence);

		const vk::PresentInfoKHR presentInfoKHR(signal_semaphore, *mSwapChain, imageIndex);
		return (vk::Result)vkQueuePresentKHR(*graphics_queue, &*presentInfoKHR);
		//return graphics_queue.presentKHR(presentInfoKHR);
	}

	vk::ImageLayout &VulkanSwapChain::GetImageLayout(uint32_t imageIndex)
	{
		return mImageLayouts[imageIndex];
	}

} // namespace BHive