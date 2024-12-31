#include "future_test.h"
#include <future>
#include <importers/TextureImporter.h>

Ref<BHive::Texture> get_value(const std::filesystem::path &path)
{
    return BHive::TextureImporter::Import(path);
}

Ref<BHive::Texture> TestFuture()
{
    std::future<Ref<BHive::Texture>> test;

    auto path = "C:/Users/dariu/Documents/BHive/Project/resources/textures/heroSpritesheet.png";
    test = std::async(std::launch::deferred, get_value, path);

    return test.get();
}