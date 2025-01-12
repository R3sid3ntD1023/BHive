#include "PhysicsLogger.h"

namespace BHive
{
	void PhysicsLogger::log(Level level, const std::string &physicsWorldName, Category category,
							const std::string &message, const char *filename, int lineNumber)
	{
		switch (level)
		{
		case reactphysics3d::Logger::Level::Error:
		{
			LOG_ERROR("{} - {}", physicsWorldName, message);
			break;
		}
		case reactphysics3d::Logger::Level::Warning:
		{
			LOG_WARN("{} - {}", physicsWorldName, message);
			break;
		}
		case reactphysics3d::Logger::Level::Information:
		{
			LOG_TRACE("{} - {}", physicsWorldName, message);
			break;
		}
		default:
			break;
		}
	}
} // namespace BHive