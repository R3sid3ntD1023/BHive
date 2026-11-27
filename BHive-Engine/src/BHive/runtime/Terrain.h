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

	struct FShoreParameters
	{
		int32_t Size = 1024;
		glm::vec2 Center = {0, 0};
		float Extent = 1.f;

		template <typename Ar>
		void Serialize(Ar &ar)
		{
			ar(Center, Extent);
		}

		bool operator==(const FShoreParameters &rhs) const { return Size == rhs.Size && Center == rhs.Center && Extent == rhs.Extent; }
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

		void GenerateShoreMap(const FShoreParameters &shoreParams);

		Ref<Texture2D> GetShoreMap() const { return mShoreMap; }

	private:
		DecodedTexture mSource;
		FTerrainParameters mParams;
		Ref<Texture2D> mShoreMap;
		FShoreParameters mShoreParameters;
	};
} // namespace BHive