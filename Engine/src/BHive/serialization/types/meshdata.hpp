#pragma once

#include <cereal/cereal.hpp>
#include "mesh/MeshData.h"

namespace BHive
{
	template <typename A>
	inline void CEREAL_SERIALIZE_FUNCTION_NAME(A &ar, FVertex &obj)
	{
		ar(cereal::make_nvp("Position", obj.Position), cereal::make_nvp("TexCoord", obj.TexCoord),
		   cereal::make_nvp("Normal", obj.Normal), cereal::make_nvp("Tangent", obj.Tangent),
		   cereal::make_nvp("BiNormal", obj.BiNormal), cereal::make_nvp("Color", obj.Color),
		   cereal::make_nvp("BoneIDs", cereal::binary_data(obj.mBoneID, 4 * sizeof(float))),
		   cereal::make_nvp("Weights", cereal::binary_data(obj.mWeights, 4 * sizeof(float))));
	}

	template <typename A>
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
	inline void CEREAL_SERIALIZE_FUNCTION_NAME(A &ar, FMeshData &obj)
	{
		ar(cereal::make_nvp("SubMeshes", obj.mSubMeshes), cereal::make_nvp("Vertices", obj.mVertices),
		   cereal::make_nvp("Indices", obj.mIndices), cereal::make_nvp("BoundingBox", obj.mBoundingBox));
	}

} // namespace BHive