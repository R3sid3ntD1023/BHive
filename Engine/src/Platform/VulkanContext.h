#pragma once

#include "VulkanCore.h"
#include "VulkanDevice.h"
#include "VulkanInstance.h"
#include "VulkanSurface.h"

struct GLFWwindow;

namespace BHive
{
	class VulkanContext
	{
	public:
		VulkanContext(GLFWwindow *window);
		~VulkanContext();

		void Init();

		static VulkanInstance &GetInstance();

	private:
		GLFWwindow *mWindowHandle = nullptr;
		Ref<VulkanInstance> mInstance;
		Ref<VulkanSurface> mSurface;
		Ref<VulkanDevice> mDevice;
	};
} // namespace BHive