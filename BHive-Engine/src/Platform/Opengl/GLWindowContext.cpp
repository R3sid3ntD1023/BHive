#include "GLWindowContext.h"
#include "core/Window.h"
#include "gfx/RenderCommand.h"
#include "GLRendererAPI.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace BHive
{
	GLWindowContext::GLWindowContext(Window *window)
	{
		ASSERT(window)
		mWindowHandle = window->GetNative();
		ASSERT(mWindowHandle)
	}

	void GLWindowContext::Init()
	{
		glfwMakeContextCurrent(mWindowHandle);

		auto success = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		if (!success)
			LOG_ERROR("GLRendererAPI::Init() Failed to load opengl")

		LOG_INFO("OPENGL INFO: ");
		LOG_INFO("\n\r\t {}", (const char *)glGetString(GL_VENDOR));
		LOG_INFO("\n\r\t {}", (const char *)glGetString(GL_VERSION));
		LOG_INFO("\n\r\t {}", (const char *)glGetString(GL_RENDERER));

		GLint num_extensions = 0;
		glGetIntegerv(GL_NUM_EXTENSIONS, &num_extensions);

		// std::vector<const char *> extensions(num_extensions);
		// for (int i = 0; i < num_extensions; i++)
		// {
		// 	extensions[i] = (const char *)glGetStringi(GL_EXTENSIONS, i);
		// }

		// LOG_INFO("GL available extensions: {} - {}", num_extensions, extensions);

		glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
		// glEnable(GL_POLYGON_SMOOTH);

		glfwSwapInterval(IsVSyncEnabled() ? 1 : 0);
	}

	void GLWindowContext::SwapBuffers()
	{
		auto api = RenderCommand::GetGraphicsAPI<GLRendererAPI>();
		api->RenderFrame();

		glfwSwapBuffers(mWindowHandle);
	}
} // namespace BHive