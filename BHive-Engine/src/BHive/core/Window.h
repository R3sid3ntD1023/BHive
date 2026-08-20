#pragma once

#include "core/Core.h"
#include "delegates/EventDelegate.h"
#include "events/Event.h"
#include "events/KeyEvents.h"
#include "events/ApplicationEvents.h"

struct GLFWwindow;

namespace BHive
{
	class WindowContext;

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

		void ToggleFullScreen();

		void Maximize();

		void Minimize();

		void SetPosition(int x, int y);

		void SetTitle(const std::string &title);

		static void PollEvents();

		static void Init();

	public:
		GLFWwindow *GetNative() const { return mWindow; }

		WindowContext *GetContext() const { return mContext.get(); }

		const std::string &GetTitle() const { return mState.Title; }

		const glm::ivec2 &GetPosition() const { return mState.Position; }

		const glm::ivec2 &GetSize() const { return mState.Size; }

		float GetAspectRatio() const { return (float)mState.Size.x / (float)mState.Size.y; }

		bool IsVSyncEnabled() const { return mState.VSync; }

		bool IsMinimized() const { return mState.mIsMinimized; };

		void OnEvent(Event &e);

		bool OnKeyEvent(KeyEvent &e);

		bool OnWindowResizeEvent(WindowResizeEvent &e);

		struct FWindowState
		{
			std::string Title;

			glm::ivec2 Size;

			glm::ivec2 Position;

			bool VSync;

			bool mIsFullScreen = false;

			bool mIsMaximized = false;

			bool mIsMinimized = false;
		};

	private:
		GLFWwindow *mWindow = nullptr;
		FWindowState mState;
		Scope<WindowContext> mContext;

		friend void OnWindowFocusCallback(GLFWwindow *window, int focused);
	};

	class BHIVE_API WindowManager
	{
	public:
		Window *Create(const FWindowProperties &properties = {});

		void Update(float dt);

	private:
		std::vector<Ref<Window>> mWindows;
	};

} // namespace BHive