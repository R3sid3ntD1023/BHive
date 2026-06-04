#pragma once

#include "gfx/WindowContext.h"
#include "VulkanBackend.h"

struct GLFWwindow;

namespace BHive
{
	class VulkanSwapChain;
	class VulkanDevice;
	class VulkanPipeline;

	class BHIVE_API VulkanWindowContext : public WindowContext
	{
	public:
		VulkanWindowContext(void *windowHandle);

		~VulkanWindowContext() ;

		virtual void Init();

		virtual void SwapBuffers();

		void OnFramebufferResized(uint32_t w, uint32_t h) override;

	private:
		void RequestSwapChainRecreate();

	private:
		GLFWwindow *mWindowHandle = nullptr;

		bool mFramebufferResized = false;
		
	};
} // namespace BHive