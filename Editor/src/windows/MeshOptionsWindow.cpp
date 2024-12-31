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

namespace BHive
{
    MeshOptionsWindow::MeshOptionsWindow(FMeshImportOptions &importData)
        : WindowBase(ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize), mImportData(importData)
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
            auto meta_path = path.parent_path() / (path.stem().string() + ".meta");
            CreateAssetMetaFile(meta_path);
            mShouldClose = true;
        }
    }

    void MeshOptionsWindow::CreateAssetMetaFile(const std::filesystem::path &path)
    {
        auto asset_manager = AssetManager::GetAssetManager<EditorAssetManager>();
        if (!asset_manager)
        {
            return;
        }

        MaterialTable material_table;

        auto &material_data = mImportData.mData.mMaterialData;
        size_t num_materials = material_data.size();
        material_table.resize(num_materials);

        if (import_materials)
        {
            for (size_t i = 0; i < num_materials; i++)
            {

                auto &data = material_data[i];

                auto &textures = data.mTextureData;
                auto num_textures = textures.size();

                for (size_t i = 0; i < num_textures; i++)
                {
                    auto &texture = textures[i];
                    if (!texture.is_embedded())
                    {
                        asset_manager->ImportAsset(path.parent_path() / texture.mPath);
                    }
                }

                auto name = data.mName.empty() ? path.stem().string() + std::to_string(i) : data.mName;
                auto material_path = mMaterialDirectory / (name + ".material");

                if (!std::filesystem::exists(material_path))
                {

                    Material material;
                    material.mAldebo = {data.mAlbedo.x, data.mAlbedo.y, data.mAlbedo.z, data.mAlbedo.w};
                    material.mMetallic = data.mMetallic;
                    material.mRoughness = data.mRoughness;

                    FileStreamWriter ar(material_path);
                    ar(material);
                }

                asset_manager->ImportAsset(material_path);

                auto material_handle = asset_manager->GetHandle(material_path);
                material_table.set_material(AssetManager::GetAsset<Material>(material_handle), i);
            }
        }

        if (as_static_mesh)
        {
            FileStreamWriter ar(path);
            ar(AssetType::get<StaticMesh>());
            ar(mImportData.mData.mMeshData, material_table);
            return;
        }
        else if (has_animations || as_skeletal_mesh)
        {
            auto skeleton_path = path.parent_path() / (path.stem().string() + ".skeleton");

            if (!std::filesystem::exists(skeleton_path))
            {

                FileStreamWriter ar(skeleton_path);
                ar(mImportData.mData.mBoneData, mImportData.mData.mSkeletonHeirarchyData);
            }

            asset_manager->ImportAsset(skeleton_path);
            auto skeleton_handle = asset_manager->GetHandle(skeleton_path);

            if (as_skeletal_mesh)
            {
                FileStreamWriter ar(path);
                ar(AssetType::get<SkeletalMesh>());
                ar(mImportData.mData.mMeshData, material_table, skeleton_handle);
            }

            if (import_animations)
            {
                auto &animation_data = mImportData.mData.mAnimationData;
                size_t num_animations = animation_data.size();

                for (size_t i = 0; i < num_animations; i++)
                {
                    auto &data = animation_data[i];

                    auto name = data.mName.empty() ? path.stem().string() + std::to_string(i) : data.mName;
                    auto animation_path = mAnimationDirectory / (name + ".animation");

                    if (!std::filesystem::exists(animation_path))
                    {
                        FileStreamWriter ar(animation_path);
                        ar(data.mDuration, data.TicksPerSecond, data.mGlobalInverseMatrix, data.mFrames, skeleton_handle);
                    }

                    asset_manager->ImportAsset(animation_path);
                }
            }
        }
    }

} // namespace BHive
