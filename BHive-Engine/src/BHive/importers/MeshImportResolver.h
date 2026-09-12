#pragma once

#include "MeshImporter.h"
#include "core/core.h"
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
		MaterialTable OverrideMaterials;
	};

	class BHIVE_API MeshImportResolver
	{
	public:
		using AdditionalAssets = std::vector<ResourceHandle>;

		struct Result
		{
			MeshPtr Mesh;
			MaterialTable Materials;
			SkeletonPtr Skeleton;
			std::vector<SkeletalAnimationPtr> Animations;
		};

	public:
		MeshImportResolver(const FMeshImportOptions &options);

		Result Resolve(const DecodedMesh &decodedMesh);

		void SetOptions(const FMeshImportOptions &options) { mOptions = options; };

		const AdditionalAssets &GetAdditonalAssets() const { return mAdditionalAssets; }

	private:
		std::vector<SkeletalAnimationPtr> ResolveAnimations(const std::vector<DecodedAnimation> &animations);

		void ResolveMaterials(const std::vector<DecodedMaterial> &materials, MaterialTable &material_table, const std::filesystem::path &assetPath);

		MaterialPtr GetMaterialFromShadingMode(EMaterialShadingMode mode);

	private:
		FMeshImportOptions mOptions;
		AdditionalAssets mAdditionalAssets;
	};

} // namespace BHive