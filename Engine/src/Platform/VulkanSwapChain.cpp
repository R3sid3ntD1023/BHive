#include "gfx/RenderCommand.h"
#include "VulkanSwapChain.h"
#include <GLFW/glfw3.h>

namespace BHive
{
	VulkanSwapChain::VulkanSwapChain()
	{
	}
	VulkanSwapChain::~VulkanSwapChain()
	{
	}

	void VulkanSwapChain::Init()
	{
		auto &api = VulkanAPI::GetAPI();

		auto details = api.GetSwapChainSupportDetails();
		auto surface_format = ChooseSurfaceFormat(details.Formats);
		auto present_mode = ChoosePresentMode(details.PresentModes);
		auto extent = ChooseExtent(details.Capabilities);

		uint32_t image_count = image_count = details.Capabilities.minImageCount + 1;
		if (details.Capabilities.maxImageCount > 0 && image_count > details.Capabilities.maxImageCount)
			image_count = details.Capabilities.maxImageCount;

		VkSwapchainCreateInfoKHR createinfo{};
		createinfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createinfo.surface = api.GetSurface();
		createinfo.minImageCount = image_count;
		createinfo.imageFormat = surface_format.format;
		createinfo.imageColorSpace = surface_format.colorSpace;
		createinfo.imageExtent = extent;
		createinfo.imageArrayLayers = 1;
		createinfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		const auto &indices = api.GetQueueFamilyIndices();

		if (indices.Graphics != indices.Present)
		{
			createinfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createinfo.queueFamilyIndexCount = 2;
			createinfo.pQueueFamilyIndices = indices.GetIndices().data();
		}
		else
		{
			createinfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createinfo.queueFamilyIndexCount = 0;	  // optional
			createinfo.pQueueFamilyIndices = nullptr; // optional
		}

		createinfo.preTransform = details.Capabilities.currentTransform;
		createinfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createinfo.presentMode = present_mode;
		createinfo.clipped = VK_TRUE;
		createinfo.oldSwapchain = VK_NULL_HANDLE;

		auto device = api.GetDevice();
		VK_ASSERT(vkCreateSwapchainKHR(device, &createinfo, nullptr, &mSwapChain), "Failed to create SwapChain!");

		// Get images
		vkGetSwapchainImagesKHR(device, mSwapChain, &image_count, nullptr);
		mImages.resize(image_count);
		vkGetSwapchainImagesKHR(device, mSwapChain, &image_count, mImages.data());

		// create image views
		mImageViews.resize(mImages.size());
		for (auto &image : mImages)
		{
			VkImageViewCreateInfo create_info{};
			create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			create_info.image = image;
			create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			create_info.format = api.GetSurfaceFormat();
			create_info.components = {
				VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY};
			create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			create_info.subresourceRange.baseMipLevel = 0;
			create_info.subresourceRange.levelCount = 1;
			create_info.subresourceRange.baseArrayLayer = 0;
			create_info.subresourceRange.layerCount = 1;
		}
	}

	void VulkanSwapChain::Release()
	{
		auto &api = VulkanAPI::GetAPI();
		auto device = api.GetDevice();

		for (auto &image_view : mImageViews)
		{
			vkDestroyImageView(device, image_view, nullptr);
		}

		vkDestroySwapchainKHR(device, mSwapChain, nullptr);
	}

	VkSurfaceFormatKHR VulkanSwapChain::ChooseSurfaceFormat(const SurfaceFormats &formats)
	{
		for (const auto &format : formats)
		{
			if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
				return format;
		}

		return formats[0];
	}

	VkPresentModeKHR VulkanSwapChain::ChoosePresentMode(const PresentModes &modes)
	{
		for (auto &mode : modes)
		{
			if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
				return mode;
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D VulkanSwapChain::ChooseExtent(const VkSurfaceCapabilitiesKHR &capabilites)
	{
		if (capabilites.currentExtent.width != std::numeric_limits<uint32_t>::max())
			return capabilites.currentExtent;

		int width = 0, height = 0;
		glfwGetFramebufferSize(glfwGetCurrentContext(), &width, &height);

		VkExtent2D extents{(unsigned)width, (unsigned)height};

		extents.width = std::clamp(extents.width, capabilites.minImageExtent.width, capabilites.maxImageExtent.width);
		extents.height = std::clamp(extents.height, capabilites.minImageExtent.height, capabilites.maxImageExtent.height);

		return extents;
	}

} // namespace BHive