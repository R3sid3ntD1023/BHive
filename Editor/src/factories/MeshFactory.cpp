#include "MeshFactory.h"
#include "mesh/SkeletalAnimation.h"
#include "subsystem/SubSystem.h"
#include "subsystems/WindowSubSystem.h"
#include "mesh/MeshImportData.h"
#include "windows/MeshOptionsWindow.h"
#include "mesh/MeshImporter.h"
#include "mesh/SkeletalMesh.h"
#include "threading/Threading.h"
#include "asset/AssetSerializer.h"

namespace BHive
{
    struct ImportJob : public IJob
    {
        ImportJob(const FMeshImportOptions &data)
            : mData(data)
        {
        }

        virtual bool IsDone() { return true; }

        virtual const char *GetName() const { return "MESH::Import"; }

        virtual ThreadFunction GetDispatchedFunction()
        {
            return [this]()
            { ShowImportWindow(); };
        }

        void ShowImportWindow()
        {
            auto &window_system = SubSystemContext::Get().GetSubSystem<WindowSubSystem>();
            auto window = window_system.AddWindow<MeshOptionsWindow>(mData);
        }

        FMeshImportOptions mData;
    };

    bool MeshFactory::Import(Ref<Asset> &asset, const std::filesystem::path &path)
    {
        auto meta_path = path.parent_path() / (path.stem().string() + ".meta");
        if (std::filesystem::exists(meta_path))
        {
            FileStreamReader ar(meta_path);
            AssetType type = InvalidType;

            ar(type);
            if (!type)
                return false;

            auto obj = type.create().get_value<Ref<Asset>>();
            if (!obj)
                return false;

            obj->Deserialize(ar);
            asset = obj;

            return true;
        }

        MeshImporter importer;
        auto import_data = importer.Import(path);

        Thread::PushJob(new ImportJob(FMeshImportOptions{path, import_data}));
        return asset != nullptr;
    }

    bool SkeletonFactory::Import(Ref<Asset> &asset, const std::filesystem::path &path)
    {
        auto skeleton = CreateRef<Skeleton>();
        if (AssetSerializer::deserialize(*skeleton, path))
        {
            asset = skeleton;
        }

        return asset != nullptr;
    }

    bool AnimationFactory::Import(Ref<Asset> &asset, const std::filesystem::path &path)
    {
        auto animation = CreateRef<SkeletalAnimation>();
        if (AssetSerializer::deserialize(*animation, path))
        {
            asset = animation;
        }

        return asset != nullptr;
    }

    REFLECT_Factory(MeshFactory, StaticMesh, ".gltf", ".glb")
    REFLECT_Factory(SkeletonFactory, Skeleton, ".skeleton")
    REFLECT_Factory(AnimationFactory, SkeletalAnimation, ".animation")
}