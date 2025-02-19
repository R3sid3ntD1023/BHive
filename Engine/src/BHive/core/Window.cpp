#include "Window.h"

#include <glfw/glfw3.h>

namespace BHive
{
	static unsigned sWindowCount = 0;

	void GLFWErrorCallback(int error_code, const char *message)
	{
		LOG_ERROR(message);
	}

	Window::Window(const FWindowProperties &properties)
		: mData({properties.Title, properties.Size, properties.VSync})
	{
		if (sWindowCount == 0)
		{
			glfwInit();
			glfwSetErrorCallback(GLFWErrorCallback);
		}

		GLFWmonitor *primary_monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode *video_mode = glfwGetVideoMode(primary_monitor);

#if _DEBUG
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif // _DEBUG

		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		mWindow = glfwCreateWindow(properties.Size.x, properties.Size.y, properties.Title.c_str(), nullptr, nullptr);
		sWindowCount++;

		mContext = GraphicsContext::Create(mWindow);
		mContext->Init();

		glfwSetWindowUserPointer(mWindow, &mData);

		RegisterCallbacks();

		SetVysnc(properties.VSync);

		if (properties.Maximize)
			glfwMaximizeWindow(mWindow);

		glfwShowWindow(mWindow);
	}

	Window::~Window()
	{
		sWindowCount--;
		glfwDestroyWindow(mWindow);

		if (sWindowCount == 0)
		{
			glfwTerminate();
		}
	}

	void Window::Update()
	{
		mContext->SwapBuffers();
	}

	void Window::SetVysnc(bool enabled)
	{
		glfwSwapInterval(enabled ? 1 : 0);
	}

	void Window::Maximize()
	{
		if (mIsMaximized)
		{
			glfwRestoreWindow(mWindow);
			mIsMaximized = false;
		}
		else
		{
			glfwMaximizeWindow(mWindow);
			mIsMaximized = true;
		}
	}

	void Window::Minimize()
	{
		glfwIconifyWindow(mWindow);
		mIsMaximized = false;
	}

	void Window::SetPosition(int x, int y)
	{
		glfwSetWindowPos(mWindow, x, y);
	}

	void Window::SetEventCallback(FOnWindowInputEvent &event)
	{
		mData.Input.mEvent = event;
	}
	void Window::PollEvents()
	{
		glfwPollEvents();
	}

	void Window::RegisterCallbacks()
	{
		glfwSetWindowCloseCallback(
			mWindow,
			[](GLFWwindow *window)
			{
				auto input = (FWindowData *)glfwGetWindowUserPointer(window);
				input->Input.OnWindowClose();
			});

		glfwSetWindowSizeCallback(
			mWindow,
			[](GLFWwindow *window, int x, int y)
			{
				auto input = (FWindowData *)glfwGetWindowUserPointer(window);
				input->Input.OnWindowResize(x, y);
				input->mSize = {x, y};
			});

		glfwSetKeyCallback(
			mWindow,
			[](GLFWwindow *window, int key, int scancode, int action, int mods)
			{
				auto input = (FWindowData *)glfwGetWindowUserPointer(window);
				input->Input.OnKeyEvent(key, scancode, action, mods);
			});

		glfwSetMouseButtonCallback(
			mWindow,
			[](GLFWwindow *window, int button, int action, int mods)
			{
				auto input = (FWindowData *)glfwGetWindowUserPointer(window);
				input->Input.OnMouseButton(button, action, mods);
			});

		glfwSetScrollCallback(
			mWindow,
			[](GLFWwindow *window, double x, double y)
			{
				auto input = (FWindowData *)glfwGetWindowUserPointer(window);
				input->Input.OnMouseScroll(x, y);
			});

		glfwSetCursorPosCallback(
			mWindow,
			[](GLFWwindow *window, double x, double y)
			{
				auto input = (FWindowData *)glfwGetWindowUserPointer(window);
				input->Input.OnMouseMoved(x, y);
			});

		glfwSetCharCallback(
			mWindow,
			[](GLFWwindow *window, unsigned codepoint)
			{
				auto input = (FWindowData *)glfwGetWindowUserPointer(window);
				input->Input.OnKeyTypedEvent(codepoint);
			});

		glfwSetJoystickCallback(WindowInput::OnJoyStickConnected);

		for (int i = 0; i < GLFW_JOYSTICK_LAST; i++)
		{
			auto status = glfwJoystickPresent(i);
			if (status)
			{
				WindowInput::OnJoyStickConnected(i, GLFW_CONNECTED);
				break;
			}
		}
	}

} // namespace BHive