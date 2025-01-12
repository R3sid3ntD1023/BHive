#pragma once

namespace BHive
{
    class StreamWriter;
    class StreamReader;

    struct accessor
    {
        template <typename TArchive, typename T>
        static auto Serialize(TArchive &ar, const T &t) -> decltype(t.Serialize(ar))
        {
           return t.Serialize(ar);
        }

       template <typename TArchive, typename T>
        static auto Deserialize(TArchive& ar, T& t) -> decltype(t.Deserialize(ar))
        {
            return t.Deserialize(ar);
        }

    };
} // namespace BHive
