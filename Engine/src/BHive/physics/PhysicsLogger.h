#pragma once


#include "PhysicsCore.h"


namespace BHive
{
	class BHIVE PhysicsLogger : public rp3d::Logger
	{
	public:
		virtual void log(Level level, const std::string& physicsWorldName, Category category, const std::string& message, const char* filename, int lineNumber);
	};
}