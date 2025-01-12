#pragma once

#include <map>
#include <unordered_map>

namespace std
{
    template <typename TArchive, template <typename...> class Map, typename... TArgs, typename = typename Map<TArgs...>::mapped_type>
    inline void Serialize(TArchive& ar, const Map<TArgs...>& map)
    {
        ar(map.size());

        for (const auto& i : map)
        {
            ar(i.first, i.second);
        }
    }


    template <typename TArchive, template <typename...> class Map, typename... TArgs, typename = typename Map<TArgs...>::mapped_type>
    void Deserialize(TArchive& ar, Map<TArgs...>& map)
    {
        size_t size;

        ar(size);

        map.clear();
        auto hint = map.begin();

        for (size_t i = 0; i < size; i++)
        {
            typename Map<TArgs...>::key_type key;
            typename Map<TArgs...>::mapped_type value;

            ar(key, value);

            hint = map.emplace_hint(hint, move(key), move(value));
        }
    }

}