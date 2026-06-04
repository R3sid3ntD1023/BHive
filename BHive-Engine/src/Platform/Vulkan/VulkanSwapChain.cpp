#include "VulkanSwapChain.h"
#include "VulkanUtils.h"
#include "VulkanBackend.h"

namespace BHive
{

	VulkanSwapChain::VulkanSwapChain()
		: mDevice(VulkanBackend::GetLogicalDevice())
	{
		
	}

	VulkanSwapChain::~VulkanSwapChain()
	{
		for (auto &img : mImages)
		{
			img.Destroy();
		}

		mDepthImage.Destroy();
	}

	void VulkanSwapChain::Init(vk::SurfaceKHR surface, const VulkanSwapChainCreateInfo &create_info)
	{
		mImages.clear();
		mPresentSemaphores.clear();
		mRenderFinishedSemaphores.clear();
		mInFlightFences.clear();

		mExtent = VulkanUtils::ChooseSwapExtent(create_info.Capabilities, create_info.Width, create_info.Height);
		mImageFormat = VulkanUtils::ChooseSwapSurfaceFormat(create_info.Formats);
		mMinImageCount = VulkanUtils::ChooseMinImageCount(create_info.Capabilities);

		auto present_mode = VulkanUtils::ChooseSwapPresentMode(create_info.PresentModes);
		vk::SwapchainCreateInfoKHR swap_chain_create_info(
			{}, surface, mMinImageCount, mImageFormat.format, mImageFormat.colorSpace, mExtent, 1, vk::ImageUsageFlagBits::eColorAttachment, vk::SharingMode::eExclusive, {},
			create_info.Capabilities.currentTransform, vk::CompositeAlphaFlagBitsKHR::eOpaque, present_mode, true, nullptr, nullptr);

		mSwapChain = mDevice.createSwapchainKHR(swap_chain_create_info);
		auto images = mSwapChain.getImages();

		mImages.resize(images.size());
		auto& mng = VulkanBackend::GetGPUResourceManager();

		for (size_t i = 0; i < images.size(); i++)
		{
			auto raw = images[i];
			auto &img = mImages[i];

			ImageCreateInfo info{};
			info.ImageCI.arrayLayers = 1;
			info.ImageCI.mipLevels = 1;
			info.ImageCI.usage = vk::ImageUsageFlagBits::eColorAttachment;
			info.DebugName = std::format("SwapChainImage_{}", i);

			auto range = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
			info.ViewCI = vk::ImageViewCreateInfo({}, raw, vk::ImageViewType::e2D, mImageFormat.format, {}, range); 
			img.Initialize(raw, info);
		}

		auto image_count = static_cast<uint32_t>(mImages.size());

		mRenderFinishedSemaphores.reserve(image_count);
		mPresentSemaphores.reserve(MAX_FRAMES_IN_FLIGHT);
		mInFlightFences.reserve(MAX_FRAMES_IN_FLIGHT);

		for (uint32_t i = 0; i < image_count; i++)
		{	
			mRenderFinishedSemaphores.emplace_back(mDevice, vk::SemaphoreCreateInfo());	

			if (i < MAX_FRAMES_IN_FLIGHT)
			{
				mPresentSemaphores.emplace_back(mDevice, vk::SemaphoreCreateInfo());

				mInFlightFences.emplace_back(mDevice, vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
			}
		}

		mDepthFormat = VulkanUtils::FindDepthFormat();

		ImageCreateInfo depth_info{};
		depth_info.ImageCI = vk::ImageCreateInfo(
			{}, vk::ImageType::e2D, mDepthFormat, vk::Extent3D{mExtent, 1}, 1, 1, vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment,
			vk::SharingMode::eExclusive, 0);
		auto range = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil, 0, 1, 0, 1);
		depth_info.ViewCI = vk::ImageViewCreateInfo({}, VK_NULL_HANDLE, vk::ImageViewType::e2D, mDepthFormat, {}, range);
		depth_info.DebugName = std::format("SwapChainImage_DepthStencil");

		mDepthImage.Initialize(depth_info);
	}

	void VulkanSwapChain::Recreate(vk::SurfaceKHR surface, uint32_t w, uint32_t h)
	{
		mDevice.waitIdle();

		for (auto &img : mImages)
		{
			img.Destroy();
		}

		mDepthImage.Destroy();

		auto &physical_device = VulkanBackend::GetPhysicalDevice();
		auto surfaceCapabilities = physical_device.getSurfaceCapabilitiesKHR(surface);
		auto formats = physical_device.getSurfaceFormatsKHR(surface);
		auto presentModes = physical_device.getSurfacePresentModesKHR(surface);

		VulkanSwapChainCreateInfo create_info{};
		create_info.Width = w;
		create_info.Height = h;
		create_info.Capabilities = surfaceCapabilities;
		create_info.Formats = formats;
		create_info.PresentModes = presentModes;

		Init(surface, create_info);
	}

	void VulkanSwapChain::WaitForFence(uint32_t frame)
	{
		vk::Fence fence = mInFlightFences[frame];
		if (fence)
		{
			while(vk::Result::eTimeout ==  mDevice.waitForFences(fence, VK_TRUE, UINT64_MAX));
			mDevice.resetFences(fence);
		}	
	}

	vk::ResultValue<uint32_t> VulkanSwapChain::AquireNextImage(uint32_t frame)
	{
		vk::Semaphore present = mPresentSemaphores[frame]; //per frame
		return  mSwapChain.acquireNextImage(UINT64_MAX, present, VK_NULL_HANDLE);
	}

	vk::Result VulkanSwapChain::Present(vk::CommandBuffer cmd, uint32_t imageIndex, uint32_t frame)
	{
		vk::Fence fence = mInFlightFences[frame];
		vk::Semaphore wait_semaphore = mPresentSemaphores[frame];
		vk::Semaphore signal_semaphore = mRenderFinishedSemaphores[imageIndex];

		vk::SemaphoreSubmitInfo wait_info(wait_semaphore, 0, vk::PipelineStageFlagBits2::eAllCommands);
		vk::CommandBufferSubmitInfo cmd_submit_info(cmd);
		vk::SemaphoreSubmitInfo signal_info(signal_semaphore, 0, vk::PipelineStageFlagBits2::eAllCommands);

		const vk::SubmitInfo2 submitInfo2({}, wait_info, cmd_submit_info, signal_info);

		auto &graphics_queue = VulkanBackend::GetQueueFamilies().GraphicsQueue;
		graphics_queue.submit2(submitInfo2, fence);

		const vk::PresentInfoKHR presentInfoKHR(signal_semaphore, *mSwapChain, imageIndex);
		return (vk::Result)vkQueuePresentKHR(*graphics_queue, &*presentInfoKHR);
	}

} // namespace BHive