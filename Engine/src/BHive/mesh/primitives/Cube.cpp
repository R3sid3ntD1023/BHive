#include "Cube.h"

namespace BHive
{
	PCube::PCube(float size)
		: StaticMesh(GetMeshData(size))
	{
	}

	FMeshData PCube::GetMeshData(float size)
	{
		const float s = size * .5f;
		std::vector<FVertex> vertices = {
			// front
			{.Position = {-s, -s, s}, .TexCoord = {0, 0}, .Normal = {0, 0, 1}, .Color = {1, 0, 0, 1}},
			{.Position = {s, -s, s}, .TexCoord = {1, 0}, .Normal = {0, 0, 1}, .Color = {1, 0, 0, 1}},
			{.Position = {s, s, s}, .TexCoord = {1, 1}, .Normal = {0, 0, 1}, .Color = {1, 0, 0, 1}},
			{.Position = {-s, s, s}, .TexCoord = {0, 1}, .Normal = {0, 0, 1}, .Color = {1, 0, 0, 1}},

			// back
			{.Position = {-s, s, -s}, .TexCoord = {0, 0}, .Normal = {0, 0, -1}, .Color = {1, 1, 0, 1}},
			{.Position = {s, s, -s}, .TexCoord = {1, 0}, .Normal = {0, 0, -1}, .Color = {1, 1, 0, 1}},
			{.Position = {s, -s, -s}, .TexCoord = {1, 1}, .Normal = {0, 0, -1}, .Color = {1, 1, 0, 1}},
			{.Position = {-s, -s, -s}, .TexCoord = {0, 1}, .Normal = {0, 0, -1}, .Color = {1, 1, 0, 1}},

			// left
			{.Position = {-s, -s, -s}, .TexCoord = {0, 0}, .Normal = {-1, 0, 0}, .Color = {0, 1, 0, 1}},
			{.Position = {-s, -s, s}, .TexCoord = {1, 0}, .Normal = {-1, 0, 0}, .Color = {0, 1, 0, 1}},
			{.Position = {-s, s, s}, .TexCoord = {1, 1}, .Normal = {-1, 0, 0}, .Color = {0, 1, 0, 1}},
			{.Position = {-s, s, -s}, .TexCoord = {0, 1}, .Normal = {-1, 0, 0}, .Color = {0, 1, 0, 1}},

			// right
			{.Position = {s, -s, -s}, .TexCoord = {0, 0}, .Normal = {1, 0, 1}, .Color = {0, 1, 1, 1}},
			{.Position = {s, s, -s}, .TexCoord = {1, 0}, .Normal = {1, 0, 1}, .Color = {0, 1, 1, 1}},
			{.Position = {s, s, s}, .TexCoord = {1, 1}, .Normal = {1, 0, 1}, .Color = {0, 1, 1, 1}},
			{.Position = {s, -s, s}, .TexCoord = {0, 1}, .Normal = {1, 0, 1}, .Color = {0, 1, 1, 1}},

			// top
			{.Position = {-s, s, -s}, .TexCoord = {0, 0}, .Normal = {0, 1, 0}, .Color = {0, 0, 1, 1}},
			{.Position = {-s, s, s}, .TexCoord = {1, 0}, .Normal = {0, 1, 0}, .Color = {0, 0, 1, 1}},
			{.Position = {s, s, s}, .TexCoord = {1, 1}, .Normal = {0, 1, 0}, .Color = {0, 0, 1, 1}},
			{.Position = {s, s, -s}, .TexCoord = {0, 1}, .Normal = {0, 1, 0}, .Color = {0, 0, 1, 1}},

			// bottom
			{.Position = {s, -s, -s}, .TexCoord = {0, 0}, .Normal = {0, -1, 0}, .Color = {1, 0, 1, 1}},
			{.Position = {s, -s, s}, .TexCoord = {1, 0}, .Normal = {0, -1, 0}, .Color = {1, 0, 1, 1}},
			{.Position = {-s, -s, s}, .TexCoord = {1, 1}, .Normal = {0, -1, 0}, .Color = {1, 0, 1, 1}},
			{.Position = {-s, -s, -s}, .TexCoord = {0, 1}, .Normal = {0, -1, 0}, .Color = {1, 0, 1, 1}},
		};

		StaticMesh::CalculateTangentsAndBitTangents(vertices.data(), vertices.size());

		std::vector<uint32_t> indces = {
			0,	1,	2,	2,	3,	0,	4,	5,	6,	6,	7,	4,	8,	9,	10, 10, 11, 8,
			12, 13, 14, 14, 15, 12, 16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20,
		};

		FMeshData data{};
		data.mVertices = vertices;
		data.mIndices = indces;
		data.mBoundingBox = AABB(vertices[7].Position, vertices[2].Position);

		auto &submesh = data.mSubMeshes.emplace_back();
		submesh.StartIndex = 0;
		submesh.StartVertex = 0;
		submesh.IndexCount = (uint32_t)data.mIndices.size();
		submesh.MaterialIndex = 0;

		return data;
	}
} // namespace BHive