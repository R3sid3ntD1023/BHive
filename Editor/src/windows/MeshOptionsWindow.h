#pragma once

#include "WindowBase.h"
#include "mesh/MeshImportData.h"
#include "core/EventDelegate.h"

namespace BHive
{
    DECLARE_EVENT(OnMeshOptionsCompleted)
    struct FMeshImportOptions;
    class Skeleton;

    class MeshOptionsWindow : public WindowBase
    {
    public:
        MeshOptionsWindow(FMeshImportOptions &importData);

        virtual void OnUpdateContent() final override;

        bool ShouldClose() const override { return WindowBase::ShouldClose() || mShouldClose; };

    protected:
        void CreateAssetMetaFile(const std::filesystem::path &path);

        virtual const char *GetName() const { return "Mesh Import"; }

    private:
        bool mIsOpen = true;
        FMeshImportOptions &mImportData;

        bool has_bones;
        bool has_materials;
        bool has_animations;
        bool as_static_mesh;
        bool as_skeletal_mesh;
        bool import_materials;
        bool import_animations;

        std::filesystem::path mMaterialDirectory;
        std::filesystem::path mAnimationDirectory;
        bool mShouldClose{false};
        TAssetHandle<Skeleton> mSkeleton;
    };

} // namespace BHive
