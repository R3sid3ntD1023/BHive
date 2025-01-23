#include "Window.h"
#include "WindowInput.h"
#include <glfw/glfw3.h>

namespace BHive
{
	static unsigned sWindowCount = 0;

	void GLFWErrorCallback(int error_code, const char *message)
	{
		LOG_ERROR(message);
	}

	Window::Window(const FWindowProperties &properties)
		: mProperties(properties)
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
		mWindow = glfwCreateWindow(properties.Width, properties.Height, properties.Title.c_str(), nullptr, nullptr);
		sWindowCount++;

		mContext = GraphicsContext::Create(mWindow);
		mContext->Init();

		glfwSetWindowUserPointer(mWindow, &mProperties);
		glfwSetWindowCloseCallback(mWindow, [](GLFWwindow *) { WindowInput::OnWindowClose(); });

		glfwSetWindowSizeCallback(mWindow, [](GLFWwindow *, int x, int y) { WindowInput::OnWindowResize(x, y); });

		glfwSetKeyCallback(
			mWindow, [](GLFWwindow *, int key, int scancode, int action, int mods) { WindowInput::OnKeyEvent(key, scancode, action, mods); });

		glfwSetMouseButtonCallback(mWindow, [](GLFWwindow *, int button, int action, int mods) { WindowInput::OnMouseButton(button, action, mods); });

		glfwSetScrollCallback(mWindow, [](GLFWwindow *, double x, double y) { WindowInput::OnMouseScroll(x, y); });

		glfwSetCursorPosCallback(mWindow, [](GLFWwindow *, double x, double y) { WindowInput::OnMouseMoved(x, y); });

		glfwSetCharCallback(mWindow, [](GLFWwindow *, unsigned codepoint) { WindowInput::OnKeyTypedEvent(codepoint); });

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

		SetVysnc(properties.VSync);

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

	void Window::PollEvents()
	{
		glfwPollEvents();
	}

	const char *Window::GetTitle() const
	{
		return glfwGetWindowTitle(mWindow);
	}

	std::pair<int, int> Window::GetPosition() const
	{
		int x, y;
		glfwGetWindowPos(mWindow, &x, &y);
		return {x, y};
	}

	std::pair<int, int> Window::GetSize() const
	{
		int x, y;
		glfwGetWindowSize(mWindow, &x, &y);
		return {x, y};
	}
} // namespace BHive