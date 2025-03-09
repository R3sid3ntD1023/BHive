#pragma once

#include "core/Core.h"

struct GLFWwindow;

namespace BHive
{
	class BHIVE GraphicsContext
	{
	public:
		using EXTENSIONS = std::unordered_set<std::string>;

	public:
		GraphicsContext(GLFWwindow *window);
		virtual ~GraphicsContext() = default;

		virtual void Init();
		virtual void SwapBuffers();

		static const EXTENSIONS &GetExtensions();

		static bool IsExtensionSupported(const std::string &extension);

	private:
		GLFWwindow *mWindowHandle;
		static inline EXTENSIONS sExtensions;
	};
} // namespace BHive