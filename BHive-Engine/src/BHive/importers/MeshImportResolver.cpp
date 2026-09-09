#include "MeshImportResolver.h"
#include "TextureImporter.h"
#include "gfx/animation/SkeletalAnimation.h"
#include "gfx/animation/Skeleton.h"
#include "gfx/factories/MaterialFactory.h"
#include "gfx/factories/MeshFactory.h"
#include "gfx/factories/TextureFactory.h"
#include "gfx/mesh/SkeletalMesh.h"
#include "gfx/mesh/StaticMesh.h"

namespace BHive
{
	struct TextureResolver
	{
		DecodedTexture Resolve(const EmbeddedTexture &texture, const std::filesystem::path &parent_path)
		{
			auto name = texture.Path.filename().string();
			auto hash = std::hash<std::string>()(name);

			if (!mLoadedTextures.contains(hash))
			{
				if (texture.Source == EmbeddedTexture::External)
				{
					return TextureLoader::FromFile(parent_path / texture.Path);
				}
				else
				{
					return TextureLoader::LoadFromMemory(texture.EmbeddedData, texture.EmbeddedData.GetSize());
				}

				mLoadedTextures.insert(hash);
			}
		}

	private:
		std::set<uint64_t> mLoadedTextures;
	};

	MeshImportResolver::MeshImportResolver(const FMeshImportOptions &options)
		: mOptions(options)
	{
	}

	MeshImportResolver::Result MeshImportResolver::Resolve(const DecodedMesh &decodedMesh)
	{
		MeshPtr asset{};

		auto name = mOptions.AssetPath.stem().string();
		auto skeleton = mOptions.Skeleton;

		LOG_TRACE("SubMesh count {}", decodedMesh.MeshData.SubMeshes.size());

		switch (mOptions.MeshType)
		{
		case EMeshType::StaticMesh:
		{
			asset = MeshFactory::CreateStatic(decodedMesh.MeshData);
			break;
		}
		case EMeshType::SkeletalMesh:
		{
			if (!skeleton)
			{
				skeleton = SkeletonFactory::Create(decodedMesh.Bones, decodedMesh.BoneHeirarchy);
				mAdditionalAssets.push_back(skeleton);
			}

			if (mOptions.ImportAnimations)
			{
				ResolveAnimations(decodedMesh.Animations);
			}

			asset = MeshFactory::CreateSkeletal(decodedMesh.MeshData, skeleton);
			break;
		}
		case EMeshType::SkeletalAnimation:
		{
			if (!decodedMesh.Animations.empty())
			{
				if (!skeleton)
				{
					skeleton = SkeletonFactory::Create(decodedMesh.Bones, decodedMesh.BoneHeirarchy);
					mAdditionalAssets.push_back(skeleton);
				}

				ResolveAnimations(decodedMesh.Animations);
			}

			break;
		}
		}

		MaterialTable materials;
		materials.Resize(decodedMesh.MeshData.MaterialCount);

		if (mOptions.ImportMaterials)
		{

			ResolveMaterials(decodedMesh.Materials, materials, decodedMesh.Path);
		}

		return {asset, materials};
	}

	void MeshImportResolver::ResolveAnimations(const std::vector<DecodedAnimation> &animations)
	{
		for (auto &decoded : animations)
		{
			auto anim = SkeletalAnimationFactory::Create(decoded.Duration, decoded.TicksPerSecond, decoded.Frames, decoded.GlobalInverseMatrix);
			mAdditionalAssets.emplace_back(anim);
		}
	}

	void MeshImportResolver::ResolveMaterials(const std::vector<DecodedMaterial> &materials, MaterialTable &material_table, const std::filesystem::path &assetPath)
	{
		TextureResolver resolver{};

		size_t num_materials = materials.size();

		for (size_t i = 0; i < num_materials; i++)
		{
			MaterialPtr overrideHandle = mOptions.OverrideMaterials.Get(i);
			if (overrideHandle)
			{
				material_table.Set(overrideHandle, i);
				continue;
			}

			auto materialHandle = MaterialFactory::CreateLambert();
			auto material = materialHandle.As<Material>();

			const auto &material_data = materials[i];
			const auto &textures = material_data.Textures;
			const auto num_textures = textures.size();

			for (size_t texIdx = 0; texIdx < num_textures; texIdx++)
			{
				auto &texture = textures[texIdx];

				auto decoded = resolver.Resolve(texture, assetPath.parent_path());
				auto handle = TextureFactory::Create2D(decoded);

				if (handle)
				{
					mAdditionalAssets.push_back(handle);
				}
			}

			material_table.Set(materialHandle, (uint32_t)i);
		}
	}

} // namespace BHive