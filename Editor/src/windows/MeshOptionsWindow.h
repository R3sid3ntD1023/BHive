#pragma once

#include "WindowBase.h"
#include "mesh/MeshImportData.h"
#include "factories/MeshFactory.h"

namespace BHive
{
    struct FMeshImportOptions;
    class Skeleton;
	class MeshFactory;
	class Texture;
	class Material;

    class MeshOptionsWindow : public WindowBase
    {
    public:
        MeshOptionsWindow(MeshFactory* factory, const FMeshImportOptions &importData);

        virtual void OnUpdateContent() final override;

        bool ShouldClose() const override { return WindowBase::ShouldClose() || mShouldClose; };

    protected:
        void CreateAssetMetaFile();
		void SetMaterialTexture(FTextureData::EType type, const Ref<Texture> &texture,
								Ref<Material> &material);

        virtual const char *GetName() const { return "Mesh Import"; }

    private:
        bool mIsOpen = true;
        FMeshImportOptions mImportData;

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
		MeshFactory *mFactory = nullptr;
    };

} // namespace BHive
