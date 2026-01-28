#pragma once

#include "mesh/StaticMesh.h"

namespace BHive
{
	class BHIVE_API PCube : public StaticMesh
	{
	public:
		PCube(float size);

	private:
		FMeshData GetMeshData(float size);
	};
} // namespace BHive