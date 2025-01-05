#pragma once

#include <string>

namespace BHive
{
    template <typename TArchive, typename T, typename Traits, typename Alloc>
    inline void Serialize(TArchive& ar, const std::basic_string<T, Traits, Alloc>& str)
    {
        ar(str.size());
        ar.WriteImpl(str.data(), sizeof(T) * str.size());
    }

    template <typename TArchive , typename T, typename Traits, typename Alloc>
    inline void Deserialize(TArchive& ar , std::basic_string<T, Traits, Alloc>& str)
    {
        size_t size;
        ar(size);

        str.resize(size);
        ar.ReadImpl(str.data(), sizeof(T) * size);
    }
}