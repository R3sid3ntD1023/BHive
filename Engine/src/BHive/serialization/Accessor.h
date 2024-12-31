#pragma once

namespace BHive
{
    class StreamWriter;
    class StreamReader;

    struct accessor
    {
        template <typename T>
        static auto Serialize(StreamWriter &stream, const T &t) -> decltype(t.Serialize(stream))
        {
           return t.Serialize(stream);
        }

        template<typename T>
        static auto Deserialize(StreamReader& stream, T& t) -> decltype(t.Deserialize(stream))
        {
            return t.Deserialize(stream);
        }
    };
} // namespace BHive
