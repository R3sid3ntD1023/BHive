#pragma once

#include "Inspector.h"

namespace BHive
{
	struct Inspector_UUID : public Inspector
	{
		virtual bool Inspect(FPropertyData &property_data, const bool is_read_only) override;

		REFLECTABLEV(Inspector)
	};

	template <typename T>
	struct Inspector_Vec : public Inspector
	{
		virtual bool Inspect(FPropertyData &property_data, const bool is_read_only) override;

		REFLECTABLEV(Inspector)
	};

	struct Inspector_Color : public Inspector
	{
		virtual bool Inspect(FPropertyData &property_data, const bool is_read_only) override;

		REFLECTABLEV(Inspector)
	};

	REFLECT_EXTERN(Inspector_UUID)
	REFLECT_EXTERN(Inspector_Color)
	REFLECT_EXTERN(Inspector_Vec<glm::vec2>)
	REFLECT_EXTERN(Inspector_Vec<glm::vec3>)
	REFLECT_EXTERN(Inspector_Vec<glm::vec4>)
} // namespace BHive
