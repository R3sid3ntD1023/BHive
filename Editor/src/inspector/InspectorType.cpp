#include "InspectorType.h"
#include "gui/ImGuiExtended.h"
#include <rttr/type.h>

namespace BHive
{
    bool Inspector_Type::Inspect(rttr::variant &var, bool read_only, const meta_getter &metadata)
    {
        auto data = var.get_value<rttr::type>();
        auto name = data.get_name();

        ImGui::TextUnformatted(name.data());
        return false;
    }

    REFLECT_INSPECTOR(Inspector_Type, rttr::type);
} // namespace BHive
