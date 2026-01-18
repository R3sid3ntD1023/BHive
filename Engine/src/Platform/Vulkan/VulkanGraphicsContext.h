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

		~VulkanGraphicsContext() override;

		virtual void Init();

		virtual void SwapBuffers();

		const Ref<VulkanSwapChain> &GetSwapChain() const { return mSwapChain; }

		void OnFramebufferResized(uint32_t w, uint32_t h) override;

		uint32_t GetImageIndex() const { return mImageIndex; }

	private:
		void CreateSwapChain();

		void RecreateSwapChain();

	private:
		GLFWwindow *mWindowHandle;

		vk::raii::SurfaceKHR mSurface = nullptr;

		vk::raii::Device mDevice = nullptr;

		VkQueueFamilies mQueueFamilies;

		Ref<VulkanSwapChain> mSwapChain;

		uint32_t mImageIndex = 0;

		bool mFramebufferResized = false;
	};
} // namespace BHive