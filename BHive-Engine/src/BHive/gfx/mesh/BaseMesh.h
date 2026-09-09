#pragma once

#include "MeshData.h"
#include "asset/Asset.h"
#include "gfx/registries/Handles.h"

namespace BHive
{
	class VertexArray;
	class StorageBuffer;

	class BHIVE_API BaseMesh : public Asset
	{
	public:
		using submeshes = std::vector<FSubMesh>;
		using iterator = submeshes::iterator;
		using const_iterator = submeshes::const_iterator;

		BaseMesh() = default;
		BaseMesh(const FMeshData &data);

		void Initialize();

		VertexArrayPtr GetVertexArray() const { return mVertexArray; }

		const FMeshData &GetData() const { return mData; }

		const auto &GetSubMeshes() const { return mData.SubMeshes; }

		size_t GetMaterialCount() const { return mData.MaterialCount; }

		void Save(cereal::BinaryOutputArchive &ar) const override;

		void Load(cereal::BinaryInputArchive &ar) override;

		iterator begin() { return mData.SubMeshes.begin(); }

		iterator end() { return mData.SubMeshes.end(); }

		const_iterator begin() const { return mData.SubMeshes.begin(); }

		const_iterator end() const { return mData.SubMeshes.end(); }

		virtual AABB GetBoundingBox() const { return mData.Bounds; }

		REFLECTABLEV(Asset)

	private:
		void CreateVertexArrayBuffer();

	private:
		FMeshData mData;

		VertexArrayPtr mVertexArray;
	};

	struct BHIVE_API MeshUtils
	{
		static void CalculateTangentsAndBitTangents(FVertex *vertices, size_t size);
	};

	REFLECT_EXTERN(BaseMesh)
} // namespace BHive