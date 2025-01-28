#pragma once

#include "core/Core.h"
#include "IRenderableAsset.h"
#include "material/MaterialTable.h"
#include "MeshData.h"

namespace BHive
{
	class VertexArray;

	class BHIVE StaticMesh : public IRenderableAsset
	{
	public:
		StaticMesh() = default;
		StaticMesh(const FMeshData &data);

		static Ref<StaticMesh> CreatePlane(float width, float height);
		static Ref<StaticMesh> CreateCube(float size);
		static Ref<StaticMesh> CreateSphere(float radius = 0.5f, uint32_t sectors = 32, uint32_t stacks = 32);

		static void CalculateTangentsAndBitTangents(std::vector<FVertex> &vertices);

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLEV(IRenderableAsset)
	};

	REFLECT_EXTERN(StaticMesh)
} // namespace BHive