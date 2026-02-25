#include "VulkanSwapChain.h"
#include "GPUResourceManager.h"

namespace BHive
{

	VulkanSwapChain::VulkanSwapChain()
		: mDevice(VulkanBackend::GetLogicalDevice())
	{
		
	}

	VulkanSwapChain::~VulkanSwapChain()
	{
		
	}

	void VulkanSwapChain::Init(vk::raii::SurfaceKHR &surface, const VulkanSwapChainCreateInfo &create_info)
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
			{}, *surface, mMinImageCount, mImageFormat.format, mImageFormat.colorSpace, mExtent, 1, vk::ImageUsageFlagBits::eColorAttachment, vk::SharingMode::eExclusive, {},
			create_info.Capabilities.currentTransform, vk::CompositeAlphaFlagBitsKHR::eOpaque, present_mode, true, nullptr, nullptr);

		mSwapChain = mDevice.createSwapchainKHR(swap_chain_create_info);
		auto images = mSwapChain.getImages();

		for (auto& image : images)
		{
			Vulkan::Image img;
			img.SetImage(image);

			ImageViewDesc desc{};
			desc.Type = vk::ImageViewType::e2D;
			desc.Aspect = vk::ImageAspectFlagBits::eColor;
			desc.Format = mImageFormat.format;
			GPUResourceManager::Get().CreateImageView(img, desc);

			mImages.emplace_back(img);
		}

		auto image_count = static_cast<uint32_t>(mImages.size());
		auto frame_count = VulkanBackend::MAX_FRAMES_IN_FLIGHT;

		
		mRenderFinishedSemaphores.reserve(image_count);
		mPresentSemaphores.reserve(frame_count);
		mInFlightFences.reserve(frame_count);

		for (uint32_t i = 0; i < image_count; i++)
		{	
			mRenderFinishedSemaphores.emplace_back(mDevice, vk::SemaphoreCreateInfo());	

			if (i < frame_count)
			{
				mPresentSemaphores.emplace_back(mDevice, vk::SemaphoreCreateInfo());

				mInFlightFences.emplace_back(mDevice, vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
			}
		}

		mDepthFormat = VulkanUtils::FindDepthFormat();

		ImageDesc desc{};
		desc.Width = mExtent.width;
		desc.Height = mExtent.height;
		desc.Format = mDepthFormat;
		desc.Tiling = vk::ImageTiling::eOptimal;
		desc.Type = vk::ImageType::e2D;
		desc.Usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;
		desc.MemoryFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;
		mDepthImage = GPUResourceManager().Get().CreateImage(desc);

		ImageViewDesc view_desc{};
		view_desc.Type = vk::ImageViewType::e2D;
		view_desc.Format = mDepthFormat;
		view_desc.Aspect = vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
		GPUResourceManager().Get().CreateImageView(mDepthImage, view_desc);
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

	vk::Result VulkanSwapChain::Present(const vk::raii::CommandBuffer &buffers, uint32_t imageIndex, uint32_t frame)
	{
		
		vk::Fence fence = mInFlightFences[frame];
		vk::Semaphore wait_semaphore = mPresentSemaphores[frame];
		vk::Semaphore signal_semaphore = mRenderFinishedSemaphores[imageIndex];

		vk::SemaphoreSubmitInfo wait_info(wait_semaphore, 0, vk::PipelineStageFlagBits2::eAllCommands);
		vk::CommandBufferSubmitInfo cmd_submit_info(buffers);
		vk::SemaphoreSubmitInfo signal_info(signal_semaphore, 0, vk::PipelineStageFlagBits2::eAllCommands);

		const vk::SubmitInfo2 submitInfo2({}, wait_info, cmd_submit_info, signal_info);

		auto &graphics_queue = VulkanBackend::GetQueueFamilies().GraphicsQueue;
		graphics_queue.submit2(submitInfo2, fence);

		const vk::PresentInfoKHR presentInfoKHR(signal_semaphore, *mSwapChain, imageIndex);
		return (vk::Result)vkQueuePresentKHR(*graphics_queue, &*presentInfoKHR);
	}

} // namespace BHive