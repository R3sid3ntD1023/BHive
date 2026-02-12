#pragma once

#include "core/Core.h"

namespace BHive
{
	class BHIVE_API WindowContext
	{
	public:
		virtual ~WindowContext() = default;

		virtual void Init() = 0;

		virtual void SwapBuffers() = 0;

		virtual void OnFramebufferResized(uint32_t w, uint32_t h) = 0;

		static WindowContext &Get() { return *sInstance; }

		static Scope<WindowContext> Create(void *windowHandle);

	private:
		static inline WindowContext *sInstance = nullptr;
	};
} // namespace BHive