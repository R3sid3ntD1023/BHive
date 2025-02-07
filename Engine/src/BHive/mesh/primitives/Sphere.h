#pragma once

#include "mesh/StaticMesh.h"

namespace BHive
{
	class PSphere : public StaticMesh
	{
	public:
		PSphere(float radius, uint32_t sectors, uint32_t stacks);

	private:
		FMeshData GetMeshData(float radius, uint32_t sectors, uint32_t stacks) const;
	};
} // namespace BHive