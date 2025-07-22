#pragma once

#include "Math.h"

namespace BHive
{
	struct FPlane
	{
		glm::vec3 Normal{0, 1, 0};
		float Distance{0};
	};

} // namespace BHive
