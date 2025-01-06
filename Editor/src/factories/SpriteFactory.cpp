#include "SpriteFactory.h"
#include "sprite/Sprite.h"
#include "asset/AssetSerializer.h"

namespace BHive
{
    void SpriteFactory::CreateNew(const std::filesystem::path &path)
    {
        Sprite sprite;
        AssetSerializer::serialize(sprite, path);
    }

    bool SpriteFactory::Import(Ref<Asset> &asset, const std::filesystem::path &path)
    {
        auto sprite = CreateRef<Sprite>();
        if (AssetSerializer::deserialize(*sprite, path))
        {
            asset = sprite;
        }

        return asset != nullptr;
    }

    REFLECT_Factory(SpriteFactory, Sprite, ".sprite")

} // namespace BHive
