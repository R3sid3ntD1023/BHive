#include "Terrain.h"
#include "gfx/Texture.h"

namespace BHive
{
	void Terrain::Init(const FTerrainParameters &params)
	{
		mParams = params;
		mSource = TextureImporter::FromFile(params.Source);
	}

	float Terrain::Slope(float x, float z) const
	{
		auto n = Normal(x, z);
		auto ndotu = glm::dot(n, glm::vec3(0.0f, 1.0f, 0.0f));
		return glm::degrees(glm::acos(ndotu));
	}

	float Terrain::Height(float x, float z) const
	{
		float u = (x + mParams.WorldHalfSize) / (mParams.WorldHalfSize * 2);
		float v = (z + mParams.WorldHalfSize) / (mParams.WorldHalfSize * 2);

		u = std::clamp(u, 0.0f, 1.0f);
		v = std::clamp(v, 0.0f, 1.0f);

		if (!mSource.Data)
			return 0.0f;

		auto s = mSource.Size;

		const int ix = static_cast<int>(u * (s.x - 1));
		const int iz = static_cast<int>(v * (s.y - 1));
		if (ix < 0 || ix >= s.x || iz < 0 || iz >= s.y)
			return 0.0f;

		const int index = (iz * s.x + ix) * 4;
		float pixel = (static_cast<float>(mSource.Data[index])) / 255.0f;
		return (pixel - 0.5f) * 2.0f * mParams.Scale;
	}

	glm::vec3 Terrain::Normal(float x, float z) const
	{
		float hL = Height(x - 0.5f, z);
		float hR = Height(x + 0.5f, z);
		float hD = Height(x, z - 0.5f);
		float hU = Height(x, z + 0.5f);
		const float dhdx = hR - hL;
		const float dhdz = hU - hD;
		return glm::normalize(glm::vec3(-dhdx, 1.0f, -dhdz));
	}

	bool Terrain::IsWalkable(float x, float z) const
	{
		// auto n = Normal(x, z);
		// auto h = Height(x, z);
		auto slope = Slope(x, z);
		bool slopeOk = slope <= mParams.MaxSlope;

		return slopeOk;
	}

	FMeshData Terrain::BuildMesh()
	{
		FMeshData m;

		auto res = mParams.Resolution;
		const float step = 2.0f * mParams.WorldHalfSize / static_cast<float>(res);
		for (int i = 0; i <= res; ++i)
		{
			for (int j = 0; j <= res; ++j)
			{
				const float x = -mParams.WorldHalfSize + static_cast<float>(j) * step;
				const float z = -mParams.WorldHalfSize + static_cast<float>(i) * step;
				const float dhdx = (Height(x + 0.5f, z) - Height(x - 0.5f, z));
				const float dhdz = (Height(x, z + 0.5f) - Height(x, z - 0.5f));
				const glm::vec3 n = glm::normalize(glm::vec3(-dhdx, 1.0f, -dhdz));
				const glm::vec3 t = glm::normalize(glm::vec3(1.0f, dhdx, 0.0f));

				FVertex v;
				v.Normal = n;
				v.Position = {x, Height(x, z), z};
				v.TexCoord = {x / 8.0f, z / 8.0f};
				v.Tangent = t;

				m.Vertices.push_back(v);
			}
		}
		const uint32_t stride = static_cast<uint32_t>(res + 1);
		for (int i = 0; i < res; ++i)
		{
			for (int j = 0; j < res; ++j)
			{
				const uint32_t a = static_cast<uint32_t>(i) * stride + static_cast<uint32_t>(j);
				const uint32_t b = a + stride;
				m.Indices.insert(m.Indices.end(), {a, b, b + 1, a, b + 1, a + 1});
			}
		}

		auto &s = m.SubMeshes.emplace_back();
		s.MaterialIndex = 0;
		s.IndexCount = m.Indices.size();
		s.StartIndex = 0;
		s.StartVertex = 0;
		s.Bounds = AABB{glm::vec3{-mParams.WorldHalfSize}, glm::vec3{mParams.WorldHalfSize}};
		return m;
	}

	REFLECT(FTerrainParameters)
	{
		BEGIN_REFLECT(FTerrainParameters)
		REFLECT_PROPERTY(Source)
		(META_DATA(EPathMetaData_AsFiles, true), META_DATA(EPathMetaData_Extensions, "(png)\0*.png\0(jpg)\0*.jpg\0")) REFLECT_PROPERTY(MaxSlope) REFLECT_PROPERTY(WorldHalfSize)
			REFLECT_PROPERTY(Scale) REFLECT_PROPERTY(Resolution);
	}
} // namespace BHive