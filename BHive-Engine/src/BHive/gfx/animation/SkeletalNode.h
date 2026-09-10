#pragma once

#include "core/Core.h"
#include "core/math/Math.h"

namespace BHive
{
	struct BHIVE_API SkeletalNode
	{
		uint64_t NameHash;
		// glm::mat4 ParentTransformation{1.f};
		// glm::mat4 RelativeTransform{1.f};
		glm::mat4 Transformation{1.f};
		std::vector<SkeletalNode> Children;

		template <typename A>
		void Serialize(A &ar)
		{
			ar(NameHash, Transformation, Children);
		}
	};

} // namespace BHive