#include "MathFunctionLibrary.h"
#include "Transform.h"

namespace BHive
{
	template <typename T>
	inline T MathFunctionLibrary::mix(const T &t0, const T &t1, float w)
	{
		return glm::mix(t0, t1, w);
	}

	template <>
	inline FTransform MathFunctionLibrary::mix(const FTransform &t0, const FTransform &t1, float w)
	{
		auto translation = glm::mix(t0.get_translation(), t1.get_translation(), w);
		auto rotation = glm::slerp(t0.get_quaternion(), t1.get_quaternion(), w);
		auto scale = glm::mix(t0.get_scale(), t1.get_scale(), w);

		FTransform t;
		t.set_translation(translation);
		t.set_quaternion(rotation);
		t.set_scale(scale);

		return t;
	}

	template <typename T>
	inline T MathFunctionLibrary::lerp(const T &v0, const T &v1, float t)
	{
		return v0 + t * (v1 - v0);
	}

	template <typename T>
	inline T MathFunctionLibrary::remap(const T &v, const T &inMin, const T &inMax, const T &outMin, const T &outMax)
	{
		T in_span = inMax - inMin;
		T out_span = outMax - outMin;

		float scaled = (v - inMin) / in_span;

		return outMin + (scaled * out_span);
	}

	template <typename T>
	inline T MathFunctionLibrary::normalize(const T &v, const T &min, const T &max)
	{
		return (v - min) / ((max - min));
	}

} // namespace BHive