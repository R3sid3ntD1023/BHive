#include "BlackBoard.h"

namespace BHive
{
    void BlackBoard::RemoveKey(const std::string &name)
    {
        ASSERT(mKeys.contains(name));

        mKeys.erase(name);
    }

    BlackBoardKey *BlackBoard::GetKey(const std::string &name) const
    {
        BlackBoardKey* key = nullptr;

        if(mKeys.contains(name))
            key = mKeys.at(name).get();

        return key;  
    }

} // namespace BHive
