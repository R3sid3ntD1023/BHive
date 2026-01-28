#pragma once

#include "mesh/StaticMesh.h"

namespace BHive
{
	class BHIVE_API PPlane : public StaticMesh
	{
	public:
		PPlane(float width, float height);

	private:
		FMeshData GetMeshData(float width, float height) const;
	};
} // namespace BHive