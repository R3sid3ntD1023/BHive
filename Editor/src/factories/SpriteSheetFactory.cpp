#include "SpriteSheetFactory.h"
#include "sprite/SpriteSheet.h"
#include "asset/AssetSerializer.h"

namespace BHive
{
    void SpriteSheetFactory::CreateNew(const std::filesystem::path &path)
    {
        SpriteSheet object;
        AssetSerializer::serialize(object, path);
    }

    bool SpriteSheetFactory::Import(Ref<Asset> &asset, const std::filesystem::path &path)
    {
        auto _asset = CreateRef<SpriteSheet>();
        if (AssetSerializer::deserialize(*_asset, path))
        {
            asset = _asset;
            return true;
        }

        return false;
    }

    REFLECT_Factory(SpriteSheetFactory, SpriteSheet, ".spritesheet")
} // namespace BHive
