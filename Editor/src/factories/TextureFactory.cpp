#include "TextureFactory.h"
#include "importers/TextureImporter.h"

namespace BHive
{

    bool TextureFactory::Import(Ref<Asset> &asset, const std::filesystem::path &path)
    {
        asset = TextureImporter::Import(path);
        return asset != nullptr;
    }

    REFLECT_Factory(TextureFactory, Texture, ".png", ".jpg", ".jpeg")
} // namespace BHive
