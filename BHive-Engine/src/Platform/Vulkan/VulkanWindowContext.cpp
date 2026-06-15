#include "VulkanWindowContext.h"
#include <GLFW/glfw3.h>
#include "gfx/RenderCommand.h"
#include "VulkanRendererAPI.h"
#include "core/WindowInput.h"
#include "core/Window.h"
#include "VulkanBackend.h"

namespace BHive
{
	VulkanWindowContext::VulkanWindowContext(Window *window)
	{
		ASSERT(window, "Window is null");
		mWindowHandle = static_cast<GLFWwindow *>(window->GetNative());
		ASSERT(mWindowHandle, "Window handle is null!");

		window->GetWindowInput().WindowEvent.Add(this, &VulkanWindowContext::OnEvent);
	}

	VulkanWindowContext::~VulkanWindowContext()
	{
		auto api = RenderCommand::GetGraphicsAPI();
		if (api->GetCurrentContext() == this)
			api->SetCurrentContext(nullptr);
	}

	void VulkanWindowContext::Init()
	{
		VulkanBackend::Get().Init(mWindowHandle);
	}


	void VulkanWindowContext::SwapBuffers()
	{
		auto api = RenderCommand::GetGraphicsAPI<VulkanRendererAPI>();
		api->SetCurrentContext(this);

		if (mIsMinimized)
			return;

		if (mHasPendingResize)
		{
			auto now = std::chrono::steady_clock::now();
			auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - mLastResizeTime).count();
			constexpr int DebounceMs = 50;

			if (ms > DebounceMs)
			{
				api->ResetFrameIndex();
				VulkanBackend::Get().RequestSwapChainRecreate(mPendingWidth, mPendingHeight);
				mHasPendingResize = false;
			}
		}

		auto result = VulkanBackend::Get().Present();
		if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR)
		{
			api->ResetFrameIndex();
			VulkanBackend::Get().RequestSwapChainRecreate(mPendingWidth, mPendingHeight);
			mHasPendingResize = false;
			return;
		}
	}

	void VulkanWindowContext::OnEvent(Event &event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch(this, &VulkanWindowContext::OnWindowResized);
	}

	bool VulkanWindowContext::OnWindowResized(WindowResizeEvent &e)
	{
		mIsMinimized = (e.x == 0 && e.y == 0);
		if (mIsMinimized)
			return false;

		mHasPendingResize = true;
		mPendingWidth = e.x;
		mPendingHeight = e.y;
		mLastResizeTime = std::chrono::steady_clock::now();
		
		return false;
	}

} // namespace BHive