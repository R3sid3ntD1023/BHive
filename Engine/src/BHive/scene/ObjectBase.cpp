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

    void ObjectBase::Serialize(StreamWriter &writer) const
    {
        writer(mData.mName);
        writer(mID);
    }

    void ObjectBase::Deserialize(StreamReader &reader)
    {
        reader(mData.mName);
        reader(mID);
    }

} // namespace BHive
