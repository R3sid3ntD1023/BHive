#include "gfx/animation/SkeletalAnimation.h"
#include "gfx/mesh/SkeletalMesh.h"
#include "gfx/animation/Skeleton.h"
#include "gfx/mesh/StaticMesh.h"
#include "MeshImportResolver.h"
#include "gfx/factories/MaterialFactory.h"
#include "gfx/factories/MeshFactory.h"
#include "gfx/factories/TextureFactory.h"
#include "TextureImporter.h"

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
				if (!texture.Source == EmbeddedTexture::External)
				{
					return TextureLoader::FromFile(parent_path / texture.Path);
				}
				else
				{
					return TextureLoader::LoadFromMemory(texture.EmbeddedData, texture.EmbeddedData.GetSize());
				}
			}
		}

	private:
		std::set<uint64_t> mLoadedTextures;
	};

	MeshImportResolver::MeshImportResolver(const FMeshImportOptions &options)
		: mOptions(options)
	{
	}

	MeshPtr MeshImportResolver::Resolve(const DecodedMesh &decodedMesh)
	{
		MeshPtr asset{};

		auto name = mOptions.AssetPath.stem().string();
		auto skeleton = mOptions.Skeleton;

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

		if (auto mesh = asset.As<BaseMesh>(); mesh && mOptions.ImportMaterials)
		{
			ResolveMaterials(decodedMesh.Materials, mesh->GetMaterialTable());
		}

		return asset;
	}

	void MeshImportResolver::ResolveAnimations(const std::vector<DecodedAnimation> &animations)
	{
		for (auto &decoded : animations)
		{
			auto anim = SkeletalAnimationFactory::Create(decoded.Duration, decoded.TicksPerSecond, decoded.Frames, decoded.GlobalInverseMatrix);
			mAdditionalAssets.emplace_back(anim);
		}
	}

	void MeshImportResolver::ResolveMaterials(const std::vector<DecodedMaterial> &materials, MaterialTable &material_table)
	{
		TextureResolver resolver{};

		size_t num_materials = materials.size();

		material_table.Resize(num_materials);

		for (size_t i = 0; i < num_materials; i++)
		{
			MaterialPtr overrideHandle = mOptions.OverideMaterials.Get(i);
			auto materialHandle = overrideHandle ? overrideHandle : material_table[i];

			if (!materialHandle)
			{
				materialHandle = MaterialFactory::CreateLambert();
				auto material = materialHandle.As<Material>();

				const auto &material_data = materials[i];
				const auto &textures = material_data.Textures;
				const auto num_textures = textures.size();

				for (size_t texIdx = 0; texIdx < num_textures; texIdx++)
				{
					auto &texture = textures[texIdx];

					auto decoded = resolver.Resolve(texture, mOptions.AssetPath.parent_path());
					auto handle = TextureFactory::Create2D(decoded);

					if (handle)
					{
						mAdditionalAssets.push_back(handle);
					}
				}
			}

			material_table.Set(materialHandle, (uint32_t)i);
		}
	}

} // namespace BHive