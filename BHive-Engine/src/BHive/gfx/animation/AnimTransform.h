#pragma once

#include "core/math/Math.h"

namespace BHive
{
	struct AnimTransform
	{
		glm::vec3 Position{0, 0, 0};
		glm::quat Rotation{glm::identity<glm::quat>()};
		glm::vec3 Scale{1, 1, 1};

		glm::vec3 TransformPoint(const glm::vec3 &p) const;

		AnimTransform &FromMat(const glm::mat4 &m);

		glm::mat4 ToMat4() const;

		template <typename Ar>
		void Serialize(Ar &ar)
		{
			ar(Position, Rotation, Scale);
		}

		AnimTransform operator*(const AnimTransform &rhs) const;
	};

} // namespace BHive