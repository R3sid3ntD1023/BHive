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
		VulkanWindowContext(Window* window);

		~VulkanWindowContext() ;

		virtual void Init();

		virtual void SwapBuffers();

	private:
		void OnEvent(Event &event);

		bool OnWindowResized(WindowResizeEvent& e);

		void RequestSwapChainRecreate(int w, int h);

	private:
		GLFWwindow *mWindowHandle = nullptr;

		bool mIsMinimized = false;
		
	};
} // namespace BHive