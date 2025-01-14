#include "AssetType.h"

namespace BHive
{
    void Serialize(StreamWriter &ar, const rttr::type &obj)
    {
        auto name = std::string(obj.get_name().data());
        ar(name);
    }

    void Deserialize(StreamReader &ar, rttr::type &obj)
    {
        std::string name;
        ar(name);
        obj = rttr::type::get_by_name(name);
    }
}