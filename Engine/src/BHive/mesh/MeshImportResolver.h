#pragma once

#include "core/core.h"
#include "mesh/MeshImportData.h"
#include "material/MaterialTable.h"

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
		Ref<Skeleton> Skeleton;

		// Materials to override if ImportMaterials is false
		std::vector<Ref<Material>> OverideMaterials;
	};

	class MeshImportResolver
	{
	public:
		using LoadTextureSigniture = std::function<Ref<Texture>(const std::filesystem::path &)>;
		using LoadTextureMemorySigniture = std::function<Ref<Texture>(const uint8_t *data, size_t size)>;
		using CreateMaterialSigniture = std::function<Ref<Material>()>;
		using AdditionalAssets = std::vector<Ref<Asset>>;

	public:
		MeshImportResolver(
			const FMeshImportData &data, const FMeshImportOptions &options, AdditionalAssets &additional_assets);

		Ref<Asset> Resolve();

		void SetLoaders(
			typename LoadTextureSigniture load_texture_func, typename LoadTextureMemorySigniture load_texture_memory_func,
			typename CreateMaterialSigniture create_material_func);

	private:
		Ref<Asset> ResolveAnimations();

		void ResolveMaterials(MaterialTable &material_table);

	private:
		const FMeshImportData &mData;
		const FMeshImportOptions &mOptions;

		AdditionalAssets &mAdditionalAssets;
		std::string mAssetName;
		Ref<Skeleton> mSkeleton; // Used for skeletal meshes

		LoadTextureSigniture LoadTextureFunc;
		LoadTextureMemorySigniture LoadTextureMemoryFunc;
		CreateMaterialSigniture CreateMaterialFunc;
	};

} // namespace BHive