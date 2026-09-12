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
		TexturePtr Resolve(const EmbeddedTexture &texture, const std::filesystem::path &parent_path)
		{

			auto name = texture.Path.filename().string();
			auto hash = std::hash<std::string>()(name);

			if (!mLoadedTextures.contains(hash))
			{
				DecodedTexture decodedTex{};
				if (texture.Source == EmbeddedTexture::External)
				{
					decodedTex = TextureLoader::FromFile(parent_path / texture.Path);
				}
				else
				{
					decodedTex = TextureLoader::LoadFromMemory(texture.EmbeddedData, texture.EmbeddedData.GetSize());
				}

				mLoadedTextures[hash] = TextureFactory::Create2D(decodedTex);
			}

			return mLoadedTextures.at(hash);
		}

	private:
		std::unordered_map<uint64_t, TexturePtr> mLoadedTextures;
	};

	MeshImportResolver::MeshImportResolver(const FMeshImportOptions &options)
		: mOptions(options)
	{
	}

	MeshImportResolver::Result MeshImportResolver::Resolve(const DecodedMesh &decodedMesh)
	{
		MeshImportResolver::Result result{};

		auto name = mOptions.AssetPath.stem().string();
		auto skeleton = mOptions.Skeleton;

		LOG_TRACE("SubMesh count {}", decodedMesh.MeshData.SubMeshes.size());

		switch (mOptions.MeshType)
		{
		case EMeshType::StaticMesh:
		{
			result.Mesh = MeshFactory::CreateStatic(decodedMesh.MeshData);
			break;
		}
		case EMeshType::SkeletalMesh:
		{
			if (!skeleton)
			{
				skeleton = SkeletonFactory::Create(decodedMesh.Bones, decodedMesh.BoneHeirarchy);
				result.Skeleton = skeleton;
			}

			if (mOptions.ImportAnimations)
			{
				result.Animations = ResolveAnimations(decodedMesh.Animations);
			}

			result.Mesh = MeshFactory::CreateSkeletal(decodedMesh.MeshData, skeleton);
			break;
		}
		case EMeshType::SkeletalAnimation:
		{
			if (decodedMesh.Animations.empty())
				break;

			if (!skeleton)
			{
				skeleton = SkeletonFactory::Create(decodedMesh.Bones, decodedMesh.BoneHeirarchy);
				result.Skeleton = skeleton;
			}

			result.Animations = ResolveAnimations(decodedMesh.Animations);

			break;
		}
		}

		result.Materials.Resize(decodedMesh.MeshData.MaterialCount);

		if (mOptions.ImportMaterials)
		{

			ResolveMaterials(decodedMesh.Materials, result.Materials, decodedMesh.Path);
		}

		return result;
	}

	std::vector<SkeletalAnimationPtr> MeshImportResolver::ResolveAnimations(const std::vector<DecodedAnimation> &animations)
	{
		std::vector<SkeletalAnimationPtr> resolvedAnimations;
		for (auto &decoded : animations)
		{
			auto anim = SkeletalAnimationFactory::Create(decoded);
			resolvedAnimations.emplace_back(anim);
		}

		return resolvedAnimations;
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

			const auto &materialData = materials[i];
			const auto &textures = materialData.Textures;
			const auto num_textures = textures.size();

			auto materialHandle = GetMaterialFromShadingMode(materialData.ShadingMode);

			for (size_t texIdx = 0; texIdx < num_textures; texIdx++)
			{
				auto &textureData = textures[texIdx];

				auto textureHandle = resolver.Resolve(textureData, assetPath.parent_path());

				if (textureHandle)
				{
					materialHandle.As<Material>()->SetTextureFromType(textureData.Type, textureHandle);
				}
			}

			material_table.Set(materialHandle, (uint32_t)i);
		}
	}

	MaterialPtr MeshImportResolver::GetMaterialFromShadingMode(EMaterialShadingMode mode)
	{
		switch (mode)
		{
		case EMaterialShadingMode::Standard:
			return MaterialFactory::CreateStandard();
		default:
			return MaterialFactory::CreateLambert();
		}
	}

} // namespace BHive