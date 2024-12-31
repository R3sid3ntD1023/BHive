#include "InputKey.h"

namespace BHive
{
    void FInputKey::Serialize(StreamWriter &ar) const
    {
        ar(mKey, mIsMouse);
    }

    void FInputKey::Deserialize(StreamReader &ar)
    {
        ar(mKey, mIsMouse);
    }
}