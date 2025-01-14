#include "AssetMetaData.h"

namespace BHive
{
    void FAssetMetaData::Serialize(StreamWriter &ar) const
    {
        ar(Type, Path, Name);
    }

    void FAssetMetaData::Deserialize(StreamReader &ar)
    {
        ar(Type, Path, Name);
    }

    REFLECT(FAssetMetaData)
    {
        BEGIN_REFLECT(FAssetMetaData)
        REFLECT_PROPERTY("Type", Type)
        REFLECT_PROPERTY("Path", Path)
        REFLECT_PROPERTY("Name", Name);
    }
}