#pragma once

#include "core/Core.h"

namespace BHive
{
    class Texture;

    class ThumbnailCache
    {
    private:
        /* data */
    public:
        ThumbnailCache() = default;
        
        Ref<Texture> Get(const std::filesystem::path& path);

    private:
        std::unordered_map<std::string, Ref<Texture>> mCache;
    };
} // namespace BHive
