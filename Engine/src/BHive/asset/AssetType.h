#pragma once

#include <stdint.h>
#include "reflection/Reflection.h"
#include "serialization/Serialization.h"

namespace BHive
{
    using AssetType = rttr::type;
    static const AssetType InvalidType = rttr::type::get<rttr::detail::invalid_type>();

    void Serialize(StreamWriter &ar, const rttr::type &obj);

    void Deserialize(StreamReader &ar, rttr::type &obj);
}
