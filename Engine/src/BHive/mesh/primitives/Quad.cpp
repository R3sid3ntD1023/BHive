#include "Quad.h"

namespace BHive
{
	PQuad::PQuad()
		: StaticMesh(GetMeshData())
	{
	}
	FMeshData PQuad::GetMeshData() const
	{
		float w = 1.f;
		float h = 1.f;
		const auto normal = glm::vec3(0, 1, 0);

		std::vector<FVertex> vertices = {
			FVertex{.Position = {-w, -h, 0}, .TexCoord = {0, 0}, .Normal = normal},
			FVertex{.Position = {w, -h, 0}, .TexCoord = {1, 0}, .Normal = normal},
			FVertex{.Position = {w, h, 0}, .TexCoord = {1, 1}, .Normal = normal},
			FVertex{.Position = {-w, h, 0}, .TexCoord = {0, 1}, .Normal = normal}};

		// CalculateTangentsAndBitTangents(vertices.data(), 4);

		FMeshData data;
		data.mIndices = {0, 1, 2, 2, 3, 0};
		data.mVertices = vertices;
		data.mSubMeshes.push_back(FSubMesh{.StartVertex = 0, .StartIndex = 0, .IndexCount = 6, .MaterialIndex = 0});
		data.mBoundingBox = {{-1, -1, 0}, {1, 1, 0}};

		return data;
	}
} // namespace BHive