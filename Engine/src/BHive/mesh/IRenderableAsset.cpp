#include "gfx/VertexArray.h"
#include "IRenderableAsset.h"

namespace BHive
{
	IRenderableAsset::IRenderableAsset(const FMeshData &data)
		: mData(data)
	{
		Initialize();
	}

	void IRenderableAsset::Initialize()
	{
		auto &data = mData;
		auto indexbuffer = IndexBuffer::Create(data.mIndices.data(), (uint32_t)data.mIndices.size());
		auto vertexbuffer = VertexBuffer::Create(data.mVertices.size() * sizeof(FVertex));
		vertexbuffer->SetData(data.mVertices.data(), data.mVertices.size() * sizeof(FVertex));
		vertexbuffer->SetLayout(FVertex::Layout());

		mVertexArray = VertexArray::Create();
		mVertexArray->AddVertexBuffer(vertexbuffer);
		mVertexArray->SetIndexBuffer(indexbuffer);
	}

	void IRenderableAsset::Save(cereal::BinaryOutputArchive &ar) const
	{
		Asset::Save(ar);
		ar(mData, mMaterialTable);
	}

	void IRenderableAsset::Load(cereal::BinaryInputArchive &ar)
	{
		Asset::Load(ar);
		ar(mData, mMaterialTable);

		Initialize();
	}

	REFLECT(IRenderableAsset)
	{
		BEGIN_REFLECT(IRenderableAsset)
		REFLECT_PROPERTY("Materials", mMaterialTable);
	}
} // namespace BHive