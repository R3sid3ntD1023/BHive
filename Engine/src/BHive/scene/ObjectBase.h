#pragma once

#include "core/Core.h"
#include "core/UUID.h"
#include "reflection/Reflection.h"
#include "serialization/Serialization.h"

namespace BHive
{
    struct FObjectInitializer
    {
        std::string mName = "NewObject";
    };

    class ObjectBase : public ISerializable
    {
    private:
        /* data */
    public:
        ObjectBase(const FObjectInitializer &initializer = {});
        ObjectBase(const ObjectBase &other) = default;

        virtual ~ObjectBase() = default;

        const std::string &GetName() const { return mData.mName; }
        void SetName(const std::string &name);

        const UUID &GetUUID() const { return mID; }

        virtual void Serialize(StreamWriter &writer) const;

        virtual void Deserialize(StreamReader &reader);

        bool operator==(const ObjectBase &rhs) const { return mID == rhs.mID; }
        bool operator!=(const ObjectBase &rhs) const { return mID != rhs.mID; }

    private:
        FObjectInitializer mData;
        UUID mID;

        REFLECTABLEV()
    };

    REFLECT(ObjectBase)
    {
        BEGIN_REFLECT(ObjectBase)
        REFLECT_PROPERTY_READ_ONLY("UUID", mID)
        REFLECT_PROPERTY("Name", GetName, SetName);
    }

} // namespace BHive
