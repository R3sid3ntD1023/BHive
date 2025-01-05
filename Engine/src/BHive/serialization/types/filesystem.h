#pragma once

#include <filesystem>

namespace BHive
{
    template<typename TArchive>
    inline void Deserialize(TArchive & ar, std::filesystem::path& path)
    {
        std::string path_str;
        ar(path_str);
        path = path_str;
    }

    template<typename TArchive>
    inline void Serialize(TArchive& ar, const std::filesystem::path& path)
    {
        ar(path.string());
    }
}