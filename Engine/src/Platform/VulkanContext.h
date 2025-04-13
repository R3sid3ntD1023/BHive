#pragma once

#include "VulkanCore.h"

struct GLFWwindow;

namespace BHive
{
	class VulkanSwapChain;

	class VulkanContext
	{
	public:
		VulkanContext(GLFWwindow *window);
		~VulkanContext();

		void Init();

	private:
		GLFWwindow *mWindowHandle = nullptr;
		Ref<VulkanSwapChain> mSwapChain;
	};
} // namespace BHive