#pragma once

#include "Inspector.h"

namespace BHive
{
    struct InspectorEnum : public Inspector
    {
        using EnumNames = std::unordered_map<int, std::string>;

        virtual bool Inspect(rttr::variant &var, bool read_only, const meta_getter &get_meta_data = {});

        REFLECTABLEV(Inspector)

    protected:
        EnumNames &GetEnumNameValues(const rttr::enumeration &enumeration);

    private:
        static inline std::unordered_map<rttr::type::type_id, EnumNames> mEnumNameCache;
    };

    struct InspectorEnumAsBtye : public InspectorEnum
    {

        virtual bool Inspect(rttr::variant &var, bool read_only, const meta_getter &get_meta_data = {});

        REFLECTABLEV(InspectorEnum)
    };
} // namespace BHive
