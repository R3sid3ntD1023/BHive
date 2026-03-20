#pragma once

#include "gfx/mesh/StaticMesh.h"

namespace BHive
{
	class BHIVE_API PQuad : public StaticMesh
	{
	public:
		PQuad();

		FMeshData GetMeshData() const;
	};
} // namespace BHive