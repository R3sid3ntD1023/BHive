#include "gfx/VertexArray.h"
#include "StaticMesh.h"

namespace BHive
{
	StaticMesh::StaticMesh(const FMeshData &data)
		: IRenderableAsset(data)
	{
	}

	Ref<StaticMesh> StaticMesh::CreatePlane(float width, float height)
	{
		width *= .5f;
		height *= .5f;

		std::vector<FVertex> points = {
			{.Position = {-width, -height, 0}, .TexCoord = {0, 0}, .Normal = {0, 0, 1}},
			{.Position = {width, -height, 0}, .TexCoord = {1, 0}, .Normal = {0, 0, 1}},
			{.Position = {width, height, 0}, .TexCoord = {1, 1}, .Normal = {0, 0, 1}},
			{.Position = {-width, height, 0}, .TexCoord = {0, 1}, .Normal = {0, 0, 1}}};

		StaticMesh::CalculateTangentsAndBitTangents(points);

		std::vector<uint32_t> indces = {0, 1, 2, 2, 3, 0};

		FMeshData data{};
		data.mVertices = points;
		data.mIndices = indces;
		data.mBoundingBox = AABB(points[0].Position, points[2].Position);

		auto &submesh = data.mSubMeshes.emplace_back();
		submesh.mStartIndex = 0;
		submesh.mStartVertex = 0;
		submesh.mIndexCount = (uint32_t)data.mIndices.size();
		submesh.mVertexCount = (uint32_t)data.mVertices.size();
		submesh.mMaterialIndex = 0;

		return CreateRef<StaticMesh>(data);
	}

	Ref<StaticMesh> StaticMesh::CreateCube(float size)
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

		StaticMesh::CalculateTangentsAndBitTangents(vertices);

		std::vector<uint32_t> indces = {
			0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4, 8, 9, 10, 10, 11, 8, 12, 13, 14, 14, 15, 12, 16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20,
		};

		FMeshData data{};
		data.mVertices = vertices;
		data.mIndices = indces;
		data.mBoundingBox = AABB(vertices[7].Position, vertices[2].Position);

		auto &submesh = data.mSubMeshes.emplace_back();
		submesh.mStartIndex = 0;
		submesh.mStartVertex = 0;
		submesh.mIndexCount = (uint32_t)data.mIndices.size();
		submesh.mVertexCount = (uint32_t)data.mVertices.size();
		submesh.mMaterialIndex = 0;

		return CreateRef<StaticMesh>(data);
	}

	// https://songho.ca/opengl/gl_sphere.html
	Ref<StaticMesh> StaticMesh::CreateSphere(float radius, uint32_t sectors, uint32_t stacks)
	{
		std::vector<FVertex> vertices;
		std::vector<uint32_t> indices;

		float sectorStep = 2 * PI / sectors;
		float stackStep = PI / stacks;

		float x, y, z, xz;
		float nx, ny, nz;
		float s, t;
		float stackAngle, sectorAngle;
		float lengthInv = 1.0f / radius;

		for (unsigned i = 0; i <= stacks; i++)
		{
			stackAngle = PI / 2 - i * stackStep;
			xz = radius * cosf(stackAngle);
			y = radius * sinf(stackAngle);

			for (unsigned j = 0; j <= sectors; j++)
			{
				sectorAngle = j * sectorStep;

				x = xz * cosf(sectorAngle);
				z = xz * sinf(sectorAngle);

				s = (float)j / sectors;
				t = (float)i / stacks;

				nx = x * lengthInv;
				ny = y * lengthInv;
				nz = z * lengthInv;

				FVertex vertex;
				vertex.Position = {x, y, z};
				vertex.Normal = {nx, ny, nz};
				vertex.TexCoord = {s, t};

				vertices.push_back(vertex);
			}
		}

		StaticMesh::CalculateTangentsAndBitTangents(vertices);

		unsigned k1, k2;
		for (unsigned i = 0; i < stacks; i++)
		{
			k1 = i * (sectors + 1);
			k2 = k1 + sectors + 1;

			for (unsigned j = 0; j < stacks; j++, k1++, k2++)
			{
				if (i != 0)
				{
					indices.push_back(k1 + 1);
					indices.push_back(k2);
					indices.push_back(k1);
				}

				if (i != (stacks - 1))
				{
					indices.push_back(k2 + 1);
					indices.push_back(k2);
					indices.push_back(k1 + 1);
				}
			}
		}

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

		return CreateRef<StaticMesh>(data);
	}

	void StaticMesh::CalculateTangentsAndBitTangents(std::vector<FVertex> &vertices)
	{
		auto count = vertices.size();
		for (size_t i = 0; i < count; i++)
		{
			auto &vertex = vertices[i];

			auto up = glm::vec3(0, 1, 0);
			auto normal = vertices[i].Normal;

			auto tangent = glm::cross(normal, up);
			auto bitangent = glm::cross(tangent, normal);
			tangent = glm::cross(normal, bitangent);

			vertex.BiNormal = bitangent;
			vertex.Tangent = tangent;
		}
	}

	void StaticMesh::Save(cereal::BinaryOutputArchive &ar) const
	{
		IRenderableAsset::Save(ar);
	}

	void StaticMesh::Load(cereal::BinaryInputArchive &ar)
	{
		IRenderableAsset::Load(ar);
	}

	REFLECT(StaticMesh)
	{
		BEGIN_REFLECT(StaticMesh)
		REFLECT_CONSTRUCTOR();
	}
} // namespace BHive