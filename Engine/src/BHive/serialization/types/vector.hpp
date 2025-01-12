#pragma once

#include <vector>

namespace std
{

    template <typename TArchive, typename T>
    inline void Serialize(TArchive& ar, const vector<T>& vec)
    {
        ar(vec.size());

        for (auto& v : vec)
            ar(v);
    }

    template <typename TArchive, typename T>
    inline void Deserialize(TArchive& ar, vector<T>& vec)
    {
        size_t size = 0;
        ar(size);
        vec.clear();

        vec.resize(size);

        for (size_t i = 0; i < size; i++)
        {
            ar(vec[i]);
        }
    }


}