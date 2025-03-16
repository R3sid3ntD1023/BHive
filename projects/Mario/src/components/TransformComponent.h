#pragma once

#include "Component.h"
#include "math/Transform.h"

namespace BHive
{
	struct TransformComponent : public Component
	{
		FTransform Transform{};
	};

} // namespace BHive