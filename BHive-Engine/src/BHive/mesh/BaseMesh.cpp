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
		auto &data = mData;
		auto indexbuffer = IndexBuffer::Create((uint32_t)data.mIndices.size(), EBufferUsageType::Static);
		indexbuffer->SetData(data.mIndices.data(), data.mIndices.size() * sizeof(uint32_t));

		auto vertexbuffer = VertexBuffer::Create(data.mVertices.size() * sizeof(FVertex), EBufferUsageType::Static);
		vertexbuffer->SetData(data.mVertices.data(), data.mVertices.size() * sizeof(FVertex));
		vertexbuffer->SetLayout(FVertex::Layout());

		mVertexArray = VertexArray::Create({vertexbuffer}, indexbuffer);
	}

	REFLECT(BaseMesh)
	{
		BEGIN_REFLECT(BaseMesh)
		REFLECT_PROPERTY("Materials", mMaterialTable);
	}
} // namespace BHive