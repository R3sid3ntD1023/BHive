#pragma once

#include "core/Core.h"
#include "gfx/GraphicsContext.h"

struct GLFWwindow;

namespace BHive
{
	struct FWindowProperties
	{
		std::string Title = "Window";
		unsigned Width = 800, Height = 600;

		bool VSync = true;
		bool mCenterWindow = true;
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

		static void PollEvents();

	public:
		GLFWwindow *GetNative() { return mWindow; }
		const FWindowProperties &GetProperties() const { return mProperties; }
		GraphicsContext &GetContext() { return *mContext; }
		const char *GetTitle() const;
		std::pair<int, int> GetPosition() const;
		std::pair<int, int> GetSize() const;

	private:
		bool mIsMaximized = false;
		GLFWwindow *mWindow = nullptr;
		FWindowProperties mProperties;
		Scope<GraphicsContext> mContext;
	};
}