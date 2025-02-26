#pragma once

#include "Inspector.h"

namespace BHive
{
    DECLARE_INSPECTOR(String);

    DECLARE_INSPECTOR(Float);

    DECLARE_INSPECTOR(Bool);

    template <typename TIntegerType>
    DECLARE_INSPECTOR(Int);

} // namespace BHive
