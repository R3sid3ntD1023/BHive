#include "AnimTransform.h"

namespace BHive
{
	AnimTransform AnimTransform::operator*(const AnimTransform &rhs) const
	{
		AnimTransform result;

		result.Position = Position + Rotation * (Scale * rhs.Position);

		result.Rotation = glm::normalize(Rotation * rhs.Rotation);

		result.Scale = Scale * rhs.Scale;

		return result;
	}

	glm::vec3 AnimTransform::TransformPoint(const glm::vec3 &p) const
	{
		return Position + Rotation * (Scale * p);
	}

	AnimTransform &AnimTransform::FromMat(const glm::mat4 &m)
	{
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(m, Scale, Rotation, Position, skew, perspective);

		return *this;
	}

	glm::mat4 AnimTransform::ToMat4() const
	{
		return glm::translate(Position) * glm::toMat4(Rotation) * glm::scale(Scale);
	}

} // namespace BHive