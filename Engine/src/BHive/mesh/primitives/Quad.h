#pragma once

#include "mesh/StaticMesh.h"

namespace BHive
{
	class BHIVE_API PQuad : public StaticMesh
	{
	public:
		PQuad();

		FMeshData GetMeshData() const;
	};
} // namespace BHive