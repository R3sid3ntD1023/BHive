#include "VulkanSwapChain.h"
#include "VulkanUtils.h"
#include "VulkanBackend.h"

namespace BHive
{
	VulkanSwapChain::VulkanSwapChain(VkSurfaceKHR surface)
	{
		auto &instance = VulkanBackend::GetInstance();
		mSurface = vk::raii::SurfaceKHR(instance, surface);
	}

	VulkanSwapChain::~VulkanSwapChain()
	{
		auto &device = VulkanBackend::GetLogicalDevice();
		device.waitIdle();

		// mInFlightFences.clear();
		// mRenderFinishedSemaphores.clear();
		// mPresentSemaphores.clear();
		// mImages.clear();
		// mDepthImage = {};
		// mSwapChain.clear();
		// mSurface.clear();
	}

	void VulkanSwapChain::Init(uint32_t w, uint32_t h)
	{
		auto &device = VulkanBackend::GetLogicalDevice();
		auto &physical_device = VulkanBackend::GetPhysicalDevice();
		auto formats = physical_device.getSurfaceFormatsKHR(mSurface);
		auto presentModes = physical_device.getSurfacePresentModesKHR(mSurface);

		mCapabilities = physical_device.getSurfaceCapabilitiesKHR(mSurface);
		mImageFormat = VulkanUtils::ChooseSwapSurfaceFormat(formats);
		mPresentMode = VulkanUtils::ChooseSwapPresentMode(vk::PresentModeKHR::eImmediate, presentModes);

		mExtent = VulkanUtils::ChooseSwapExtent(mCapabilities, w, h);
		mMinImageCount = VulkanUtils::ChooseMinImageCount(mCapabilities);
		mDepthFormat = VulkanUtils::FindDepthFormat();

		CreateSwapChain(device);
		CreateSyncObjects(device);
		CreateImages(device);
		CreateDepthImage(device);
	}

	bool VulkanSwapChain::Recreate(uint32_t w, uint32_t h)
	{
		if (w <= 0 || h <= 0)
			return false;

		auto &device = VulkanBackend::GetLogicalDevice();
		device.waitIdle();

		LOG_TRACE("recreating swap chain... with size[{}x{}]", w, h);

		mImages.clear();
		mDepthImage = {};

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
			image.Native().GetView(0, 0, 0), vk::ImageLayout::eColorAttachmentOptimal, {}, {}, vk::ImageLayout::eUndefined, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, colorValue);

		vk::RenderingAttachmentInfo depth_attachment_info(
			depth.Native().GetView(0, 0, 0), vk::ImageLayout::eDepthStencilAttachmentOptimal, {}, {}, vk::ImageLayout::eUndefined, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eDontCare,
			depthValue);

		vk::RenderingInfo renderingInfo({}, vk::Rect2D({0, 0}, mExtent), 1, 0, attachmentInfo, &depth_attachment_info);
		cmd.beginRendering(renderingInfo);

		vk::Viewport viewport(0.f, (float)mExtent.height, (float)mExtent.width, -(float)mExtent.height, 0.0f, 1.0f);
		vk::Rect2D scissor({0, 0}, mExtent);

		cmd.setViewportWithCount(viewport);
		cmd.setScissorWithCount(scissor);
	}

	void VulkanSwapChain::EndRendering(vk::CommandBuffer cmd, uint32_t imageIndex)
	{
		auto &image = mImages.at(imageIndex);
		image.Transition(cmd, ImageState::Present());
	}

	void VulkanSwapChain::CreateSwapChain(vk::raii::Device &device)
	{
		vk::SwapchainCreateInfoKHR swap_chain_create_info(
			{}, mSurface, mMinImageCount, mImageFormat.format, mImageFormat.colorSpace, mExtent, 1, vk::ImageUsageFlagBits::eColorAttachment, vk::SharingMode::eExclusive, {},
			mCapabilities.currentTransform, vk::CompositeAlphaFlagBitsKHR::eOpaque, mPresentMode, true, nullptr, nullptr);

		mSwapChain = device.createSwapchainKHR(swap_chain_create_info);
	}

	void VulkanSwapChain::CreateSyncObjects(vk::raii::Device &device)
	{
		uint32_t imageCount = mSwapChain.getImages().size();

		mPresentSemaphores.clear();
		mRenderFinishedSemaphores.clear();
		mInFlightFences.clear();

		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			mPresentSemaphores.emplace_back(device, vk::SemaphoreCreateInfo());
			mInFlightFences.emplace_back(device, vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
		}

		for (uint32_t i = 0; i < imageCount; i++)
		{
			mRenderFinishedSemaphores.emplace_back(device, vk::SemaphoreCreateInfo());
		}
	}

	void VulkanSwapChain::CreateImages(vk::raii::Device &device)
	{
		auto swapChainImages = mSwapChain.getImages();
		mImages.resize(swapChainImages.size());

		for (size_t i = 0; i < swapChainImages.size(); i++)
		{
			auto swapChainImage = swapChainImages[i];
			auto &img = mImages[i];

			ImageCreateInfo info{};
			info.ImageCI.arrayLayers = 1;
			info.ImageCI.mipLevels = 1;
			info.ImageCI.usage = vk::ImageUsageFlagBits::eColorAttachment;
			info.DebugName = std::format("SwapChainImage_{}", i);

			auto range = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
			info.ViewCI = vk::ImageViewCreateInfo({}, swapChainImage, vk::ImageViewType::e2D, mImageFormat.format, {}, range);
			img.Initialize(swapChainImage, info);
		}
	}

	void VulkanSwapChain::CreateDepthImage(vk::raii::Device &device)
	{
		if (mDepthImage)
			mDepthImage = {};

		ImageCreateInfo depth_info{};
		depth_info.ImageCI = vk::ImageCreateInfo(
			{}, vk::ImageType::e2D, mDepthFormat, vk::Extent3D{mExtent, 1}, 1, 1, vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment,
			vk::SharingMode::eExclusive, 0);
		auto range = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil, 0, 1, 0, 1);
		depth_info.ViewCI = vk::ImageViewCreateInfo({}, VK_NULL_HANDLE, vk::ImageViewType::e2D, mDepthFormat, {}, range);
		depth_info.DebugName = std::format("SwapChainImage_DepthStencil");

		mDepthImage.Initialize(depth_info);
	}

	vk::Semaphore VulkanSwapChain::GetRenderFinishedSemaphore(uint32_t imageIndex)
	{
		return mRenderFinishedSemaphores.at(imageIndex);
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

		if (fence)
		{
			auto &device = VulkanBackend::GetLogicalDevice();
			device.waitForFences(fence, VK_TRUE, UINT64_MAX);
			device.resetFences(fence);
		}
	}

	vk::ResultValue<uint32_t> VulkanSwapChain::AquireNextImage(uint32_t frame)
	{
		vk::Semaphore imageAvialable = GetImageAvailableSemaphore(frame);
		return mSwapChain.acquireNextImage(UINT64_MAX, imageAvialable, VK_NULL_HANDLE);
	}

	vk::Result VulkanSwapChain::Present(vk::CommandBuffer cmd, uint32_t imageIndex, uint32_t frame)
	{
		vk::Fence fence = GetInFlightFence(frame);
		vk::Semaphore waitSemaphore = GetImageAvailableSemaphore(frame);
		vk::Semaphore signalSemaphore = GetRenderFinishedSemaphore(imageIndex);

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