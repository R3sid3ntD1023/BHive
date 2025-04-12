#pragma once

#include "core/Core.h"
#include "core/EventDelegate.h"
#include "events/Event.h"
#include "gfx/GraphicsContext.h"
#include "Platform/VulkanContext.h"
#include "WindowInput.h"

struct GLFWwindow;

namespace BHive
{

	struct FWindowProperties
	{
		std::string Title = "Window";
		glm::ivec2 Size{800, 600};
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
		void SetTitle(const std::string &title);
		void SetEventCallback(FOnWindowInputEvent &event);

		static void PollEvents();

	public:
		GLFWwindow *GetNative() { return mWindow; }
		// GraphicsContext &GetContext() { return *mContext; }
		VulkanContext &GetContext() { return *mContext; }
		const std::string &GetTitle() const { return mData.Title; }
		const glm::ivec2 &GetSize() const { return mData.mSize; }
		bool IsVSyncEnabled() const { return mData.VSync; }

	private:
		void RegisterCallbacks();

		struct FWindowData
		{
			std::string Title;
			glm::ivec2 mSize;
			bool VSync;
			WindowInput Input;
		};

	private:
		bool mIsMaximized = false;
		GLFWwindow *mWindow = nullptr;
		FWindowData mData;
		// Scope<GraphicsContext> mContext;
		Scope<VulkanContext> mContext;
	};
} // namespace BHive