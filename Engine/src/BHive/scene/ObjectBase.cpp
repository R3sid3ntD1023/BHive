#include "ObjectBase.h"

namespace BHive
{
    ObjectBase::ObjectBase(const FObjectInitializer &initializer)
        : mData(initializer)
    {
    }

    void ObjectBase::SetName(const std::string &name)
    {
        mData.mName = name;
    }

    void ObjectBase::GenerateNewUUID()
    {
        mID = UUID();
    }

    ObjectBase* ObjectBase::Copy()
    {
        return new ObjectBase(*this);
    }

    void ObjectBase::Serialize(StreamWriter &ar) const
    {
        ar(mData.mName);
        ar(mID);
    }

    void ObjectBase::Deserialize(StreamReader &ar)
    {
        ar(mData.mName);
        ar(mID);
    }

    REFLECT(ObjectBase)
    {
        BEGIN_REFLECT(ObjectBase)
        REFLECT_PROPERTY_READ_ONLY("UUID", mID)
        REFLECT_PROPERTY("Name", GetName, SetName);
    }

} // namespace BHive
