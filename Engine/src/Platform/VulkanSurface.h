#pragma once

#include "VulkanCore.h"
#include "VulkanInstance.h"

struct GLFWwindow;

namespace BHive
{
	class VulkanSurface
	{
	public:
		VulkanSurface(const Ref<VulkanInstance> &instance, GLFWwindow *window);
		~VulkanSurface();

		operator VkSurfaceKHR() const { return mSurface; }

	private:
		VkSurfaceKHR mSurface = VK_NULL_HANDLE;
		Ref<VulkanInstance> mInstance;
	};
} // namespace BHive