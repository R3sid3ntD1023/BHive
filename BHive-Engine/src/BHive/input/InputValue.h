#pragma once

#include "core/Core.h"

namespace BHive
{
	struct BHIVE_API InputValue
	{
		typedef float Axis1D;
		typedef glm::vec2 Axis2D;
		typedef bool Button;

		InputValue() = default;
		InputValue(Button button)
			: mValue((button ? 1.0f : 0.0f), 0.0f, 0.0f)
		{
		}
		InputValue(Axis1D axis)
			: mValue(axis, 0.0f, 0.0f)
		{
		}

		InputValue(const Axis2D &axis)
			: mValue(axis, 0.0f)
		{
		}

		template <typename T>
		const T Get() const;

	private:
		glm::vec3 mValue;
	};
} // namespace BHive
