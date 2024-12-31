#pragma once

#include "core/Core.h"
#include "TBlackBoardKey.h"
#include "reflection/Reflection.h"

namespace BHive
{
    class BlackBoard
    {
    public:
        /* data */
        using Keys = std::unordered_map<std::string, Scope<BlackBoardKey>>;

    public:
        BlackBoard() = default;
        ~BlackBoard() = default;

        template<typename T>
        void AddKey(const std::string& name, const T& initial_value)
        {
            mKeys.emplace(name, new TBlackBoardKey<T>(initial_value));
        }

        void RemoveKey(const std::string& name);

        BlackBoardKey* GetKey(const std::string& name) const;

        const Keys& GetKeys() const { return mKeys; }

        Keys& GetKeys() { return mKeys; }

    private:
        Keys mKeys;
    };
    
} // namespace BHive
