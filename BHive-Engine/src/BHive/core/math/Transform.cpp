#include "Transform.h"
#include "core/debug/Instrumentor.h"

namespace BHive
{
	FTransform FTransform::Rotate(const glm::vec3 &eulerDegrees) const
	{
		FTransform result = *this;

		auto q = Quaternion() * glm::quat(glm::radians(eulerDegrees));

		result.Rotation = glm::degrees(glm::eulerAngles(q));

		return result;
	}

	glm::vec3 FTransform::TransformPoint(const glm::vec3 &p) const
	{
		return Translation + Quaternion() * (Scale * p);
	}

	glm::vec3 FTransform::GetForward() const
	{
		return glm::normalize(GetForwardUnnormalized());
	}

	glm::vec3 FTransform::GetForwardUnnormalized() const
	{
		return Quaternion() * glm::vec3(0, 0, -1);
	}

	glm::vec3 FTransform::GetRight() const
	{
		return Quaternion() * glm::vec3(1, 0, 0);
	}

	glm::vec3 FTransform::GetUp() const
	{
		return Quaternion() * glm::vec3(0, 1, 0);
	}

	FTransform FTransform::Inverse() const
	{
		auto q = Quaternion();
		auto invScale = 1.0f / Scale;
		auto invRotation = glm::inverse(q);
		auto invTranslation = invRotation * (-Translation * invScale);

		return {invTranslation, glm::degrees(glm::eulerAngles(invRotation)), invScale};
	}

	FTransform &FTransform::FromMat(const glm::mat4 &m)
	{
		glm::quat q;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(m, Scale, q, Translation, skew, perspective);

		Rotation = glm::degrees(glm::eulerAngles(q));

		return *this;
	}

	glm::mat4 FTransform::ToMat4() const
	{
		auto q = Quaternion();
		return glm::translate(Translation) * glm::toMat4(q) * glm::scale(Scale);
	}

	glm::quat FTransform::Quaternion() const
	{
		return glm::quat(glm::radians(Rotation));
	}

	std::string FTransform::ToString() const
	{
		return "{" + std::format("{},{},{} ", glm::to_string(Translation), glm::to_string(Rotation), glm::to_string(Scale)) + "}";
	}

	FTransform FTransform::operator*(const FTransform &rhs) const
	{
		FTransform result;

		auto q0 = Quaternion();
		auto q1 = rhs.Quaternion();
		auto q = glm::normalize(q0 * q1);

		result.Translation = Translation + q0 * (Scale * rhs.Translation);

		result.Rotation = glm::degrees(glm::eulerAngles(q));
		result.Scale = Scale * rhs.Scale;

		return result;
	}

	bool FTransform::operator==(const FTransform &rhs) const
	{
		return Translation == rhs.Translation && Rotation == rhs.Rotation && Scale == rhs.Scale;
	}

} // namespace BHive
