#pragma once

#include <utility>

namespace BHive
{

    template <typename TArchive, typename T1, typename T2>
    inline void Serialize(TArchive& ar, const std::pair<T1, T2>& obj)
    {
        ar(obj.first, obj.second);
    }

    template <typename TArchive, typename T1, typename T2>
    inline void Deserialize(TArchive& ar, std::pair<T1, T2>& obj)
    {
        ar(obj.first, obj.second);
    }

}