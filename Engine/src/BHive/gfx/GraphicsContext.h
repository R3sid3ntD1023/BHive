#pragma once

#include "core/Core.h"
#include "core/EventDelegate.h"
#include "VulkanCore.h"

struct GLFWwindow;

namespace BHive
{
	class VulkanSwapChain;
	class VulkanDevice;
	class VulkanPipeline;

	class BHIVE_API GraphicsContext
	{
	public:
		

		GraphicsContext(GLFWwindow *window);

		virtual ~GraphicsContext();

		virtual void Init();

		virtual void SwapBuffers();

		const Ref<VulkanSwapChain> &GetSwapChain() const { return mSwapChain; }

		void OnFramebufferResized(uint32_t w, uint32_t h);

		uint32_t GetImageIndex() const { return mImageIndex; }

		static GraphicsContext &Get()
		{
			ASSERT(sInstance);
			return *sInstance;
		}

	private:

		void CreateSwapChain();

		void RecreateSwapChain();

	private:
		GLFWwindow *mWindowHandle;

		vk::raii::SurfaceKHR mSurface = nullptr;

		vk::raii::Device mDevice = nullptr;

		VkQueueFamilies mQueueFamilies;

		Ref<VulkanSwapChain> mSwapChain;

		static inline GraphicsContext *sInstance = nullptr;

		uint32_t mImageIndex = 0;

		bool mFramebufferResized = false;
	};
} // namespace BHive