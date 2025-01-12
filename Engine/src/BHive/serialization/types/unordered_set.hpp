#pragma once

#include <unordered_set>

namespace std
{
    namespace details
    {
        template<typename TArchive, typename TSet>
        void save(TArchive& ar, const TSet& set)
        {
            ar(set.size());

            for (const auto& i : set)
                ar(i);
        }


        template<typename TArchive, typename TSet>
        void load(TArchive& ar, TSet& set)
        {
            size_t size;
            ar(size);

            set.clear();
            set.reserve(size);

            for (size_t i = 0; i < size; i++)
            {
                typename TSet::key_type key;

                ar(key);
                set.emplace(move(key));
            }
        }
    }

    template <typename TArchive, typename K, typename H, typename KE, typename A>
    void Serialize(TArchive& ar, const unordered_set<K, H, KE, A>& unordered_set)
    {
        details::save(ar, unordered_set);
    }

    template <typename TArchive , typename K, typename H, typename KE, typename A>
    void Deserialize(TArchive& ar, unordered_set<K, H, KE, A>& unordered_set)
    {
        details::load(ar, unordered_set);
    }

}