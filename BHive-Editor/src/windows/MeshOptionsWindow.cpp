#include "asset/AssetManager.h"
#include "asset/EditorAssetManager.h"
#include "core/FileDialog.h"
#include "factories/MaterialFactory.h"
#include "factories/MeshFactory.h"
#include "factories/TextureFactory.h"
#include "gui/ImGuiExtended.h"
#include "Inspectors.h"
#include "mesh/SkeletalAnimation.h"
#include "mesh/SkeletalMesh.h"
#include "mesh/Skeleton.h"
#include "mesh/StaticMesh.h"
#include "MeshOptionsWindow.h"
#include "material/BDRFMaterial.h"

#define ALBEDO_TEX "Albedo";
#define METALLIC_TEX "Metallic";
#define ROUGHNESS_TEX "Roughness";
#define METALLIC_ROUGHNESS_TEX "MetallicRoughness";
#define NORMAL_TEX "Normal";
#define EMISSION_TEX "Emission";
#define OPACITY_TEX "Opacity";
#define DISPLACEMENT_TEX "Displacement";

namespace BHive
{
	MeshOptionsWindow::MeshOptionsWindow(MeshFactory *factory, const FMeshImportOptions &importData)
		: WindowBase(ImGuiWindowFlags_NoSavedSettings),
		  mImportData(importData),
		  mFactory(factory)
	{
		has_bones = mImportData.mData.mBoneData.size();
		has_materials = mImportData.mData.mMaterialData.size();
		has_animations = mImportData.mData.mAnimationData.size();

		as_static_mesh = !has_bones;
		as_skeletal_mesh = has_bones;
		import_materials = has_materials;
		import_animations = has_animations && has_bones;

		mMaterialDirectory = mImportData.mPath.parent_path();
		mAnimationDirectory = mImportData.mPath.parent_path();
		mMaterials.resize(mImportData.mData.mMaterialData.size());
	}

	void MeshOptionsWindow::OnGuiRender()
	{
		ImGui::BeginDisabled(as_skeletal_mesh);
		ImGui::Checkbox("StaticMesh", &as_static_mesh);
		ImGui::EndDisabled();

		ImGui::BeginDisabled(!has_bones || as_static_mesh);
		ImGui::Checkbox("SkeletalMesh", &as_skeletal_mesh);
		ImGui::EndDisabled();

		ImGui::BeginDisabled(!has_materials);
		ImGui::Checkbox("Import Materials", &import_materials);
		ImGui::EndDisabled();

		ImGui::SeparatorText("Materials");
		ImGui::BeginDisabled(!import_materials);
		ImGui::TextUnformatted("MaterialDirectory");
		ImGui::SameLine();
		ImGui::TextUnformatted(mMaterialDirectory.string().c_str());
		ImGui::SameLine();
		if (ImGui::Button("...##Material"))
		{
			mMaterialDirectory = FileDialogs::GetDirectory();
		}
		ImGui::EndDisabled();

		for (size_t i = 0; i < mMaterials.size(); i++)
		{
			inspect(std::format("Materials", i), mMaterials[i]);
		}

		ImGui::SeparatorText("Animations");
		ImGui::BeginDisabled(!has_animations);
		ImGui::Checkbox("Import Animations", &import_animations);

		inspect("Skeleton", mSkeleton);

		ImGui::EndDisabled();

		ImGui::BeginDisabled(!import_animations);
		ImGui::TextUnformatted("AnimationDirectory");
		ImGui::SameLine();
		ImGui::TextUnformatted(mAnimationDirectory.string().c_str());
		ImGui::SameLine();
		if (ImGui::Button("...##Animation"))
		{
			mAnimationDirectory = FileDialogs::GetDirectory();
		}
		ImGui::EndDisabled();

		if (ImGui::Button("Cancel"))
		{
			mShouldClose = true;
		}

		if (ImGui::Button("Import"))
		{
			auto path = mImportData.mPath;
			CreateAssetMetaFile();
			mShouldClose = true;
		}
	}

