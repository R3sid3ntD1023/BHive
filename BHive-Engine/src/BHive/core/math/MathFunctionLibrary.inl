#include "MathFunctionLibrary.h"
#include "Transform.h"
#include "gfx/animation/AnimTransform.h"

namespace BHive
{

	template <typename T>
	inline T MathFunctionLibrary::Lerp(const T &v0, const T &v1, float t)
	{
		return v0 + t * (v1 - v0);
	}

	template <typename T>
	inline T MathFunctionLibrary::Remap(const T &v, const T &inMin, const T &inMax, const T &outMin, const T &outMax)
	{
		T in_span = inMax - inMin;
		T out_span = outMax - outMin;

		float scaled = (v - inMin) / in_span;

		return outMin + (scaled * out_span);
	}

	template <typename T>
	inline T MathFunctionLibrary::Normalize(const T &v, const T &min, const T &max)
	{
		return (v - min) / ((max - min));
	}

	template <>
	inline FTransform MathFunctionLibrary::Lerp(const FTransform &a, const FTransform &b, float t)
	{
		FTransform result;

		result.Translation = glm::mix(a.Translation, b.Translation, t);
		result.Scale = glm::mix(a.Scale, b.Scale, t);

		auto q0 = a.Quaternion();
		auto q1 = b.Quaternion();

		auto q = glm::normalize(glm::slerp(q0, q1, t));

		result.Rotation = glm::degrees(glm::eulerAngles(q));

		return result;
	}

	template <>
	inline AnimTransform MathFunctionLibrary::Lerp(const AnimTransform &a, const AnimTransform &b, float t)
	{
		AnimTransform result{};

		result.Position = glm::mix(a.Position, b.Position, t);
		result.Scale = glm::mix(a.Scale, b.Scale, t);
		result.Rotation = glm::normalize(glm::slerp(a.Rotation, b.Rotation, t));

		return result;
	}

} // namespace BHive