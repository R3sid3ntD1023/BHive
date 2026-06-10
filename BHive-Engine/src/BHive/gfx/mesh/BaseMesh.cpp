#include "BaseMesh.h"
#include "gfx/Buffers.h"
#include "gfx/VertexArray.h"
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

		auto indexbuffer = IndexBuffer::Create((uint32_t)indices.size(), EBufferUsageType::Static, indices.data());
		auto vertexbuffer = VertexBuffer::Create(vertices.size() * sizeof(FVertex), EBufferUsageType::Static, vertices.data());
		vertexbuffer->SetLayout(FVertex::Layout());

		mVertexArray = VertexArray::Create({vertexbuffer}, indexbuffer);
	}

	REFLECT(BaseMesh)
	{
		BEGIN_REFLECT(BaseMesh)
		REFLECT_PROPERTY("Materials", mMaterialTable);
	}
} // namespace BHive