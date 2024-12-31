#include "InputAction.h"

namespace BHive
{
    void FInputAction::Serialize(StreamWriter &ar) const
    {
        ar(mName, mKey);
    }

    void FInputAction::Deserialize(StreamReader &ar)
    {
        ar(mName, mKey);
    }

    REFLECT(FInputAction)
    {
        BEGIN_REFLECT(FInputAction)
        REFLECT_CONSTRUCTOR()
        CONSTRUCTOR_POLICY_OBJECT
        REFLECT_CONSTRUCTOR(const std::string &, const FInputKey &)
        CONSTRUCTOR_POLICY_OBJECT
        REFLECT_PROPERTY("Name", mName)
        REFLECT_PROPERTY("Key", mKey);
    }

} // namespace BHive
