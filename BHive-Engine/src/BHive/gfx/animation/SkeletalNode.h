#pragma once

#include "AnimTransform.h"
#include "core/Core.h"

namespace BHive
{
	struct BHIVE_API SkeletalNode
	{
		uint64_t NameHash;

		AnimTransform Transformation;

		std::vector<SkeletalNode> Children;

		template <typename A>
		void Serialize(A &ar)
		{
			ar(NameHash, Transformation, Children);
		}
	};

} // namespace BHive