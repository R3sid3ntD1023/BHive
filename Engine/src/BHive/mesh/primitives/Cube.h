#pragma once

#include "mesh/StaticMesh.h"

namespace BHive
{
	class PCube : public StaticMesh
	{
	public:
		PCube(float size);

	private:
		FMeshData GetMeshData(float size);
	};
} // namespace BHive