#pragma once

#include "Inspector.h"

namespace BHive
{
    rttr::variant meta_data_empty(const rttr::variant &);

    bool inspect(rttr::variant &var, bool skip_custom = false, bool read_only = false, const Inspector::meta_getter &get_meta_data = meta_data_empty);

    bool inspect(rttr::instance &object, rttr::property &property, bool read_only = false);

    template <typename T>
    bool inspect(const std::string &label, const T &obj, bool skip_custom = false, const Inspector::meta_getter &get_meta_data = meta_data_empty)
    {
        ScopedPropertyLayout layout(label);
        rttr::variant var = obj;
        return inspect(var, skip_custom, true, get_meta_data);
    }

    template <typename T>
    bool inspect(const std::string &label, T &obj, bool skip_custom = false, bool read_only = false, const Inspector::meta_getter &get_meta_data = meta_data_empty)
    {
        ScopedPropertyLayout layout(label);
        rttr::variant var = obj;
        bool changed = inspect(var, skip_custom, read_only, get_meta_data);

        if (changed)
        {
            obj = var.get_value<T>();
        }

        return changed;
    }

} // namespace BHive
