#include "GLGraphicsContext.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>
#include "threading/Threading.h"

namespace BHive
{
	GLGraphicsContext::GLGraphicsContext(void *window)
		: mWindowHandle((GLFWwindow *)window)
	{
	}

	GLGraphicsContext::~GLGraphicsContext()
	{
	}

	void GLGraphicsContext::Init()
	{
		glfwMakeContextCurrent(mWindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

		ASSERT(status);

		BEGIN_THREAD_DISPATCH()
		LOG_INFO("OPENGL: ");
		LOG_INFO("{}", (const char *)glGetString(GL_VENDOR));
		LOG_INFO("{}", (const char *)glGetString(GL_VERSION));
		LOG_INFO("{}", (const char *)glGetString(GL_RENDERER));
		END_THREAD_DISPATCH()
	}

	void GLGraphicsContext::SwapBuffers()
	{

		glfwSwapBuffers(mWindowHandle);
	}
}