	void MeshOptionsWindow::CreateAssetMetaFile()
	{
		auto asset_manager = AssetManager::GetAssetManager<EditorAssetManager>();
		if (!asset_manager)
		{
			return;
		}

		auto name = mImportData.mPath.stem().string();
		Ref<Asset> asset;

		if (as_static_mesh)
		{
			auto mesh = CreateRef<StaticMesh>(mImportData.mData.mMeshData);
			mesh->SetName(name);
			asset = mesh;
		}
		else if (has_animations || as_skeletal_mesh)
		{
			if (!mSkeleton)
			{
				mSkeleton = CreateRef<Skeleton>(mImportData.mData.mBoneData, mImportData.mData.mSkeletonHeirarchyData);

				mSkeleton->SetName(name + "_Skeleton");
				mFactory->mOtherAssets.push_back(mSkeleton);
			}

			if (as_skeletal_mesh)
			{
				auto mesh = CreateRef<SkeletalMesh>(mImportData.mData.mMeshData, mSkeleton);
				mesh->SetName(name);
				asset = mesh;
			}

			if (import_animations)
			{
				auto &animation_data = mImportData.mData.mAnimationData;
				size_t num_animations = animation_data.size();
				std::vector<Ref<SkeletalAnimation>> animations(num_animations);

				for (size_t i = 0; i < num_animations; i++)
				{
					auto &data = animation_data[i];

					std::string anim_name = !asset ? name : std::format("{}_Animation({})", name, i);

					animations[i] = CreateRef<SkeletalAnimation>(
						data.mDuration, data.TicksPerSecond, data.mFrames, mSkeleton, data.mGlobalInverseMatrix);
					animations[i]->SetName(anim_name);
				}

				if (!asset)
					asset = animations[0];

				for (auto &animation : animations)
				{
					if (asset != animation)
						mFactory->mOtherAssets.push_back(animation);
				}
			}
		}

		auto mesh = Cast<BaseMesh>(asset);
		if (mesh && import_materials)
		{
			TextureFactory tex_factory;
			MaterialFactory mat_factory;

			auto &material_table = mesh->GetMaterialTable();
			auto &material_data = mImportData.mData.mMaterialData;
			size_t num_materials = material_data.size();

			material_table.resize(num_materials);

			for (size_t i = 0; i < num_materials; i++)
			{
				auto material = mMaterials[i];

				if (!material)
				{
					material = Cast<Material>(mat_factory.CreateNew());

					auto &data = material_data[i];
					auto &textures = data.mTextureData;
					auto num_textures = textures.size();

					for (size_t i = 0; i < num_textures; i++)
					{
						auto &texture = textures[i];
						Ref<Asset> texture_asset;
						if (!texture.is_embedded())
						{
							texture_asset = tex_factory.Import(mImportData.mPath.parent_path() / texture.mPath);
						}
						else
						{
							texture_asset = tex_factory.Import(texture.mEmbeddedData, texture.mEmbeddedDataSize);
						}

						if (texture_asset)
						{
							texture_asset->SetName(texture.mPath.stem().string());
							mFactory->mOtherAssets.push_back(texture_asset);
							SetMaterialTexture(texture.mType, Cast<Texture>(texture_asset), material);
						}
					}

					auto material_name = data.mName.empty() ? std::format("{}({})", name, i) : data.mName;

					material->SetName(material_name);
					/*material->mAldebo = {data.mAlbedo.x, data.mAlbedo.y, data.mAlbedo.z, data.mAlbedo.w};
					material->mMetallic = data.mMetallic;
					material->mRoughness = data.mRoughness;*/

					mFactory->mOtherAssets.push_back(material);
				}

				material_table.set_material(material, i);
			}
		}

		if (asset)
			mFactory->OnImportCompleted.invoke(asset);
	}

	void
	MeshOptionsWindow::SetMaterialTexture(FTextureData::EType type, const Ref<Texture> &texture, Ref<Material> &material)
	{
		if (!texture || !material || type == FTextureData::Type_NONE)
			return;

		std::string name = "";

		switch (type)
		{
		case BHive::FTextureData::Type_ALBEDO:
			name = ALBEDO_TEX;
			break;
		case BHive::FTextureData::Type_METALLIC:
			name = METALLIC_TEX;
			break;
		case BHive::FTextureData::Type_ROUGHNESS:
			name = ROUGHNESS_TEX;
			break;
		case BHive::FTextureData::Type_METALLIC_ROUGHNESS:
			name = METALLIC_ROUGHNESS_TEX;
			break;
		case BHive::FTextureData::Type_NORMAL:
			name = NORMAL_TEX;
			break;
		case BHive::FTextureData::Type_EMISSION:
			name = EMISSION_TEX;
			break;
		case BHive::FTextureData::Type_OPACITY:
			name = OPACITY_TEX;
			break;
		case BHive::FTextureData::Type_DISPLACEMENT:
			name = DISPLACEMENT_TEX;
			break;
		default:
			break;
		}

		if (!name.empty())
			material->SetTexture(name.c_str(), texture);
	}

} // namespace BHive
