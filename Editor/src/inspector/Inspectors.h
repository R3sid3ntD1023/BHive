#pragma once

#include "Inspector.h"

namespace BHive
{
    rttr::variant meta_data_empty(const rttr::variant &);

    bool inspect(rttr::variant &var, bool skip_custom = false, bool read_only = false, const Inspector::meta_getter &get_meta_data = meta_data_empty);

    bool inspect(rttr::instance &object, rttr::property &property, bool read_only = false);

} // namespace BHive
