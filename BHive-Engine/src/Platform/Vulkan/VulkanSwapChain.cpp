#include "VulkanSwapChain.h"
#include "VulkanUtils.h"
#include "VulkanBackend.h"

namespace BHive
{
	VulkanSwapChain::VulkanSwapChain(vk::raii::Device &device, vk::SurfaceKHR surface, uint32_t w, uint32_t h)
		: mDevice(device),
		  mSurface(surface)
	{

		auto &physical_device = VulkanBackend::GetPhysicalDevice();
		auto surfaceCapabilities = physical_device.getSurfaceCapabilitiesKHR(surface);
		auto formats = physical_device.getSurfaceFormatsKHR(surface);
		auto presentModes = physical_device.getSurfacePresentModesKHR(surface);

		mExtent = VulkanUtils::ChooseSwapExtent(surfaceCapabilities, w, h);
		mImageFormat = VulkanUtils::ChooseSwapSurfaceFormat(formats);
		mPresentMode= VulkanUtils::ChooseSwapPresentMode(presentModes);
		mMinImageCount = VulkanUtils::ChooseMinImageCount(mCapabilities);

		Init(device, mExtent.width, mExtent.height);
	}

	VulkanSwapChain::~VulkanSwapChain()
	{
		for (auto &img : mImages)
		{
			img.Destroy();
		}

		mDepthImage.Destroy();
	}

	void VulkanSwapChain::Init(vk::raii::Device &device, uint32_t w, uint32_t h)
	{
		mDevice = device;
		mExtent = vk::Extent2D(w, h);
	
		vk::SwapchainCreateInfoKHR swap_chain_create_info
		(
			{}, 
			mSurface, 
			mMinImageCount, 
			mImageFormat.format, 
			mImageFormat.colorSpace, 
			mExtent, 
			1, 
			vk::ImageUsageFlagBits::eColorAttachment,
			vk::SharingMode::eExclusive, 
			{},
			mCapabilities.currentTransform, 
			vk::CompositeAlphaFlagBitsKHR::eOpaque, 
			mPresentMode, 
			true, 
			nullptr,
			nullptr
		);

		mSwapChain = device.createSwapchainKHR(swap_chain_create_info);

		
		auto images = mSwapChain.getImages();
		mImages.resize(images.size());

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
			mRenderFinishedSemaphores.emplace_back(device, vk::SemaphoreCreateInfo());	

			if (i < MAX_FRAMES_IN_FLIGHT)
			{
				mPresentSemaphores.emplace_back(device, vk::SemaphoreCreateInfo());

				mInFlightFences.emplace_back(device, vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
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

	void VulkanSwapChain::Recreate(vk::raii::Device &device, uint32_t w, uint32_t h)
	{
		for (auto &img : mImages)
		{
			img.Destroy();
		}

		mDepthImage.Destroy();

		mSwapChain = nullptr;

		Init(device, w, h);
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