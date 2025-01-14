#pragma once

#include <cereal/cereal.hpp>
#include "mesh/MeshData.h"

namespace BHive
{
	template <typename A>
<<<<<<< HEAD
=======
	inline void CEREAL_SERIALIZE_FUNCTION_NAME(A &ar, FVertex &obj)
	{
		ar(cereal::make_nvp("Position", obj.Position), cereal::make_nvp("TexCoord", obj.TexCoord),
		   cereal::make_nvp("Normal", obj.Normal), cereal::make_nvp("Tangent", obj.Tangent),
		   cereal::make_nvp("BiNormal", obj.BiNormal), cereal::make_nvp("Color", obj.Color),
		   cereal::make_nvp("BoneIDs", cereal::binary_data(obj.mBoneID, 4 * sizeof(float))),
		   cereal::make_nvp("Weights", cereal::binary_data(obj.mWeights, 4 * sizeof(float))));
	}

	template <typename A>
>>>>>>> parent of 7ce9339 (reverted commit)
	inline void CEREAL_SERIALIZE_FUNCTION_NAME(A &ar, FSubMesh &obj)
	{
		ar(cereal::make_nvp("StartIndex", obj.mStartIndex),
		   cereal::make_nvp("StartVertex", obj.mStartVertex),
		   cereal::make_nvp("VertexCount", obj.mVertexCount),
		   cereal::make_nvp("IndexCount", obj.mIndexCount),
		   cereal::make_nvp("MaterialIndex", obj.mMaterialIndex),
		   cereal::make_nvp("InstanceCount", obj.mInstanceCount));
	}

	template <typename A>
<<<<<<< HEAD
	inline void CEREAL_SAVE_FUNCTION_NAME(A &ar, const FMeshData &obj)
	{
		ar(obj.mVertices.size());
		ar(obj.mIndices.size());
		ar(cereal::make_nvp("SubMeshes", obj.mSubMeshes),
		   cereal::make_nvp("BoundingBox", obj.mBoundingBox),
		   cereal::make_nvp(
			   "Vertices",
			   cereal::binary_data(obj.mVertices.data(), obj.mVertices.size() * sizeof(FVertex))),
		   cereal::make_nvp(
			   "Indices",
			   cereal::binary_data(obj.mIndices.data(), obj.mIndices.size() * sizeof(uint32_t))));
	}

	template <typename A>
	inline void CEREAL_LOAD_FUNCTION_NAME(A &ar, FMeshData &obj)
	{
		size_t num_vertices = 0;
		size_t num_indices = 0;

		ar(num_vertices);
		ar(num_indices);

		obj.mVertices.resize(num_vertices);
		obj.mIndices.resize(num_indices);

		ar(cereal::make_nvp("SubMeshes", obj.mSubMeshes),
		   cereal::make_nvp("BoundingBox", obj.mBoundingBox),
		   cereal::make_nvp(
			   "Vertices", cereal::binary_data(
							   const_cast<FVertex *>(obj.mVertices.data()),
							   obj.mVertices.size() * sizeof(FVertex))),
		   cereal::make_nvp(
			   "Indices", cereal::binary_data(
							  const_cast<uint32_t *>(obj.mIndices.data()),
							  obj.mIndices.size() * sizeof(uint32_t))));
=======
	inline void CEREAL_SERIALIZE_FUNCTION_NAME(A &ar, FMeshData &obj)
	{
		ar(cereal::make_nvp("SubMeshes", obj.mSubMeshes), cereal::make_nvp("Vertices", obj.mVertices),
		   cereal::make_nvp("Indices", obj.mIndices), cereal::make_nvp("BoundingBox", obj.mBoundingBox));
>>>>>>> parent of 7ce9339 (reverted commit)
	}

} // namespace BHive