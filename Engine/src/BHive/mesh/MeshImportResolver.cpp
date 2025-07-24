#include "MeshImportResolver.h"
#include "mesh/SkeletalAnimation.h"
#include "mesh/SkeletalMesh.h"
#include "mesh/Skeleton.h"
#include "mesh/StaticMesh.h"

namespace BHive
{
	struct TextureResolver
	{
		TextureResolver(LoadTextureSigniture loadTextureFunc, LoadTextureMemorySigniture loadTextureMemoryFunc)
			: mLoadTextureFunc(loadTextureFunc),
			  mLoadTextureMemoryFunc(loadTextureMemoryFunc)
		{
		}

		Ref<Texture> &Resolve(const FTextureData &data, const std::filesystem::path &parent_path)
		{

			auto &texture_asset = mLoadedTextures[data.get_name()];
			if (!texture_asset)
			{
				if (!data.is_embedded())
				{
					texture_asset = mLoadTextureFunc(parent_path / data.Path);
				}
				else
				{
					texture_asset = mLoadTextureMemoryFunc(data.EmbeddedData, data.EmbeddedData.GetSize());
				}

				texture_asset->SetName(data.get_name());
			}

			return texture_asset;
		}

	private:
		LoadTextureSigniture mLoadTextureFunc;
		LoadTextureMemorySigniture mLoadTextureMemoryFunc;

		std::unordered_map<std::string, Ref<Texture>> mLoadedTextures;
	};

	MeshImportResolver::MeshImportResolver(
		const FMeshImportData &data, const FMeshImportOptions &options, AdditionalAssets &additional)
		: mData(data),
		  mOptions(options),
		  mAdditionalAssets(additional)
	{
	}

	void MeshImportResolver::SetLoaders(
		LoadTextureSigniture load_texture, LoadTextureMemorySigniture load_texture_memory,
		CreateMaterialSigniture create_material)
	{
		LoadTextureFunc = std::move(load_texture);
		LoadTextureMemoryFunc = std::move(load_texture_memory);
		CreateMaterialFunc = std::move(create_material);
	}

	Ref<Asset> MeshImportResolver::Resolve()
	{
		Ref<Asset> asset;

		mAssetName = mOptions.AssetPath.stem().string();
		switch (mOptions.MeshType)
		{
		case EMeshType::StaticMesh:
		{
			asset = CreateRef<StaticMesh>(mData.mMeshData);

			break;
		}
		case EMeshType::SkeletalMesh:
		{
			mSkeleton = mOptions.Skeleton;
			if (!mSkeleton)
			{
				mSkeleton = CreateRef<Skeleton>(mData.mBoneData, mData.mSkeletonHeirarchyData);

				mSkeleton->SetName(mAssetName + "_Skeleton");
				mAdditionalAssets.push_back(mSkeleton);
			}

			asset = CreateRef<SkeletalMesh>(mData.mMeshData, mSkeleton);

			if (mOptions.ImportAnimations)
			{
				mAdditionalAssets.push_back(ResolveAnimations());
			}

			break;
		}
		case EMeshType::SkeletalAnimation:
		{
			if (mData.mAnimationData.empty())
			{
				return nullptr; // No animations to import
			}
			mSkeleton = mOptions.Skeleton;
			if (!mSkeleton)
			{
				mSkeleton = CreateRef<Skeleton>(mData.mBoneData, mData.mSkeletonHeirarchyData);
				mSkeleton->SetName(mAssetName + "_Skeleton");
				mAdditionalAssets.push_back(mSkeleton);
			}
			asset = ResolveAnimations();
			break;
		}
		}

		if (auto mesh = Cast<BaseMesh>(asset); mesh && mOptions.ImportMaterials)
		{
			ResolveMaterials(mesh->GetMaterialTable());
		}

		asset->SetName(mAssetName);

		return asset;
	}

	Ref<Asset> MeshImportResolver::ResolveAnimations()
	{
		size_t num_animations = mData.mAnimationData.size();
		std::vector<Ref<SkeletalAnimation>> animations(num_animations);

		for (size_t i = 0; i < num_animations; i++)
		{
			auto &data = mData.mAnimationData[i];

			std::string anim_name = std::format("{}_Animation({})", mAssetName, i);

			animations[i] = CreateRef<SkeletalAnimation>(
				data.mDuration, data.TicksPerSecond, data.mFrames, mSkeleton, data.mGlobalInverseMatrix);
			animations[i]->SetName(anim_name);
		}

		if (num_animations > 1)
		{
			for (size_t i = 1; i < animations.size(); i++)
			{
				mAdditionalAssets.push_back(animations[i]);
			}
		}

		return animations[0];
	}

	void MeshImportResolver::ResolveMaterials(MaterialTable &material_table)
	{
		ASSERT(LoadTextureFunc);
		ASSERT(LoadTextureMemoryFunc);
		ASSERT(CreateMaterialFunc);

		size_t num_materials = mData.mMaterialData.size();

		material_table.resize(num_materials);

		for (size_t i = 0; i < num_materials; i++)
		{
			auto overide_material = i < mOptions.OverideMaterials.size() ? mOptions.OverideMaterials[i] : nullptr;
			auto material = overide_material ? overide_material : material_table[i];

			if (!material)
			{
				material = Cast<Material>(CreateMaterialFunc());

				const auto &material_data = mData.mMaterialData[i];
				const auto &textures = material_data.mTextureData;
				const auto num_textures = textures.size();

				for (size_t i = 0; i < num_textures; i++)
				{
					auto &texture_data = textures[i];

					TextureResolver texture_resolver(LoadTextureFunc, LoadTextureMemoryFunc);
					auto &texture_asset = texture_resolver.Resolve(texture_data, mOptions.AssetPath.parent_path());

					if (texture_asset)
					{
						mAdditionalAssets.push_back(texture_asset);
						material->SetTexture(texture_data.Type.c_str(), texture_asset);
					}
				}

				auto material_name =
					material_data.mName.empty() ? std::format("{}({})", mAssetName, i) : material_data.mName;

				material->SetName(material_name);

				mAdditionalAssets.push_back(material);
			}

			material_table.set_material(material, i);
		}
	}

} // namespace BHive