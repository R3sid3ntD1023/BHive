#pragma once

#include "core/Core.h"

namespace BHive
{
	struct Frustum;
	struct FTransform;

	struct BHIVE_API FVolume
	{
		virtual bool InFrustum(const Frustum &frustum, const FTransform &transform) const = 0;
	};
} // namespace BHive