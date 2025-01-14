#pragma once

#include "math/Math.h"
#include <vector>

namespace BHive
{
	struct SkeletalNode
	{
		std::string mName;
		glm::mat4 mTransformation;
		std::vector<SkeletalNode> mChildren;
	};

}