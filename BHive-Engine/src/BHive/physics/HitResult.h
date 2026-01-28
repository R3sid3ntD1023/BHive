#pragma once

#include "core/Core.h"

namespace BHive
{
	class GameObject;
	struct Component;

	struct FHitResult
	{
		bool InitalOverlap{};

		glm::vec3 Normal{};
		glm::vec3 Position{};
		float Distance{};

		GameObject *Object = nullptr;
		Component *Component = nullptr;

		bool isValid() const { return Object && Component; }
	};
} // namespace BHive