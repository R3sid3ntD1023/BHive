#include "Player.h"
#include "scene/components/InputComponent.h"

namespace BHive
{
    Player::Player()
    {
        mInputComponent = AddNewComponent<InputComponent>("Input");
    }

    REFLECT(Player)
    {
        BEGIN_REFLECT(Player)
        (META_DATA(ClassMetaData_Spawnable, true))
            REFLECT_CONSTRUCTOR();
    }
} // namespace BHive
