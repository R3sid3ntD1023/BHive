#include "BaseMesh.h"
#include "gfx/Buffers.h"
#include "gfx/VertexArray.h"
#include "gfx/factories/GFXFactories.h"
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
		ar(mData, mMaterialTable);
	}

	void BaseMesh::Load(cereal::BinaryInputArchive &ar)
	{
		Asset::Load(ar);
		ar(mData, mMaterialTable);

		Initialize();
	}

	void BaseMesh::CreateVertexArrayBuffer()
	{
		const auto &indices = mData.mIndices;
		const auto &vertices = mData.mVertices;

		auto indexbuffer = BufferFactory::CreateIndexBuffer((uint32_t)indices.size(), EBufferLifetime::Static, indices.data());
		auto vertexbuffer = BufferFactory::CreateVertexBuffer(vertices.size() * sizeof(FVertex), EBufferLifetime::Static, vertices.data());
		vertexbuffer.As<VertexBuffer>()->SetLayout(FVertex::Layout());

		mVertexArray = VertexArrayFactory::Create({vertexbuffer}, indexbuffer);
	}

	REFLECT(BaseMesh)
	{
		BEGIN_REFLECT(BaseMesh)
		REFLECT_PROPERTY("Materials", mMaterialTable);
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