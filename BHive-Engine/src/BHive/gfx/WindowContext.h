#pragma once

#include "core/Core.h"

namespace BHive
{
	class Window;

	class BHIVE_API WindowContext
	{
	public:
		virtual ~WindowContext() = default;

		virtual void Init() = 0;

		virtual void SwapBuffers() = 0;

		void SetVSync(bool enabled) { mIsVSyncEnabled = enabled; }

		bool IsVSyncEnabled() const { return mIsVSyncEnabled; }

		static WindowContext &Get() { return *sInstance; }

		static Scope<WindowContext> Create(Window *window);

	private:
		static inline WindowContext *sInstance = nullptr;

		bool mIsVSyncEnabled = false;
	};
} // namespace BHive