#pragma once

namespace BHive
{
    class StreamWriter;
    class StreamReader;

    struct ISerializable
    {
        virtual void Serialize(StreamWriter &stream) const = 0;
        virtual void Deserialize(StreamReader &stream) = 0;
    };

} // namespace BHive
