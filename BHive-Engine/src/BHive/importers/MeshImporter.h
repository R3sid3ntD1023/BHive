#pragma once

#include "core/Core.h"
#include "gfx/animation/AnimationFrames.h"
#include "gfx/animation/Skeleton.h"
#include "gfx/material/MaterialEnumerations.h"
#include "gfx/mesh/MeshData.h"

namespace BHive
{
	struct DecodedAnimation
	{
		std::string Name{""};
		float Duration{0.f};
		float TicksPerSecond{0.f};
		std::unordered_map<uint64_t, FrameData> Frames;
	};

	struct EmbeddedTexture
	{
		enum ESource
		{
			External,
			Embedded
		};

		EMaterialTextureType Type{};
		ByteBuffer EmbeddedData;
		ESource Source;
		std::filesystem::path Path{};

		std::string GetName() const { return Path.filename().string(); }
	};

	struct DecodedMaterial
	{
		std::string Name;
		glm::vec4 Albedo = {.5f, .5f, .5f, 1.f};
		float Metallic = 0.f;
		float Roughness = 1.0f;
		EMaterialShadingMode ShadingMode;
		std::vector<EmbeddedTexture> Textures;
	};

	struct DecodedMesh
	{
		FMeshData MeshData;
		BoneInfo Bones;
		SkeletalNode BoneHeirarchy;
		std::vector<DecodedAnimation> Animations;
		std::vector<DecodedMaterial> Materials;
		std::filesystem::path Path{};
	};

	struct BHIVE_API MeshImporter
	{
		static DecodedMesh Import(const std::filesystem::path &path, float importScale = 1.0f);
	};
} // namespace BHive