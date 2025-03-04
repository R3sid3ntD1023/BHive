#include "Query.h"
#include <glad/glad.h>
#include <glm/common.hpp>

namespace BHive
{
	Query::Query()
	{
		glCreateQueries(GL_TIMESTAMP, 2, mQueryIDs);
	}

	Query::~Query()
	{
		glDeleteQueries(2, mQueryIDs);
	}

	void Query::Begin()
	{
		glQueryCounter(mQueryIDs[0], GL_TIMESTAMP);
	}

	void Query::End()
	{
		GLuint64 start, end;
		glQueryCounter(mQueryIDs[1], GL_TIMESTAMP);

		GLint available = 0;
		while (!available)
		{
			glGetQueryObjectiv(mQueryIDs[1], GL_QUERY_RESULT_AVAILABLE, &available);
		}

		glGetQueryObjectui64v(mQueryIDs[0], GL_QUERY_RESULT, &start);
		glGetQueryObjectui64v(mQueryIDs[1], GL_QUERY_RESULT, &end);

		// nanoseconds -> microseconds- > milliseconds
		auto elapsed = (end - start) / 1'000'000.f;
		mInstanceTime = elapsed;
		mTimeAccumulated += mInstanceTime;
		mMaxTime = glm::max(mMaxTime, mInstanceTime);
		mMinTime = glm::min(mMinTime, mInstanceTime);
	}
} // namespace BHive