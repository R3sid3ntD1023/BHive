#pragma once

#include "core/Core.h"
#include "core/EventDelegate.h"
#include "events/Event.h"
#include "gfx/GraphicsContext.h"
#include "WindowInput.h"

struct GLFWwindow;

namespace BHive
{

	struct BHIVE_API FWindowProperties
	{
		std::string Title = "Window";
		glm::ivec2 Size{800, 600};
		bool VSync = true;
		bool mCenterWindow = true;
		bool Maximize = false;
	};

	class BHIVE_API Window
	{
	public:

		Window(const FWindowProperties &properties = {});

		~Window();

		void Update();

		void SetVysnc(bool enabled);

		void Maximize();

		void Minimize();

		void SetPosition(int x, int y);

		void SetTitle(const std::string &title);

		void SetEventCallback(FOnWindowInputEvent &event);

		static void PollEvents();

	public:
		GLFWwindow *GetNative() { return mWindow; }

		GraphicsContext &GetContext() { return *mContext; }

		const std::string &GetTitle() const { return mData.Title; }

		const glm::ivec2 &GetSize() const { return mData.mSize; }

		float GetAspectRatio() const { return (float)mData.mSize.x / (float)mData.mSize.y; }

		bool IsVSyncEnabled() const { return mData.VSync; }

		static GLFWwindow *GetFocusedWindow();

	private:
		void RegisterCallbacks();

#pragma region  Callbacks
		static void OnWindowCloseCallback(GLFWwindow *window);

		static void OnWindowResizeCallback(GLFWwindow *window, int width, int height);

		static void OnKeyEventCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

		static void OnMouseButtonCallback(GLFWwindow *window, int button, int action, int mods);

		static void OnMouseScrollCallback(GLFWwindow *window, double x, double y);

		static void OnMouseMovedCallback(GLFWwindow *window, double x, double y);

		static void OnCharCallback(GLFWwindow *window, unsigned codepoint);

		static void OnFramebufferSizeCallback(GLFWwindow *window, int width, int height);

		static void OnJoyStickCallback(int joystick, int status);

		static void OnWindowFocusCallback(GLFWwindow *window, int focused);

#pragma endregion

		struct FWindowData
		{
			std::string Title;

			glm::ivec2 mSize;

			bool VSync;

			WindowInput Input;

			GraphicsContext *Context = nullptr;

			
		};


	private:
		bool mIsMaximized = false;
		GLFWwindow *mWindow = nullptr;
		FWindowData mData;
		Scope<GraphicsContext> mContext;
		static GLFWwindow *sFocusedWindow;

		friend void OnWindowFocusCallback(GLFWwindow *window, int focused);
	};

	class WindowManager
	{
	public:
	
		BHIVE_API Window *Create(const FWindowProperties &properties = {});

		BHIVE_API const std::vector<Scope<Window>> &GetWindows() const { return mWindows; }

		BHIVE_API void Shutdown();

		BHIVE_API static WindowManager &Get();

	private:

		 std::vector<Scope<Window>> mWindows;
	};
} // namespace BHive