#pragma once

#include "gfx/WindowContext.h"
#include "core/events/Event.h"
#include "core/events/ApplicationEvents.h"

struct GLFWwindow;

namespace BHive
{
	class VulkanSwapChain;
	class VulkanDevice;
	class VulkanPipeline;

	class BHIVE_API VulkanWindowContext : public WindowContext
	{
	public:
		VulkanWindowContext(Window *window);

		~VulkanWindowContext();

		virtual void Init();

		virtual void SwapBuffers();

		VulkanSwapChain &GetSwapchain() { return *mSwapChain; }

	private:
		void OnEvent(Event &event);

		bool OnWindowResized(WindowResizeEvent &e);

		void CreateSwapChain(GLFWwindow *window);

	private:
		GLFWwindow *mWindowHandle = nullptr;

		bool mIsMinimized = false;
		bool mHasPendingResize = false;
		uint32_t mPendingWidth = 0;
		uint32_t mPendingHeight = 0;
		std::chrono::steady_clock::time_point mLastResizeTime;

		Scope<VulkanSwapChain> mSwapChain;
	};
} // namespace BHive