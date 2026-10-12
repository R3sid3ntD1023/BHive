#include "MeshImportResolver.h"
#include "TextureImporter.h"
#include "gfx/Texture.h"
#include "gfx/animation/SkeletalAnimation.h"
#include "gfx/animation/Skeleton.h"
#include "gfx/factories/TextureFactory.h"
#include "gfx/material/Materials.h"
#include "gfx/mesh/SkeletalMesh.h"
#include "gfx/mesh/StaticMesh.h"

namespace BHive
{
	struct TextureResolver
	{
		Ref<Texture2D> Resolve(const EmbeddedTexture &texture, const std::filesystem::path &parent_path)
		{

			auto name = texture.Path.filename().string();
			auto hash = std::hash<std::string>()(name);

			if (!mLoadedTextures.contains(hash))
			{
				DecodedTexture decodedTex{};
				if (texture.Source == EmbeddedTexture::External)
				{
					decodedTex = TextureImporter::FromFile(parent_path / texture.Path);
				}
				else
				{
					decodedTex = TextureImporter::LoadFromMemory(texture.EmbeddedData, texture.EmbeddedData.GetSize());
				}

				mLoadedTextures[hash] = TextureFactory::Create2D(decodedTex.Size, decodedTex.CreateInfo, decodedTex.Data);
			}

			return mLoadedTextures.at(hash);
		}

	private:
		std::unordered_map<uint64_t, Ref<Texture2D>> mLoadedTextures;
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

		switch (mOptions.MeshType)
		{
		case EMeshType::StaticMesh:
		{
			result.Mesh = CreateRef<StaticMesh>(decodedMesh.MeshData);
			break;
		}
		case EMeshType::SkeletalMesh:
		{
			if (!skeleton)
			{
				skeleton = CreateRef<Skeleton>(decodedMesh.Bones, decodedMesh.BoneHeirarchy);
				result.Skeleton = skeleton;
			}

			if (mOptions.ImportAnimations)
			{
				result.Animations = ResolveAnimations(decodedMesh.Animations);
			}

			result.Mesh = CreateRef<SkeletalMesh>(decodedMesh.MeshData, skeleton.get());
			break;
		}
		case EMeshType::SkeletalAnimation:
		{
			if (decodedMesh.Animations.empty())
				break;

			if (!skeleton)
			{
				skeleton = CreateRef<Skeleton>(decodedMesh.Bones, decodedMesh.BoneHeirarchy);
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

	std::vector<Ref<SkeletalAnimation>> MeshImportResolver::ResolveAnimations(const std::vector<DecodedAnimation> &animations)
	{
		std::vector<Ref<SkeletalAnimation>> resolvedAnimations;
		for (auto &decoded : animations)
		{
			auto anim = CreateRef<SkeletalAnimation>(decoded.Duration, decoded.TicksPerSecond, decoded.Frames);
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
			AssetHandle<Material> overrideHandle = mOptions.OverrideMaterials.Get(i);
			if (overrideHandle)
			{
				material_table.Set((uint32_t)i, overrideHandle);
				continue;
			}

			const auto &materialData = materials[i];
			const auto &textures = materialData.Textures;
			const auto num_textures = textures.size();

			AssetHandle<Material> materialHandle = GetMaterialFromShadingMode(materialData.ShadingMode);

			for (size_t texIdx = 0; texIdx < num_textures; texIdx++)
			{
				auto &textureData = textures[texIdx];

				auto textureHandle = resolver.Resolve(textureData, assetPath.parent_path());

				if (textureHandle)
				{
					materialHandle->SetTextureFromType(textureData.Type, textureHandle);
				}
			}

			material_table.Set((uint32_t)i, materialHandle);
		}
	}

	Ref<Material> MeshImportResolver::GetMaterialFromShadingMode(EMaterialShadingMode mode)
	{
		switch (mode)
		{
		case EMaterialShadingMode::Standard:
			return CreateRef<StandardMaterial>();
		default:
			return CreateRef<LambertMaterial>();
		}
	}

} // namespace BHive