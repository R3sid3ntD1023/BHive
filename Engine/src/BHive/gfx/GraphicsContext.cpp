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

		LOG_INFO("OPENGL: ");
		LOG_INFO("{}", (const char *)glGetString(GL_VENDOR));
		LOG_INFO("{}", (const char *)glGetString(GL_VERSION));
		LOG_INFO("{}", (const char *)glGetString(GL_RENDERER));
	}

	void GraphicsContext::SwapBuffers()
	{
		glfwSwapBuffers(mWindowHandle);
	}
} // namespace BHive