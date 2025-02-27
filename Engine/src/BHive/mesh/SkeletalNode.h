#pragma once

#include "math/Math.h"
#include <vector>

namespace BHive
{
	struct SkeletalNode
	{
		std::string mName;
		glm::mat4 ParentTransformation{1.f};
		glm::mat4 RelativeTransform{1.f};
		glm::mat4 mTransformation{1.f};
		std::vector<SkeletalNode> mChildren;

		template <typename A>
		void Serialize(A &ar)
		{
			ar(mName, mTransformation, mChildren);
		}
	};

} // namespace BHive