#pragma once

#include "VulkanAPI.h"

namespace BHive
{
	class VulkanSwapChain
	{
	public:
		VulkanSwapChain();
		~VulkanSwapChain();

		void Init();
		void Release();

	private:
		VkSurfaceFormatKHR ChooseSurfaceFormat(const SurfaceFormats &formats);
		VkPresentModeKHR ChoosePresentMode(const PresentModes &modes);
		VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR &capabilites);

	private:
		VkSwapchainKHR mSwapChain = VK_NULL_HANDLE;
		std::vector<VkImage> mImages;
		std::vector<VkImageView> mImageViews;
	};
} // namespace BHive
