#pragma once

#include "core/core.h"
#include "MeshImporter.h"
#include "gfx/material/MaterialTable.h"
#include "gfx/registries/Handles.h"

namespace BHive
{
	class Asset;
	class Texture;
	class Material;
	class Skeleton;

	enum class EMeshType : uint8_t
	{
		StaticMesh,
		SkeletalMesh,
		SkeletalAnimation
	};

	struct FMeshImportOptions
	{
		std::filesystem::path AssetPath;
		EMeshType MeshType = EMeshType::StaticMesh;
		bool ImportMaterials = true;
		bool ImportAnimations = false;

		// skeleton to use if skeletal mesh
		SkeletonPtr Skeleton;

		// Materials to override if ImportMaterials is false
		MaterialTable OverideMaterials;
	};

	class BHIVE_API MeshImportResolver
	{
	public:
		using AdditionalAssets = std::vector<ResourceHandle>;

	public:
		MeshImportResolver(const FMeshImportOptions &options);

		MeshPtr Resolve(const DecodedMesh &decodedMesh);

		const AdditionalAssets &GetAdditonalAssets() const { return mAdditionalAssets; }

	private:
		void ResolveAnimations(const std::vector<DecodedAnimation> &animations);

		void ResolveMaterials(const std::vector<DecodedMaterial> &materials, MaterialTable &material_table);

	private:
		FMeshImportOptions mOptions;
		AdditionalAssets mAdditionalAssets;
	};

} // namespace BHive