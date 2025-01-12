#pragma once

#include <string>

namespace std
{
    template <typename TArchive, typename T, typename Traits, typename Alloc>
    inline void Serialize(TArchive& ar, const basic_string<T, Traits, Alloc>& str)
    {
        ar(str.size());
        ar.WriteImpl(str.data(), sizeof(T) * str.size());
    }

    template <typename TArchive , typename T, typename Traits, typename Alloc>
    inline void Deserialize(TArchive& ar , basic_string<T, Traits, Alloc>& str)
    {
        size_t size;
        ar(size);

        str.resize(size);
        ar.ReadImpl(str.data(), sizeof(T) * size);
    }
}