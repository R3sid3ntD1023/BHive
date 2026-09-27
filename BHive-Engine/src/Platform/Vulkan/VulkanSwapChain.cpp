#include "VulkanSwapChain.h"
#include "VulkanBackend.h"
#include "VulkanUtils.h"

namespace BHive
{
	VulkanSwapChain::VulkanSwapChain(VkSurfaceKHR surface)
		: mDevice(VulkanBackend::GetLogicalDevice())
	{
		auto &instance = VulkanBackend::GetInstance();
		mSurface = vk::raii::SurfaceKHR(instance, surface);
	}

	VulkanSwapChain::~VulkanSwapChain()
	{
		mDevice.waitIdle();
	}

	void VulkanSwapChain::Init(uint32_t w, uint32_t h)
	{
		auto &physical_device = VulkanBackend::GetPhysicalDevice();
		auto formats = physical_device.getSurfaceFormatsKHR(mSurface);
		auto presentModes = physical_device.getSurfacePresentModesKHR(mSurface);

		mCapabilities = physical_device.getSurfaceCapabilitiesKHR(mSurface);
		mImageFormat = VulkanUtils::ChooseSwapSurfaceFormat(formats);
		mPresentMode = VulkanUtils::ChooseSwapPresentMode(vk::PresentModeKHR::eImmediate, presentModes);

		mExtent = VulkanUtils::ChooseSwapExtent(mCapabilities, w, h);
		mMinImageCount = VulkanUtils::ChooseMinImageCount(mCapabilities);
		mDepthFormat = VulkanUtils::FindDepthFormat();

		CreateSwapChain();
		CreateSyncObjects();
		CreateImages();
		CreateDepthImage();
	}

	bool VulkanSwapChain::Recreate(uint32_t w, uint32_t h)
	{
		if (w <= 0 || h <= 0)
			return false;

		auto &device = VulkanBackend::GetLogicalDevice();
		device.waitIdle();

		mImages.clear();
		mDepthImage.Clear();
		mImagesInFlight.clear();
		mSwapChain.clear();

		Init(w, h);

		return true;
	}

	void VulkanSwapChain::BeginRendering(vk::CommandBuffer cmd, uint32_t imageIndex, vk::ClearColorValue colorValue, vk::ClearDepthStencilValue depthValue)
	{
		auto &image = mImages.at(imageIndex);
		auto &depth = mDepthImage;

		// Color: Undefined/ShaderRead/etc → ColorAttachment
		image.Transition(cmd, ImageState::ColorAttachment());

		// Depth: Undefined/ShaderRead/etc → DepthStencilAttachment
		depth.Transition(cmd, ImageState::DepthStencilAttachment());

		vk::RenderingAttachmentInfo attachmentInfo(
			image.GetView(0, 0),
			vk::ImageLayout::eColorAttachmentOptimal,
			{},
			{},
			vk::ImageLayout::eUndefined,
			vk::AttachmentLoadOp::eClear,
			vk::AttachmentStoreOp::eStore,
			colorValue
		);

		vk::RenderingAttachmentInfo depth_attachment_info(
			depth.GetView(0, 0),
			vk::ImageLayout::eDepthStencilAttachmentOptimal,
			{},
			{},
			vk::ImageLayout::eUndefined,
			vk::AttachmentLoadOp::eClear,
			vk::AttachmentStoreOp::eDontCare,
			depthValue
		);

		vk::RenderingInfo renderingInfo({}, vk::Rect2D({0, 0}, mExtent), 1, 0, attachmentInfo, &depth_attachment_info);
		cmd.beginRendering(renderingInfo);

		vk::Viewport viewport(0.f, (float)mExtent.height, (float)mExtent.width, -(float)mExtent.height, 0.0f, 1.0f);
		vk::Rect2D scissor({0, 0}, mExtent);

		cmd.setViewportWithCount(viewport);
		cmd.setScissorWithCount(scissor);
	}

	void VulkanSwapChain::EndRendering(vk::CommandBuffer cmd, uint32_t imageIndex)
	{
		cmd.endRendering();

		auto &image = mImages.at(imageIndex);
		image.Transition(cmd, ImageState::Present());
	}

