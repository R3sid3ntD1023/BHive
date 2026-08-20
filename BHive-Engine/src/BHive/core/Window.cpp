#include "Window.h"
#include "gfx/RenderCommand.h"
#include "gfx/WindowContext.h"
#include <glfw/glfw3.h>
#include "WindowInput.h"

namespace BHive
{
	static unsigned sWindowCount = 0;

	void GLFWErrorCallback(int error_code, const char *message)
	{
		LOG_ERROR("Window::GLFWErrorCallback {}", message);
		ASSERT(false)
	}

	Window::Window(const FWindowProperties &properties)
		: mState({properties.Title, properties.Size, {}, properties.VSync})
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

		WindowInput::RegisterCallbacks(mWindow, &mState);
		WindowInput::WindowEvent.Add(this, &Window::OnEvent);

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

		auto &fullScreen = mState.mIsFullScreen;
		if (!fullScreen)
		{
			glfwGetWindowPos(mWindow, &sPosition.x, &sPosition.y);
			glfwGetWindowSize(mWindow, &sSize.x, &sSize.y);

			GLFWmonitor *monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode *mode = glfwGetVideoMode(monitor);

			glfwSetWindowMonitor(mWindow, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
		}
		else
		{
			glfwSetWindowMonitor(mWindow, nullptr, sPosition.x, sPosition.y, sSize.x, sSize.y, 0);
			glfwSetWindowAttrib(mWindow, GLFW_DECORATED, GLFW_TRUE);
			glfwRestoreWindow(mWindow);
		}

		fullScreen = !fullScreen;
	}

	void Window::Maximize()
	{
		auto &maximized = mState.mIsMaximized;
		if (maximized)
		{
			glfwRestoreWindow(mWindow);
			maximized = false;
		}
		else
		{
			glfwMaximizeWindow(mWindow);
			maximized = true;
		}
	}

	void Window::Minimize()
	{
		glfwIconifyWindow(mWindow);
		mState.mIsMaximized = false;
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

	void Window::OnEvent(Event &e)
	{
		EventDispatcher dispachter(e);
		dispachter.Dispatch(this, &Window::OnKeyEvent);
	}

	bool Window::OnKeyEvent(KeyEvent &e)
	{
		if (e.Key == Key::F11 && e.Action == EventStatus::PRESS)
		{
			ToggleFullScreen();
			return true;
		}

		return false;
	}

	bool Window::OnWindowResizeEvent(WindowResizeEvent &e)
	{
		mState.Size = {e.x, e.y};
		mState.mIsMinimized = e.x <= 0 || e.y <= 0;
		return false;
	}

	Window *WindowManager::Create(const FWindowProperties &properties)
	{
		auto window = CreateRef<Window>(properties);
		auto raw = window.get();
		mWindows.push_back(std::move(window));
		return raw;
	}

	void WindowManager::Update(float dt)
	{
		for (auto &window : mWindows)
			window->Update();
	}
} // namespace BHive