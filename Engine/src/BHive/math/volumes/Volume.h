#pragma once

namespace BHive
{
	struct Frustum;
	struct FTransform;

	struct FVolume
	{
		virtual bool InFrustum(const Frustum &frustum, const FTransform &transform) const = 0;
	};
} // namespace BHive