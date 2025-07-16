#pragma once

#include "asset/Asset.h"
#include "material/MaterialTable.h"
#include "MeshData.h"

namespace BHive
{
	class VertexArray;
	class StorageBuffer;

	class BaseMesh : public Asset
	{
	public:
		using submeshes = std::vector<FSubMesh>;
		using iterator = submeshes::iterator;
		using const_iterator = submeshes::const_iterator;

		BaseMesh() = default;
		BaseMesh(const FMeshData &data);

		void Initialize();

		const Ref<VertexArray> &GetVertexArray() const { return mVertexArray; }

		Ref<VertexArray> &GetVertexArray() { return mVertexArray; }

		const AABB &GetBoundingBox() const { return mData.mBoundingBox; }

		const FMeshData &GetData() const { return mData; }

		const std::vector<FSubMesh> &GetSubMeshes() const { return mData.mSubMeshes; }

		MaterialTable &GetMaterialTable() { return mMaterialTable; }

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

		iterator begin() { return mData.mSubMeshes.begin(); }
		iterator end() { return mData.mSubMeshes.end(); }

		const_iterator begin() const { return mData.mSubMeshes.begin(); }
		const_iterator end() const { return mData.mSubMeshes.end(); }

		REFLECTABLEV(Asset)

	private:
		void CreateVertexArrayBuffer();

	private:
		FMeshData mData;
		Ref<VertexArray> mVertexArray;
		MaterialTable mMaterialTable;
	};

	REFLECT_EXTERN(BaseMesh)
} // namespace BHive