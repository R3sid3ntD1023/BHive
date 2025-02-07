#pragma once

#include "mesh/StaticMesh.h"

namespace BHive
{
	class PQuad : public StaticMesh
	{
	public:
		PQuad();

	private:
		FMeshData GetMeshData() const;
	};
} // namespace BHive