#pragma once

#include "core/Core.h"
#include "gfx/mesh/MeshData.h"
#include "importers/TextureImporter.h"

namespace BHive
{
	class Texture2D;

	struct FTerrainParameters
	{
		std::filesystem::path Source;
		float MaxSlope = 75.0f;
		float WorldHalfSize = 500.0f;
		float Scale = 1.0f;
		uint32_t Resolution = 100u;

		template <typename Ar>
		void Serialize(Ar &ar)
		{
			ar(Source, MaxSlope, WorldHalfSize, Scale);
		}

		REFLECTABLE()
	};

	REFLECT_EXTERN(FTerrainParameters)

	class BHIVE_API Terrain
	{
	private:
		/* data */
	public:
		void Init(const FTerrainParameters &params);

		float Slope(float x, float z) const;

		float Height(float x, float z) const;

		glm::vec3 Normal(float x, float z) const;

		bool IsWalkable(float x, float z) const;

		FMeshData BuildMesh();

	private:
		DecodedTexture mSource;
		FTerrainParameters mParams;
	};
} // namespace BHive