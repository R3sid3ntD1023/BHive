#include "MathFunctionLibrary.h"
#include "Transform.h"

namespace BHive
{
	template <typename T>
	inline T MathFunctionLibrary::Mix(const T &t0, const T &t1, float w)
	{
		return glm::mix(t0, t1, w);
	}

	template <>
	inline FTransform MathFunctionLibrary::Mix(const FTransform &t0, const FTransform &t1, float w)
	{
		auto translation = glm::mix(t0.GetTranslation(), t1.GetTranslation(), w);
		auto rotation = glm::slerp(t0.GetQuaternion(), t1.GetQuaternion(), w);
		auto scale = glm::mix(t0.GetScale(), t1.GetScale(), w);

		FTransform t;
		t.SetTranslation(translation);
		t.SetQuaternion(rotation);
		t.SetScale(scale);

		return t;
	}

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

} // namespace BHive