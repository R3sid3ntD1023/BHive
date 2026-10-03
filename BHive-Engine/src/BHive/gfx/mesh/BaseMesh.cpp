#include "BaseMesh.h"
#include "MeshManager.h"
#include "glad/glad.h"

namespace BHive
{
	BaseMesh::BaseMesh(const FMeshData &data)
		: mData(data)
	{
		Initialize();
	}

	void BaseMesh::Initialize()
	{

		CreateVertexArrayBuffer();
	}

	void BaseMesh::Save(cereal::BinaryOutputArchive &ar) const
	{
		Asset::Save(ar);
		ar(mData);
	}

	void BaseMesh::Load(cereal::BinaryInputArchive &ar)
	{
		Asset::Load(ar);
		ar(mData);

		Initialize();
	}

	void BaseMesh::CreateVertexArrayBuffer()
	{
		auto geometry = MeshManager::Get().Register(mData);
		mVertexArray = geometry.VertexArray;
		mGeometryPage = geometry.Page;
		mBaseVertex = geometry.BaseVertex;
		mBaseIndex = geometry.BaseIndex;
	}

	REFLECT(BaseMesh)
	{
		BEGIN_REFLECT(BaseMesh);
	}

	void MeshUtils::CalculateTangentsAndBitTangents(FVertex *vertices, size_t size)
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
} // namespace BHive