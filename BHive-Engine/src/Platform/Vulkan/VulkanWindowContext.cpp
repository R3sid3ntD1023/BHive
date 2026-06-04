#include "VulkanWindowContext.h"
#include <GLFW/glfw3.h>
#include "gfx/RenderCommand.h"
#include "VulkanRendererAPI.h"

namespace BHive
{
	VulkanWindowContext::VulkanWindowContext(void *windowHandle)
		:mWindowHandle(static_cast<GLFWwindow *>(windowHandle))
		 
	{
		ASSERT(mWindowHandle, "Window handle is null!");
	}

	VulkanWindowContext::~VulkanWindowContext()
	{
		auto api = RenderCommand::GetGraphicsAPI();
		if (api->GetCurrentContext() == this)
			api->SetCurrentContext(nullptr);
	}

	void VulkanWindowContext::OnFramebufferResized(uint32_t w, uint32_t h)
	{
		mFramebufferResized = true;
	}

	void VulkanWindowContext::RequestSwapChainRecreate()
	{
		int w = 0, h = 0;
		glfwGetFramebufferSize(mWindowHandle, &w, &h);

		while (w == 0 || h == 0)
		{
			glfwWaitEvents();
			glfwGetFramebufferSize(mWindowHandle, &w, &h);
		}

		VulkanBackend::Get().RequestSwapChainRecreate(w, h);
	}

	void VulkanWindowContext::Init()
	{
		VulkanBackend::Get().Init(mWindowHandle);
	}

	void VulkanWindowContext::SwapBuffers()
	{
		auto api = RenderCommand::GetGraphicsAPI<VulkanRendererAPI>();
		api->SetCurrentContext(this);

		auto result = VulkanBackend::Get().Present();

		if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || mFramebufferResized)
		{
			api->ResetFrameIndex();
			RequestSwapChainRecreate();
			
			mFramebufferResized = false;
			return;
		}
		else if (result != vk::Result::eSuccess)
		{
			ASSERT(false, "Failed to present swap chain image!")
		}
	}

} // namespace BHive