#pragma once

#include "mesh/StaticMesh.h"

namespace BHive
{
	class PQuad : public StaticMesh
	{
	public:
		PQuad();

		FMeshData GetMeshData() const;
	};
} // namespace BHive