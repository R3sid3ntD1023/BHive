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
		mImages.clear();
		mSwapChain = nullptr;
	}

	void VulkanSwapChain::Init(vk::raii::SurfaceKHR &surface, const VulkanSwapChainCreateInfo &create_info)
	{
		mExtent = VulkanUtils::ChooseSwapExtent(create_info.Capabilities, create_info.Width, create_info.Height);
		mImageFormat = VulkanUtils::ChooseSwapSurfaceFormat(create_info.Formats);

		mMinImageCount = VulkanUtils::ChooseMinImageCount(create_info.Capabilities);

		auto present_mode = VulkanUtils::ChooseSwapPresentMode(create_info.PresentModes);
		vk::SwapchainCreateInfoKHR swap_chain_create_info(
			{}, *surface, mMinImageCount, mImageFormat.format, mImageFormat.colorSpace, mExtent, 1, vk::ImageUsageFlagBits::eColorAttachment, vk::SharingMode::eExclusive, {},
			create_info.Capabilities.currentTransform, vk::CompositeAlphaFlagBitsKHR::eOpaque, present_mode, true, nullptr);

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
		for (size_t i = 0; i < mImages.size(); i++)
		{
			mPresetCompleteSemaphores.emplace_back(mDevice, vk::SemaphoreCreateInfo());
			mRenderFinishedSemaphores.emplace_back(mDevice, vk::SemaphoreCreateInfo());
		}

		for (size_t i = 0; i < VulkanCore::MAX_FRAMES_IN_FLIGHT; i++)
		{
			mInFlightFences.emplace_back(mDevice, vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
		}
	}

	vk::Image &VulkanSwapChain::GetImage(uint32_t index)
	{
		return mImages[index];
	}

	vk::raii::ImageView &VulkanSwapChain::GetImageView(uint32_t index)
	{
		return mImageViews[index];
	}

	vk::ResultValue<uint32_t> VulkanSwapChain::AquireNextImage()
	{

		while (vk::Result::eTimeout == mDevice.waitForFences(*mInFlightFences[mCurrentFrame], VK_TRUE, UINT64_MAX))
			;

		return mSwapChain.acquireNextImage(UINT64_MAX, *mPresetCompleteSemaphores[mSemaphoreIndex], nullptr);
	}

	vk::Result VulkanSwapChain::SubmitCommandBuffers(const std::vector<vk::CommandBuffer> &buffers, uint32_t imageIndex)
	{
		vk::PipelineStageFlags waitStages(vk::PipelineStageFlagBits::eColorAttachmentOutput);
		const vk::SubmitInfo submitInfo(*mPresetCompleteSemaphores[mSemaphoreIndex], waitStages, buffers, *mRenderFinishedSemaphores[imageIndex]);

		auto &graphics_queue = VulkanCore::GetQueueFamilies().GraphicsQueue;
		graphics_queue.submit(submitInfo, *mInFlightFences[mCurrentFrame]);

		const vk::PresentInfoKHR presentInfoKHR(*mRenderFinishedSemaphores[imageIndex], *mSwapChain, imageIndex);
		const VkPresentInfoKHR info = presentInfoKHR;
		auto result = vkQueuePresentKHR(*graphics_queue, &info);

		mSemaphoreIndex = (mSemaphoreIndex + 1) % mPresetCompleteSemaphores.size();
		mCurrentFrame = (mCurrentFrame + 1) % VulkanCore::MAX_FRAMES_IN_FLIGHT;

		return (vk::Result)result;
	}

	void VulkanSwapChain::ResetCommandBuffer(const vk::raii::CommandBuffer &buffer)
	{
		mDevice.resetFences(*mInFlightFences[mCurrentFrame]);
		buffer.reset();
	}

} // namespace BHive