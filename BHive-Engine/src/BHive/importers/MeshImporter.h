#pragma once

#include "core/Core.h"
#include "gfx/mesh/MeshData.h"
#include "gfx/animation/Skeleton.h"
#include "gfx/animation/AnimationFrames.h"

namespace BHive
{
	struct DecodedAnimation
	{
		std::string Name{""};
		float Duration{0.f};
		float TicksPerSecond{0.f};
		glm::mat4 GlobalInverseMatrix{1.0f};
		std::map<std::string, FrameData> Frames;
	};

	struct EmbeddedTexture
	{
		enum ESource
		{
			External,
			Embedded
		};

		std::string Type{};
		std::filesystem::path Path{};
		Buffer EmbeddedData;
		ESource Source;

		std::string GetName() const { return Path.filename().string(); }
	};

	struct DecodedMaterial
	{
		std::string Name;
		glm::vec4 Albedo = {.5f, .5f, .5f, 1.f};
		float Metallic = 0.f;
		float Roughness = 1.0f;
		std::vector<EmbeddedTexture> Textures;
	};

	struct DecodedMesh
	{
		FMeshData MeshData;
		Bones Bones;
		SkeletalNode BoneHeirarchy;
		std::vector<DecodedAnimation> Animations;
		std::vector<DecodedMaterial> Materials;
		std::filesystem::path Path{};
	};

	struct BHIVE_API MeshImporter
	{
		static DecodedMesh Import(const std::filesystem::path &path);
	};
} // namespace BHive