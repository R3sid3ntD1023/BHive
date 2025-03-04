#include "TransformFeedback.h"
#include <glad/glad.h>

namespace BHive
{
	TransformFeedback::TransformFeedback(size_t size)
	{
		glCreateBuffers(1, &mTransformFeedback);
		glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, mTransformFeedback);
		glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, size, nullptr, GL_STATIC_READ);
	}

	TransformFeedback::~TransformFeedback()
	{
		glDeleteBuffers(1, &mTransformFeedback);
	}

	void TransformFeedback::GetData(void *data, size_t size)
	{
	}

	void TransformFeedback::Begin(uint32_t mode)
	{
		glBeginTransformFeedback(mode);
	}

	void TransformFeedback::End()
	{
		glEndTransformFeedback();
	}
} // namespace BHive