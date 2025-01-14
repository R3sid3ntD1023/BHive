#include "Color.h"

namespace BHive
{
    void Color::Serialize(StreamWriter &ar) const
    {
        ar(r, g, b, a);
    }

    void Color::Deserialize(StreamReader &ar)
    {
        ar(r, g, b, a);
    }
}