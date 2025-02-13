#include "GLQuery.h"
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace BHive
{
	GLQueryTimer::GLQueryTimer()
	{
		glCreateQueries(GL_TIME_ELAPSED_EXT, 1, &mQueryID);
	}

	GLQueryTimer::~GLQueryTimer()
	{
		glDeleteQueries(1, &mQueryID);
	}

	void GLQueryTimer::Begin()
	{
		glBeginQuery(GL_TIME_ELAPSED, mQueryID);
	}

	void GLQueryTimer::End()
	{
		glEndQuery(GL_TIME_ELAPSED);

		GLuint QueryTime(0);
		glGetQueryObjectuiv(mQueryID, GL_QUERY_RESULT, &QueryTime);

		mInstanceTime = (static_cast<double>(QueryTime) / 1000.0);
		mTimeAccumulated += mInstanceTime;
		mMaxTime = glm::max(mMaxTime, mInstanceTime);
		mMinTime = glm::min(mMinTime, mInstanceTime);
	}

} // namespace BHive