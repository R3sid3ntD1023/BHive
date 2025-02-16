#include "GLTransformFeedback.h"
#include <glad/glad.h>

namespace BHive
{
	GLTransformFeedback::GLTransformFeedback(size_t size)
	{
		glCreateBuffers(1, &mTransformFeedbackID);
		glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, mTransformFeedbackID);
		glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, size, nullptr, GL_STATIC_READ);
	}

	GLTransformFeedback::~GLTransformFeedback()
	{
		glDeleteBuffers(1, &mTransformFeedbackID);
	}

	void GLTransformFeedback::GetData(void *data, size_t size)
	{
	}

	void GLTransformFeedback::Begin(uint32_t mode)
	{
		glBeginTransformFeedback(mode);
	}

	void GLTransformFeedback::End()
	{
		glEndTransformFeedback();
	}

} // namespace BHive
