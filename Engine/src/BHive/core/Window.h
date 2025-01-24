#pragma once

#include "core/Core.h"
#include "core/EventDelegate.h"
#include "events/Event.h"
#include "gfx/GraphicsContext.h"
#include "WindowInput.h"

struct GLFWwindow;

namespace BHive
{

	struct FWindowProperties
	{
		std::string Title = "Window";
		unsigned Width = 800, Height = 600;

		bool VSync = true;
		bool mCenterWindow = true;
		bool Maximize = false;
	};

	class BHIVE Window
	{
	public:
		Window(const FWindowProperties &properties = {});
		~Window();

		void Update();
		void SetVysnc(bool enabled);
		void Maximize();
		void Minimize();
		void SetPosition(int x, int y);
		void SetEventCallback(FOnWindowInputEvent &event);

		static void PollEvents();

	public:
		GLFWwindow *GetNative() { return mWindow; }
		GraphicsContext &GetContext() { return *mContext; }
		const std::string &GetTitle() const { return mData.Title; }
		float GetWidth() const { return mData.Width; }
		float GetHeight() const { return mData.Height; }
		const Input &GetInput() const { return mData.Input.GetInput(); }

	private:
		void RegisterCallbacks();

		struct FWindowData
		{
			std::string Title;
			unsigned Width, Height;
			bool VSync;
			WindowInput Input;
		};

	private:
		bool mIsMaximized = false;
		GLFWwindow *mWindow = nullptr;
		FWindowData mData;
		Scope<GraphicsContext> mContext;
	};
} // namespace BHive