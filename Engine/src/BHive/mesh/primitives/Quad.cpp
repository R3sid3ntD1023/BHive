#include "Quad.h"

namespace BHive
{
	PQuad::PQuad()
		: StaticMesh(GetMeshData())
	{
	}

	FMeshData PQuad::GetMeshData() const
	{
		float s = 1.f;
		std::vector<FVertex> vertices = {
			FVertex{.Position = {-s, -s, 0}, .TexCoord = {0, 0}, .Normal = {0, 0, 1}},
			FVertex{.Position = {s, -s, 0}, .TexCoord = {1, 0}, .Normal = {0, 0, 1}},
			FVertex{.Position = {s, s, 0}, .TexCoord = {1, 1}, .Normal = {0, 0, 1}},
			FVertex{.Position = {-s, s, 0}, .TexCoord = {0, 1}, .Normal = {0, 0, 1}}};

		// CalculateTangentsAndBitTangents(vertices.data(), 4);

		FMeshData data;
		data.mIndices = {0, 1, 2, 2, 3, 0};
		data.mVertices = vertices;
		data.mSubMeshes.push_back(FSubMesh{.mMaterialIndex = 0, .mStartVertex = 0, .mStartIndex = 0, .mVertexCount = 4, .mIndexCount = 6});
		data.mBoundingBox = {{-1, -1, 0}, {1, 1, 0}};

		return data;
	}
} // namespace BHive