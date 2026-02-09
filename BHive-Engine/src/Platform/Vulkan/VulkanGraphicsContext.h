#pragma once

#include "gfx/GraphicsContext.h"
#include "VulkanCore.h"

struct GLFWwindow;

namespace BHive
{
	class VulkanSwapChain;
	class VulkanDevice;
	class VulkanPipeline;

	class BHIVE_API VulkanGraphicsContext : public GraphicsContext
	{
	public:
		VulkanGraphicsContext(void *windowHandle);

		~VulkanGraphicsContext() ;

		virtual void Init();

		virtual void SwapBuffers();

		const Ref<VulkanSwapChain> &GetSwapChain() const { return mSwapChain; }

		void OnFramebufferResized(uint32_t w, uint32_t h) override;

	private:
		void CreateSwapChain();

		void RecreateSwapChain();

	private:
		GLFWwindow *mWindowHandle;

		vk::raii::SurfaceKHR mSurface = nullptr;

		vk::raii::Device mDevice = nullptr;

		Ref<VulkanSwapChain> mSwapChain;

		bool mFramebufferResized = false;
	};
} // namespace BHive