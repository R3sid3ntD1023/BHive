#include "VulkanWindowContext.h"

#include "gfx/RenderCommand.h"
#include "VulkanRendererAPI.h"
#include "core/WindowInput.h"
#include "core/Window.h"
#include "VulkanBackend.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include <GLFW/glfw3.h>

namespace BHive
{
	VulkanWindowContext::VulkanWindowContext(Window *window)
	{
		ASSERT(window, "Window is null");
		mWindowHandle = static_cast<GLFWwindow *>(window->GetNative());
		ASSERT(mWindowHandle, "Window handle is null!");

		WindowInput::WindowEvent.Add(this, &VulkanWindowContext::OnEvent);
	}

	void VulkanWindowContext::Init()
	{
		TryCreateSwapChain(mWindowHandle);
	}

	void VulkanWindowContext::SwapBuffers()
	{
		auto api = RenderCommand::GetGraphicsAPI<VulkanRendererAPI>();

		if (mIsMinimized || !mSwapChain)
			return;

		if (mHasPendingResize)
		{
			auto now = std::chrono::steady_clock::now();
			auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - mLastResizeTime).count();
			constexpr int DebounceMs = 50;

			if (ms > DebounceMs)
			{
				if (mSwapChain->Recreate(mPendingWidth, mPendingHeight))
				{
					api->ResetFrameIndex();
					mHasPendingResize = false;
					return;
				}

				mHasPendingResize = true;
			}
		}

		auto result = api->RenderFrame(mSwapChain.get());
		if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR)
		{

			if (mSwapChain->Recreate(mPendingWidth, mPendingHeight))
			{
				api->ResetFrameIndex();
				mHasPendingResize = false;
				return;
			}
			mHasPendingResize = true;
		}
	}

	void VulkanWindowContext::TryCreateSwapChain(GLFWwindow *window)
	{
		int w = 0, h = 0;
		glfwGetFramebufferSize(window, &w, &h);

		while (w == 0 || h == 0)
		{
			glfwWaitEvents();
			glfwGetFramebufferSize(window, &w, &h);
		}

		VkSurfaceKHR surface;
		auto &instance = VulkanBackend::GetInstance();
		glfwCreateWindowSurface(*instance, mWindowHandle, nullptr, &surface);
		mSwapChain = CreateScope<VulkanSwapChain>(surface);
		mSwapChain->Init(uint32_t(w), uint32_t(h));
	}

	void VulkanWindowContext::OnEvent(Event &event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch(this, &VulkanWindowContext::OnWindowResized);
	}

	bool VulkanWindowContext::OnWindowResized(WindowResizeEvent &e)
	{
		mIsMinimized = (e.x == 0 || e.y == 0);
		if (mIsMinimized)
			return false;

		mHasPendingResize = true;
		mPendingWidth = e.x;
		mPendingHeight = e.y;
		mLastResizeTime = std::chrono::steady_clock::now();
		return false;
	}

} // namespace BHive