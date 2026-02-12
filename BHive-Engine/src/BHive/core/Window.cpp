#include "Window.h"
#include "gfx/RenderCommand.h"
#include "gfx/WindowContext.h"
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

		GLFWwindow *shared_context = nullptr;
		if (RenderCommand::GetRendererAPI() == RendererAPI::Opengl)
		{
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if _DEBUG
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif // _DEBUG
			shared_context = glfwGetCurrentContext();
		}
		else
		{
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			shared_context = nullptr;
		}

		mWindow = glfwCreateWindow(properties.Size.x, properties.Size.y, properties.Title.c_str(), nullptr, shared_context);
		sWindowCount++;

		mContext = WindowContext::Create(mWindow);
		mContext->Init();
		mData.Instance = this;

		glfwSetWindowUserPointer(mWindow, &mData);

		RegisterCallbacks();

		SetVysnc(properties.VSync);

		if (properties.Maximize)
			glfwMaximizeWindow(mWindow);

		glfwShowWindow(mWindow);
	}

	Window::~Window()
	{
		mContext.reset();

		sWindowCount--;
		glfwDestroyWindow(mWindow);

		if (sWindowCount == 0)
		{
			glfwTerminate();
		}

		LOG_TRACE("Window Descructor Called")
	}

	void Window::Update()
	{
		mContext->SwapBuffers();
	}

	void Window::SetVysnc(bool enabled)
	{
		// glfwSwapInterval(enabled ? 1 : 0);
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

	void Window::SetTitle(const std::string &title)
	{
		glfwSetWindowTitle(mWindow, title.c_str());
		mData.Title = title;
	}

	void Window::SetEventCallback(FOnWindowInputEvent &event)
	{
		mData.Input.mEvent = event;
	}
	void Window::PollEvents()
	{
		glfwPollEvents();
	}

	void Window::OnWindowCloseCallback(GLFWwindow *window)
	{
		auto input = (FWindowData *)glfwGetWindowUserPointer(window);
		input->Input.OnWindowClose();
	}

	void Window::OnWindowResizeCallback(GLFWwindow *window, int width, int height)
	{
		auto input = (FWindowData *)glfwGetWindowUserPointer(window);
		input->Input.OnWindowResize(width, height);
		input->Size = {width, height};
	}

	void Window::OnKeyEventCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		auto input = (FWindowData *)glfwGetWindowUserPointer(window);
		input->Input.OnKeyEvent(key, scancode, action, mods);
	}

	void Window::OnMouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
	{
		auto input = (FWindowData *)glfwGetWindowUserPointer(window);
		input->Input.OnMouseButton(button, action, mods);
	}

	void Window::OnMouseScrollCallback(GLFWwindow *window, double x, double y)
	{
		auto input = (FWindowData *)glfwGetWindowUserPointer(window);
		input->Input.OnMouseScroll(x, y);
	}

	void Window::OnMouseMovedCallback(GLFWwindow *window, double x, double y)
	{
		auto input = (FWindowData *)glfwGetWindowUserPointer(window);
		input->Input.OnMouseMoved(x, y);
	}

	void Window::OnCharCallback(GLFWwindow *window, unsigned codepoint)
	{
		auto input = (FWindowData *)glfwGetWindowUserPointer(window);
		input->Input.OnKeyTypedEvent(codepoint);
	}

	void Window::OnFramebufferSizeCallback(GLFWwindow *window, int width, int height)
	{
		auto input = (FWindowData *)glfwGetWindowUserPointer(window);
		input->Instance->GetContext().OnFramebufferResized(width, height);
	}

	void Window::OnJoyStickCallback(int joystick, int status)
	{
		WindowInput::OnJoyStickConnected(joystick, status);

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

	GLFWwindow* Window::GetFocusedWindow()
	{
		if (RenderCommand::GetRendererAPI() == RendererAPI::Opengl)
			return glfwGetCurrentContext();

		return sFocusedWindow;
	}

	void Window::OnWindowFocusCallback(GLFWwindow* window, int focused)
	{
		if (focused)
		{
			Window::sFocusedWindow = window;
		}
		else
		{
			Window::sFocusedWindow = nullptr;
		}
	}


	void Window::RegisterCallbacks()
	{
		glfwSetWindowCloseCallback(mWindow, OnWindowCloseCallback);

		glfwSetWindowSizeCallback(mWindow, OnWindowResizeCallback);

		glfwSetKeyCallback(mWindow, OnKeyEventCallback);

		glfwSetMouseButtonCallback(mWindow,OnMouseButtonCallback);

		glfwSetScrollCallback(mWindow, OnMouseMovedCallback);

		glfwSetCursorPosCallback(mWindow, OnMouseMovedCallback);

		glfwSetCharCallback(mWindow, OnCharCallback);

		glfwSetFramebufferSizeCallback(mWindow, OnFramebufferSizeCallback);

		glfwSetWindowFocusCallback(mWindow, OnWindowFocusCallback);

		glfwSetJoystickCallback(OnJoyStickCallback);
	}
	
	GLFWwindow *Window::sFocusedWindow = nullptr;

	WindowManager &WindowManager::Get()
	{
		static WindowManager instance;
		return instance;
	}

	Window* WindowManager::Create(const FWindowProperties &properties)
	{
		auto window = CreateScope<Window>(properties);
		auto raw = window.get();
		mWindows.push_back(std::move(window));
		return raw;
	}

	void WindowManager::Shutdown()
	{
		mWindows.clear();
	}
} // namespace BHive