#include "Plane.h"

namespace BHive
{
	PPlane::PPlane(float width, float height)
		: StaticMesh(GetMeshData(width, height))
	{
	}

	FMeshData PPlane::GetMeshData(float width, float height) const
	{
		float w = width;
		float h = height;

		std::vector<FVertex> vertices = {
			FVertex{.Position = {-w, -h, 0}, .TexCoord = {0, 0}, .Normal = {0, 0, 1}},
			FVertex{.Position = {w, -h, 0}, .TexCoord = {1, 0}, .Normal = {0, 0, 1}},
			FVertex{.Position = {w, h, 0}, .TexCoord = {1, 1}, .Normal = {0, 0, 1}},
			FVertex{.Position = {-w, h, 0}, .TexCoord = {0, 1}, .Normal = {0, 0, 1}}};

		// CalculateTangentsAndBitTangents(vertices.data(), 4);

		FMeshData data;
		data.mIndices = {0, 1, 2, 2, 3, 0};
		data.mVertices = vertices;
		data.mSubMeshes.push_back(FSubMesh{.StartVertex = 0, .StartIndex = 0, .IndexCount = 6, .MaterialIndex = 0});
		data.mBoundingBox = {{-1, -1, 0}, {1, 1, 0}};

		return data;
	}
} // namespace BHive