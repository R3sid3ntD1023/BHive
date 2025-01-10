#include "MeshOptionsWindow.h"
#include "gui/ImGuiExtended.h"
#include "asset/AssetManager.h"
#include "asset/EditorAssetManager.h"
#include "factories/MeshFactory.h"
#include "mesh/StaticMesh.h"
#include "mesh/SkeletalMesh.h"
#include "mesh/Skeleton.h"
#include "mesh/SkeletalAnimation.h"
#include "core/FileDialog.h"
#include "inspector/Inspectors.h"
#include "factories/TextureFactory.h"
#include "factories/MaterialFactory.h"


namespace BHive
{
    MeshOptionsWindow::MeshOptionsWindow(MeshFactory *factory, const FMeshImportOptions &importData) 
    : WindowBase(ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize),
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
	}

	void MeshOptionsWindow::OnUpdateContent()
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

        ImGui::BeginDisabled(!has_animations);
        ImGui::Checkbox("Import Animations", &import_animations);

        rttr::variant skeleton_var = mSkeleton;
        if (inspect(skeleton_var))
        {
            mSkeleton = skeleton_var.get_value<TAssetHandle<Skeleton>>();
        }

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
        MaterialTable *material_table = nullptr;

        if (as_static_mesh)
        {
			auto mesh = CreateRef<StaticMesh>(mImportData.mData.mMeshData);
			mesh->SetName(name);
			material_table = &mesh->GetMaterialTable();
			asset = mesh;
        }
        else if (has_animations || as_skeletal_mesh)
        {
			Ref<Skeleton> skeleton = mSkeleton.get();
            if (!mSkeleton)
            {
			    skeleton = CreateRef<Skeleton>(
					mImportData.mData.mBoneData, mImportData.mData.mSkeletonHeirarchyData);

                skeleton->SetName(name + "_Skeleton");
                mFactory->mOtherAssets.push_back(skeleton);
            }

            if (as_skeletal_mesh)
            {
				auto mesh = CreateRef<SkeletalMesh>(mImportData.mData.mMeshData, skeleton);
				material_table = &mesh->GetMaterialTable();
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
						data.mDuration, data.TicksPerSecond, 
						data.mFrames, skeleton,
						data.mGlobalInverseMatrix);
					animations[i]->SetName(anim_name);

                    if (i > 0)
                        mFactory->mOtherAssets.push_back(animations[i]);
                }

                asset = animations[0];
            }
        }

        if (!material_table)
			return;

		auto &material_data = mImportData.mData.mMaterialData;
		size_t num_materials = material_data.size();
		material_table->resize(num_materials);

		if (import_materials)
		{
			for (size_t i = 0; i < num_materials; i++)
			{

				auto &data = material_data[i];

				auto &textures = data.mTextureData;
				auto num_textures = textures.size();

				TextureFactory tex_factory;
				for (size_t i = 0; i < num_textures; i++)
				{
					auto &texture = textures[i];
					Ref<Asset> texture_asset;
					if (!texture.is_embedded())
					{
						texture_asset = tex_factory.Import(texture.mPath);					
					}
                    else
                    {
						texture_asset = tex_factory.Import(texture.mEmbeddedData, texture.mEmbeddedDataSize);
                    }

                    if (texture_asset)
                    {
						texture_asset->SetName(texture.mPath.stem().string());
						mFactory->mOtherAssets.push_back(texture_asset);
                    }
				}

				auto material_name = data.mName.empty() ? name + std::to_string(i) : data.mName;

				Ref<Material> material = CreateRef<Material>();
				material->SetName(material_name);
				material->mAldebo = {data.mAlbedo.x, data.mAlbedo.y, data.mAlbedo.z,
									 data.mAlbedo.w};
				material->mMetallic = data.mMetallic;
				material->mRoughness = data.mRoughness;
				material_table->set_material(material, i);

				mFactory->mOtherAssets.push_back(material);
			}
		}

        if (asset)
            mFactory->OnImportCompleted.invoke(asset);
    }

} // namespace BHive
