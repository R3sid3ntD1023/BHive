#include "GraphicsContext.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace BHive
{

	GraphicsContext::GraphicsContext(GLFWwindow *window)
		: mWindowHandle(window)
	{
	}

	void GraphicsContext::Init()
	{
		glfwMakeContextCurrent(mWindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

		ASSERT(status);

		// get extensions
		GLint num_extensions = 0;
		glGetIntegerv(GL_NUM_EXTENSIONS, &num_extensions);

		LOG_INFO("OPENGL: ");
		LOG_INFO("{}", (const char *)glGetString(GL_VENDOR));
		LOG_INFO("{}", (const char *)glGetString(GL_VERSION));
		LOG_INFO("{}", (const char *)glGetString(GL_RENDERER));
		LOG_INFO("Extensions {}", num_extensions);

		for (int i = 0; i < num_extensions; i++)
		{
			const char *ext = (const char *)glGetStringi(GL_EXTENSIONS, i);
			LOG_INFO("Loaded extension: {}", ext);
		}
	}

	void GraphicsContext::SwapBuffers()
	{
		glfwSwapBuffers(mWindowHandle);
	}

} // namespace BHive