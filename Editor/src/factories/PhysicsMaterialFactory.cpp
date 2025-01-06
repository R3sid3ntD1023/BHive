#include "PhysicsMaterialFactory.h"
#include "physics/PhysicsMaterial.h"
#include "asset/AssetSerializer.h"

namespace BHive
{
    void PhysicsMaterialFactory::CreateNew(const std::filesystem::path &path)
    {
        PhysicsMaterial material;
        AssetSerializer::serialize(material, path);
    }

    bool PhysicsMaterialFactory::Import(Ref<Asset> &asset, const std::filesystem::path &path)
    {
        auto material = CreateRef<PhysicsMaterial>();
        if (AssetSerializer::deserialize(*material, path))
        {
            asset = material;
        }

        return asset != nullptr;
    }

    REFLECT_Factory(PhysicsMaterialFactory, PhysicsMaterial, ".physicsmaterial")
}