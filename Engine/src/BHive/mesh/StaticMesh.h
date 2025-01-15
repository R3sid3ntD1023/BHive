#pragma once

#include "asset/Asset.h"
#include "core/Core.h"
#include "material/MaterialTable.h"
#include "MeshData.h"

namespace BHive
{
	class VertexArray;

	class BHIVE StaticMesh : public Asset
	{
	public:
		using submeshes = std::vector<FSubMesh>;

		StaticMesh() = default;
		StaticMesh(const FMeshData &data);
		virtual ~StaticMesh() = default;

		const Ref<VertexArray> &GetVertexArray() const { return mVertexArray; }
		Ref<VertexArray> &GetVertexArray() { return mVertexArray; }
		const AABB &GetBoundingBox() const { return mData.mBoundingBox; }
		const FMeshData &GetData() const { return mData; }
		const std::vector<FSubMesh> &GetSubMeshes() const { return mData.mSubMeshes; }
		MaterialTable &GetMaterialTable() { return mMaterialTable; }

		submeshes::iterator begin() { return mData.mSubMeshes.begin(); }
		submeshes::iterator end() { return mData.mSubMeshes.end(); }

		submeshes::const_iterator begin() const { return mData.mSubMeshes.begin(); }
		submeshes::const_iterator end() const { return mData.mSubMeshes.end(); }

		static Ref<StaticMesh> CreatePlane(float width, float height);
		static Ref<StaticMesh> CreateCube(float size);
		static Ref<StaticMesh>
		CreateSphere(float radius = 0.5f, uint32_t sectors = 32, uint32_t stacks = 32);

		static void CalculateTangentsAndBitTangents(std::vector<FVertex> &vertices);

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLEV(Asset)

	private:
		void Initialize();

	private:
		FMeshData mData;
		Ref<VertexArray> mVertexArray;
		MaterialTable mMaterialTable;
	};

	REFLECT_EXTERN(StaticMesh)
} // namespace BHive