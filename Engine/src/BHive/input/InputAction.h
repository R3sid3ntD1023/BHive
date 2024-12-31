#pragma once

#include "core/Core.h"
#include "InputKey.h"
#include "serialization/Serialization.h"
#include "reflection/Reflection.h"

namespace BHive
{
    struct FInputAction : public ISerializable
    {
        FInputAction() = default;

        FInputAction(const std::string &name, const FInputKey &key)
            : mName(name), mKey(key)
        {
        }

        FInputKey GetKey() const { return mKey; }
        bool IsMouse() const { return mKey.mIsMouse; }
        bool IsKey() const { return !mKey.mIsMouse; }

        void Serialize(StreamWriter &ar) const;
        void Deserialize(StreamReader &ar);

    private:
        std::string mName;
        FInputKey mKey;

        REFLECTABLE()

        friend class InputContext;
    };

    REFLECT_EXTERN(FInputAction)

} // namespace BHive
