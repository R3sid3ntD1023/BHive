#include "Sphere.h"

namespace BHive
{
	PSphere::PSphere(float radius, uint32_t sectors, uint32_t stacks)
		: StaticMesh(GetMeshData(radius, sectors, stacks))
	{
	}

	// https://stackoverflow.com/questions/26116923/modern-opengl-draw-a-sphere-and-cylinder
	FMeshData PSphere::GetMeshData(float radius, uint32_t sectors, uint32_t stacks) const
	{

		std::vector<FVertex> vertices;
		std::vector<uint32_t> indices;

		float lengthInv = 1.f / radius;

		for (unsigned i = 0; i <= stacks; i++)
		{
			float V = i / (float)stacks;
			float phi = V * PI;

			for (unsigned j = 0; j <= sectors; j++)
			{
				float U = j / (float)sectors;
				float theta = U * (PI * 2);

				float x = cosf(theta) * sinf(phi);
				float y = cosf(phi);
				float z = sinf(theta) * sinf(phi);

				float s = 1.0f - ((float)j / sectors);
				float t = 1.0f - ((float)i / stacks);

				float nx = x * lengthInv;
				float ny = y * lengthInv;
				float nz = z * lengthInv;

				FVertex vertex;
				vertex.Position = glm::vec3{x, y, z} * radius;
				vertex.Normal = {nx, ny, nz};
				vertex.TexCoord = {s, t};

				vertices.push_back(vertex);
			}
		}

		for (unsigned i = 0; i < stacks * sectors + stacks; i++)
		{
			indices.push_back(i);
			indices.push_back(i + stacks + 1);
			indices.push_back(i + stacks);

			indices.push_back(i + stacks + 1);
			indices.push_back(i);
			indices.push_back(i + 1);
		}

		StaticMesh::CalculateTangentsAndBitTangents(vertices.data(), vertices.size());

		FMeshData data{};
		data.mVertices = vertices;
		data.mIndices = indices;
		data.mBoundingBox = AABB(glm::vec3{-radius}, glm::vec3{radius});

		auto &submesh = data.mSubMeshes.emplace_back();
		submesh.mStartIndex = 0;
		submesh.mStartVertex = 0;
		submesh.mIndexCount = (uint32_t)data.mIndices.size();
		submesh.mVertexCount = (uint32_t)data.mVertices.size();
		submesh.mMaterialIndex = 0;

		return data;
	}
} // namespace BHive