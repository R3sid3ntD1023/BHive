#pragma once

#include "math/Math.h"

namespace BHive
{
	struct Bone
	{
		std::string mName;
		int32_t mID;
		glm::mat4 mOffset;
		int32_t mParent = -1;

		template <typename A>
		inline void Serialize(A &ar)
		{
			ar(mName, mID, mOffset, mParent);
		}
	};

} // namespace BHive