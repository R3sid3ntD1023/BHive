#pragma once

#include "Inspector.h"

namespace BHive
{
	struct Inspector_UUID : public Inspector
	{
		INSPECTOR_BODY()

		REFLECTABLEV(Inspector)
	};

	template <typename T>
	struct Inspector_Vec : public Inspector
	{
		INSPECTOR_BODY()

		REFLECTABLEV(Inspector)
	};

	struct Inspector_Color : public Inspector
	{
		INSPECTOR_BODY()

		REFLECTABLEV(Inspector)
	};

	REFLECT_EXTERN(Inspector_UUID)
	REFLECT_EXTERN(Inspector_Color)
	REFLECT_EXTERN(Inspector_Vec<glm::vec2>)
	REFLECT_EXTERN(Inspector_Vec<glm::vec3>)
	REFLECT_EXTERN(Inspector_Vec<glm::vec4>)
} // namespace BHive
