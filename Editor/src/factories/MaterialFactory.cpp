#include "MaterialFactory.h"
#include "material/Material.h"
#include "asset/AssetSerializer.h"

namespace BHive
{
    void MaterialFactory::CreateNew(const std::filesystem::path &path)
    {
        Material material;
        AssetSerializer::serialize(material, path);
    }

    bool MaterialFactory::Import(Ref<Asset> &asset, const std::filesystem::path &path)
    {
        auto material = CreateRef<Material>();
        if (AssetSerializer::deserialize(*material, path))
        {
            asset = material;
        }

        return asset != nullptr;
    }

    REFLECT_FACTORY(MaterialFactory, Material, ".material")
}