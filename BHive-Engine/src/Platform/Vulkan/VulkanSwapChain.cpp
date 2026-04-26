#include "VulkanSwapChain.h"
#include "VulkanUtils.h"
#include "VulkanBackend.h"
#include "gfx/RenderCommand.h"
#include "VulkanRendererAPI.h"
#include "GPUComponents.h"

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
			VulkanBackend::GetGPUResourceManager().DestroyImage(img);
		}

		VulkanBackend::GetGPUResourceManager().DestroyImage(mDepthImage);
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

		mImages.resize(images.size());
		auto& mng = VulkanBackend::GetGPUResourceManager();

		for (size_t i = 0; i < images.size(); i++)
		{
			auto raw = images[i];
			UUID id = mng.RegisterExternalImage(raw);
			auto& img = mImages.emplace_back();
			img.ImageHandle = id;
			img.ArrayLayers = 1;
			img.MipLevels = 1;
			img.Aspect = vk::ImageAspectFlagBits::eColor;
			img.Usage = vk::ImageUsageFlagBits::eColorAttachment;
			img.DebugName = std::format("SwapChain Image{}", i);

			auto def = img.AddComponent<DefaultViewComponent>();
			auto state = img.AddComponent<StateTrackingComponent>();

			ImageViewDesc view_desc{};
			view_desc.Type = vk::ImageViewType::e2D;
			view_desc.Format = mImageFormat.format;
			view_desc.Aspect = vk::ImageAspectFlagBits::eColor;
			view_desc.BaseArrayLayer = 0;
			view_desc.BaseMipLevel = 0;
			view_desc.LayerCount = 1;
			view_desc.LevelCount = 1;

			def->View = mng.CreateImageView(raw, view_desc);
			state->Init(1, 1, ImageState::Present());
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

		ImageDesc desc{};
		desc.Width = mExtent.width;
		desc.Height = mExtent.height;
		desc.Format = mDepthFormat;
		desc.Tiling = vk::ImageTiling::eOptimal;
		desc.Type = vk::ImageType::e2D;
		desc.Usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;
		desc.MemoryFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;
		desc.Aspect = vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;

		ImageViewDesc view_desc{};
		view_desc.Type = vk::ImageViewType::e2D;
		view_desc.Format = mDepthFormat;
		view_desc.Aspect = vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;

		mDepthImage = mng.CreateImage(desc);
		mDepthImage.AddComponent<DefaultViewComponent>()->View = mng.CreateImageView(mDepthImage.GetImage(), view_desc);
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