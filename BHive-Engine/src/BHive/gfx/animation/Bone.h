#pragma once

#include "core/Core.h"

#define INVALID_BONE_INDEX 255

namespace BHive
{
	struct BHIVE_API Bone
	{
		uint64_t NameHash;

		int32_t ID = -1;

		glm::mat4 Offset = {1.0f};

		template <typename A>
		inline void Serialize(A &ar)
		{
			ar(NameHash, ID, Offset);
		}
	};

	REFLECT_INLINE(Bone)
	{
		BEGIN_REFLECT(Bone)
		REFLECT_PROPERTY_READ_ONLY("Name", NameHash)
		REFLECT_PROPERTY_READ_ONLY("ID", ID)
		REFLECT_PROPERTY_READ_ONLY("Offset", Offset);
	}

} // namespace BHive