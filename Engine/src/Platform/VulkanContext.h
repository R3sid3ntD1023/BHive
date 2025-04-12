#pragma once

#include "VulkanCore.h"
#include "VulkanDevice.h"
#include "VulkanInstance.h"

struct GLFWwindow;

namespace BHive
{
	class VulkanContext
	{
	public:
		VulkanContext(GLFWwindow *window);
		~VulkanContext();

		void Init();

		VulkanInstance &GetInstance();

	private:
		GLFWwindow *mWindowHandle = nullptr;
		Ref<VulkanInstance> mInstance;
		Ref<VulkanDevice> mDevice;
	};
} // namespace BHive