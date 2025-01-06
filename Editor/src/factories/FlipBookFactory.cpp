#include "FlipBookFactory.h"
#include "sprite/FlipBook.h"
#include "asset/AssetSerializer.h"

namespace BHive
{
    void FlipBookFactory::CreateNew(const std::filesystem::path &path)
    {
        FlipBook flipbook;
        AssetSerializer::serialize(flipbook, path);
    }

    bool FlipBookFactory::Import(Ref<Asset> &asset, const std::filesystem::path &path)
    {
        auto flipbook = CreateRef<FlipBook>();
        if (AssetSerializer::deserialize(*flipbook, path))
        {
            asset = flipbook;
        }

        return asset != nullptr;
    }

    REFLECT_Factory(FlipBookFactory, FlipBook, ".flipbook");

} // namespace BHive
