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

		const Ref<VulkanSwapChain> &GetSwapChain() const { return mSwapChain; }

		void OnFramebufferResized(uint32_t w, uint32_t h) override;

		vk::raii::CommandPool &GetCommandPool() { return mCommandPool; }

		vk::raii::CommandBuffer& GetCommandBuffer() { return mCommandBuffers[mCurrentFrame]; }

		uint32_t GetCurrentFrame() const { return mCurrentFrame; }

	private:
		void CreateSwapChain();

		void RecreateSwapChain();

		void CreateCommandBuffers();

	private:

		vk::raii::Device &mDevice;

		GLFWwindow *mWindowHandle = nullptr;

		vk::raii::SurfaceKHR mSurface = nullptr;
	
		vk::raii::CommandPool mCommandPool = nullptr;

		vk::raii::CommandBuffers mCommandBuffers = nullptr;

		Ref<VulkanSwapChain> mSwapChain;

		bool mFramebufferResized = false;

		uint32_t mCurrentFrame = 0;
	};
} // namespace BHive