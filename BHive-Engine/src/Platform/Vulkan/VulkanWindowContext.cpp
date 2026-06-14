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


	void VulkanWindowContext::RequestSwapChainRecreate(int w, int h)
	{
		VulkanBackend::Get().RequestSwapChainRecreate(w, h);
	}


	void VulkanWindowContext::SwapBuffers()
	{
		auto api = RenderCommand::GetGraphicsAPI<VulkanRendererAPI>();
		api->SetCurrentContext(this);

		if (mIsMinimized)
			return;

		VulkanBackend::Get().Present();
	}

	void VulkanWindowContext::OnEvent(Event &event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch(this, &VulkanWindowContext::OnWindowResized);
	}

	bool VulkanWindowContext::OnWindowResized(WindowResizeEvent &e)
	{
		if (e.x == 0 && e.y == 0)
		{
			mIsMinimized = true;
			return false;
		}
		
		mIsMinimized = false;

		auto api = RenderCommand::GetGraphicsAPI<VulkanRendererAPI>();
		api->ResetFrameIndex();

		RequestSwapChainRecreate(e.x, e.y);
		
		return false;
	}

} // namespace BHive