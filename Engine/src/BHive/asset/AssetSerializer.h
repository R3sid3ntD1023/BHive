#pragma once

#include "core/Core.h"
#include "serialization/Serialization.h"

namespace BHive
{

    struct AssetSerializer
    {
        template <typename T>
        static bool serialize(const T &asset, const std::filesystem::path &path)
        {
            FileStreamWriter ar(path);
            if (!ar)
                return false;

            ar(asset);
            return true;
        }

        template <typename T>
        static bool deserialize(T &asset, const std::filesystem::path &path)
        {
            FileStreamReader ar(path);
            if (!ar)
                return false;

            ar(asset);
            return true;
        }
    };

} // namespace BHive
