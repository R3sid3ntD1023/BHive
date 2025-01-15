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

		template <typename A>
		void Serialize(A &ar)
		{
			ar(mName, mTransformation, mChildren);
		}
	};

} // namespace BHive