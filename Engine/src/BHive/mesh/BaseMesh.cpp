#include "gfx/VertexArray.h"
#include "BaseMesh.h"
#include "glad/glad.h"
#include "gfx/StorageBuffer.h"

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
		auto indexbuffer = CreateRef<IndexBuffer>(data.mIndices.data(), (uint32_t)data.mIndices.size());
		auto vertexbuffer = CreateRef<VertexBuffer>(data.mVertices.size() * sizeof(FVertex));
		vertexbuffer->SetData(data.mVertices.data(), data.mVertices.size() * sizeof(FVertex));
		vertexbuffer->SetLayout(FVertex::Layout());

		mVertexArray = CreateRef<VertexArray>();
		mVertexArray->AddVertexBuffer(vertexbuffer);
		mVertexArray->SetIndexBuffer(indexbuffer);
	}

	REFLECT(BaseMesh)
	{
		BEGIN_REFLECT(BaseMesh)
		REFLECT_PROPERTY("Materials", mMaterialTable);
	}
} // namespace BHive