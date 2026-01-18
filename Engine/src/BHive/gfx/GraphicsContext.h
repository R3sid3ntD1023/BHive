#pragma once

#include "core/Core.h"

namespace BHive
{
	class BHIVE_API GraphicsContext
	{
	public:
		virtual ~GraphicsContext() = default;

		virtual void Init() = 0;

		virtual void SwapBuffers() = 0;

		virtual void OnFramebufferResized(uint32_t w, uint32_t h) = 0;

		static GraphicsContext &Get() { return *sInstance; }

		static Scope<GraphicsContext> Create(void *windowHandle);

	private:
		static inline GraphicsContext *sInstance = nullptr;
	};
} // namespace BHive