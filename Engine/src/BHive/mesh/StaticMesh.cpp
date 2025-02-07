#include "gfx/VertexArray.h"
#include "StaticMesh.h"

namespace BHive
{
	StaticMesh::StaticMesh(const FMeshData &data)
		: IRenderableAsset(data)
	{
	}

	void StaticMesh::CalculateTangentsAndBitTangents(FVertex *vertices, size_t size)
	{
		for (size_t i = 0; i < size; i++)
		{
			auto &vertex = vertices[i];

			auto up = glm::vec3(0, 1, 0);
			auto normal = vertices[i].Normal;

			auto tangent = glm::cross(normal, up);
			auto bitangent = glm::cross(tangent, normal);
			tangent = glm::cross(normal, bitangent);

			vertex.BiNormal = bitangent;
			vertex.Tangent = tangent;
		}
	}

	void StaticMesh::Save(cereal::BinaryOutputArchive &ar) const
	{
		IRenderableAsset::Save(ar);
	}

	void StaticMesh::Load(cereal::BinaryInputArchive &ar)
	{
		IRenderableAsset::Load(ar);
	}

	REFLECT(StaticMesh)
	{
		BEGIN_REFLECT(StaticMesh)
		REFLECT_CONSTRUCTOR();
	}
} // namespace BHive