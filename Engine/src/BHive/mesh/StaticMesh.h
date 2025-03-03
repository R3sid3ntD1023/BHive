#pragma once

#include "core/Core.h"
#include "BaseMesh.h"
#include "material/MaterialTable.h"
#include "MeshData.h"

namespace BHive
{
	class VertexArray;

	class BHIVE StaticMesh : public BaseMesh
	{
	public:
		StaticMesh() = default;
		StaticMesh(const FMeshData &data);

		static void CalculateTangentsAndBitTangents(FVertex *vertices, size_t size);

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLEV(BaseMesh)
	};

	REFLECT_EXTERN(StaticMesh)
} // namespace BHive