	void VulkanSwapChain::CreateSwapChain()
	{
		vk::SwapchainCreateInfoKHR swap_chain_create_info(
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

		mSwapChain = mDevice.createSwapchainKHR(swap_chain_create_info);
	}

	void VulkanSwapChain::CreateSyncObjects()
	{
		uint32_t imageCount = mSwapChain.getImages().size();

		mPresentSemaphores.clear();
		mRenderFinishedSemaphores.clear();
		mInFlightFences.clear();

		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			mPresentSemaphores.emplace_back(mDevice, vk::SemaphoreCreateInfo());
			mInFlightFences.emplace_back(mDevice, vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
		}

		for (uint32_t i = 0; i < imageCount; i++)
		{
			mRenderFinishedSemaphores.emplace_back(mDevice, vk::SemaphoreCreateInfo());
		}

		mImagesInFlight.resize(imageCount, VK_NULL_HANDLE);
	}

	void VulkanSwapChain::CreateImages()
	{
		auto swapChainImages = mSwapChain.getImages();
		mImages.resize(swapChainImages.size());

		for (size_t i = 0; i < swapChainImages.size(); i++)
		{
			auto swapChainImage = swapChainImages[i];
			auto &img = mImages[i];

			auto range = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
			vk::ImageViewCreateInfo viewInfo({}, swapChainImage, vk::ImageViewType::e2D, mImageFormat.format, {}, range);
			img.Initialize(swapChainImage, 1, 1, vk::ImageUsageFlagBits::eColorAttachment, viewInfo, {});
		}
	}

	void VulkanSwapChain::CreateDepthImage()
	{
		if (mDepthImage)
			mDepthImage = {};

		vk::ImageCreateInfo imgInfo(
			{},
			vk::ImageType::e2D,
			mDepthFormat,
			vk::Extent3D{mExtent, 1},
			1,
			1,
			vk::SampleCountFlagBits::e1,
			vk::ImageTiling::eOptimal,
			vk::ImageUsageFlagBits::eDepthStencilAttachment,
			vk::SharingMode::eExclusive,
			0
		);
		auto range = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil, 0, 1, 0, 1);
		vk::ImageViewCreateInfo viewInfo({}, VK_NULL_HANDLE, vk::ImageViewType::e2D, mDepthFormat, {}, range);
		mDepthImage.Initialize(imgInfo, viewInfo, {});
		mDepthImage.SetDebugName(std::format("SwapChainImage_DepthStencil"));
	}

	vk::Semaphore VulkanSwapChain::GetRenderFinishedSemaphore(uint32_t frame)
	{
		return mRenderFinishedSemaphores.at(frame);
	}

	vk::Semaphore VulkanSwapChain::GetImageAvailableSemaphore(uint32_t frame)
	{
		return mPresentSemaphores.at(frame);
	}

	vk::Fence VulkanSwapChain::GetInFlightFence(uint32_t frame)
	{
		return mInFlightFences.at(frame);
	}

	void VulkanSwapChain::WaitForFence(uint32_t frame)
	{
		vk::Fence fence = GetInFlightFence(frame);
		mDevice.waitForFences(fence, VK_TRUE, UINT64_MAX);
	}

	void VulkanSwapChain::ResetFence(uint32_t frame)
	{
		vk::Fence fence = GetInFlightFence(frame);
		mDevice.resetFences(fence);
	}

	vk::ResultValue<uint32_t> VulkanSwapChain::AquireNextImage(uint32_t frame)
	{
		vk::Semaphore imageAvialable = GetImageAvailableSemaphore(frame);
		auto [result, imageIndex] = mSwapChain.acquireNextImage(UINT64_MAX, imageAvialable, VK_NULL_HANDLE);

		ASSERT(imageIndex < mImagesInFlight.size());

		if (mImagesInFlight[imageIndex] != VK_NULL_HANDLE)
		{
			mDevice.waitForFences(mImagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
		}

		mImagesInFlight[imageIndex] = GetInFlightFence(frame);
		return {result, imageIndex};
	}

	vk::Result VulkanSwapChain::Present(vk::CommandBuffer cmd, uint32_t imageIndex, uint32_t frame)
	{
		vk::Fence fence = GetInFlightFence(frame);
		vk::Semaphore waitSemaphore = GetImageAvailableSemaphore(frame);
		vk::Semaphore signalSemaphore = GetRenderFinishedSemaphore(frame);

		vk::SemaphoreSubmitInfo wait_info(waitSemaphore, 0, vk::PipelineStageFlagBits2::eAllCommands);
		vk::CommandBufferSubmitInfo cmd_submit_info(cmd);
		vk::SemaphoreSubmitInfo signal_info(signalSemaphore, 0, vk::PipelineStageFlagBits2::eAllCommands);

		const vk::SubmitInfo2 submitInfo2({}, wait_info, cmd_submit_info, signal_info);

		auto &graphics_queue = VulkanBackend::GetQueueFamilies().GraphicsQueue;
		graphics_queue.submit2(submitInfo2, fence);

		const vk::PresentInfoKHR presentInfoKHR(signalSemaphore, *mSwapChain, imageIndex);
		return (vk::Result)vkQueuePresentKHR(*graphics_queue, &*presentInfoKHR);
	}

} // namespace BHive