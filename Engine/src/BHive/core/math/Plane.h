#pragma once

#include "core/Core.h"
#include "Math.h"

namespace BHive
{
	struct BHIVE_API FPlane
	{
		glm::vec3 Normal{0, 1, 0};

		float Distance{0};
	};

} // namespace BHive
