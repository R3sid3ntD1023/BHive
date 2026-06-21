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
		: mState({properties.Title, properties.Size, {} , properties.VSync})
	{
		if (sWindowCount == 0)
		{
			glfwInit();
			glfwSetErrorCallback(GLFWErrorCallback);
		}

		GLFWwindow *shared_context = nullptr;

		if (RenderCommand::GetAPI() == RendererAPI::Opengl)
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

		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		mWindow = glfwCreateWindow(properties.Size.x, properties.Size.y, properties.Title.c_str(), nullptr, shared_context);
		sWindowCount++;

		mContext = WindowContext::Create(this);
		mContext->Init();
		mState.Instance = this;

		glfwSetWindowUserPointer(mWindow, &mState);

		RegisterCallbacks();

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

	void Window::ToggleFullScreen()
	{
		static glm::ivec2 sSize{};
		static glm::ivec2 sPosition{};

		if (!mIsFullScreen)
		{
			glfwGetWindowPos(mWindow, &sPosition.x, &sPosition.y);
			glfwGetWindowSize(mWindow, &sSize.x, &sSize.y);

			GLFWmonitor *monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode *mode = glfwGetVideoMode(monitor);

			glfwSetWindowMonitor(mWindow, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
		}
		else
		{
			glfwSetWindowMonitor(mWindow, nullptr,sPosition.x, sPosition.y, sSize.x, sSize.y, 0);
			glfwSetWindowAttrib(mWindow, GLFW_DECORATED, GLFW_TRUE);
			glfwRestoreWindow(mWindow);

		}

		mIsFullScreen = !mIsFullScreen;
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
		mState.Title = title;
	}

	void Window::PollEvents()
	{
		glfwPollEvents();
	}

	void Window::OnWindowCloseCallback(GLFWwindow *window)
	{
		auto input = (FWindowState *)glfwGetWindowUserPointer(window);
		input->Input.OnWindowClose();
	}

	void Window::OnWindowResizeCallback(GLFWwindow *window, int width, int height)
	{
		auto input = (FWindowState *)glfwGetWindowUserPointer(window);
		input->Input.OnWindowResize(width, height);
		input->Size = {width, height};
	}

	void Window::OnWindowMovedCallback(GLFWwindow *window, int x, int y)
	{
		auto input = (FWindowState *)glfwGetWindowUserPointer(window);
		input->Position = {x, y};
	}

	void Window::OnKeyEventCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		auto state = (FWindowState *)glfwGetWindowUserPointer(window);
		if (key == GLFW_KEY_F11 && action == GLFW_PRESS)
		{
			state->Instance->ToggleFullScreen();
			return;
		}

		state->Input.OnKeyEvent(key, scancode, action, mods);
	}

	void Window::OnMouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
	{
		auto input = (FWindowState *)glfwGetWindowUserPointer(window);
		input->Input.OnMouseButton(button, action, mods);
	}

	void Window::OnMouseScrollCallback(GLFWwindow *window, double x, double y)
	{
		auto input = (FWindowState *)glfwGetWindowUserPointer(window);
		input->Input.OnMouseScroll(x, y);
	}

	void Window::OnMouseMovedCallback(GLFWwindow *window, double x, double y)
	{
		auto input = (FWindowState *)glfwGetWindowUserPointer(window);
		input->Input.OnMouseMoved(x, y);
	}

	void Window::OnCharCallback(GLFWwindow *window, unsigned codepoint)
	{
		auto input = (FWindowState *)glfwGetWindowUserPointer(window);
		input->Input.OnKeyTypedEvent(codepoint);
	}

	void Window::OnFramebufferSizeCallback(GLFWwindow *window, int width, int height)
	{
		auto input = (FWindowState *)glfwGetWindowUserPointer(window);
		input->Input.OnFramebufferResized(width, height);
	}

	void Window::OnJoyStickCallback(int joystick, int status)
	{
		WindowInput::OnJoyStickConnected(joystick, status);

		for (int i = 0; i < GLFW_JOYSTICK_LAST; i++)
		{
			auto present = glfwJoystickPresent(i);
			if (present)
			{
				WindowInput::OnJoyStickConnected(i, GLFW_CONNECTED);
				break;
			}
		}
	}

	GLFWwindow* Window::GetFocusedWindow()
	{
		if (RenderCommand::GetAPI() == RendererAPI::Opengl)
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

		glfwSetWindowPosCallback(mWindow, OnWindowMovedCallback);

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