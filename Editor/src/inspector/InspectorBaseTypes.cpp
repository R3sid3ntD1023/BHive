#include "inspector/InspectorBaseTypes.h"
#include "core/UUID.h"
#include "gui/ImGuiExtended.h"
#include "math/Math.h"
#include "gfx/Color.h"
#include "Inspectors.h"

namespace BHive
{
    bool Inspector_UUID::Inspect(rttr::variant &var, bool read_only, const meta_getter &get_meta_data)
    {
        auto data = var.get_value<UUID>();

        ImGui::Text("%u", (uint64_t)data);

        return false;
    }

    template <typename T>
    bool OnInspectVectorComponent(T *component, const T &min, const T &max,
                                  const char *format)
    {
    }

    template <>
    bool OnInspectVectorComponent(float *component, const float &min, const float &max,
                                  const char *format)
    {
        return ImGui::DragFloat("", component, .01f, min, max, format);
    }

    template <glm::length_t L, typename T, glm::qualifier Q>
    bool OnInspectVector(glm::vec<L, T, Q> &vec, const glm::vec<L, T, Q> &defaultvalue, const glm::vec<L, T, Q> &min,
                         const glm::vec<L, T, Q> &max, const char *format)
    {
        static const char *component_names[] = {"X", "Y", "Z", "W"};
        static ImU32 component_colors[] = {0xFF0000FF, 0xFF00FF00, 0xFFFF0000, 0xFF00AAFF};

        bool changed = false;

        float line_height = ImGui::GetLineHeight();
        auto button_size = ImVec2(line_height + 3.f, line_height);

        ImGui::PushMultiItemsWidths(L, ImGui::CalcItemWidth() - (button_size.x * 3.f));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0, 0});

        for (int i = 0; i < L; i++)
        {
            auto &component = vec[i];
            ImGui::PushStyleColor(ImGuiCol_Button, component_colors[i]);
            if (ImGui::Button(component_names[i], button_size))
            {
                component = defaultvalue[i];
                changed |= true;
            }
            ImGui::PopStyleColor();
            ImGui::SameLine();

            ImGui::PushID(i);
            changed |= OnInspectVectorComponent<T>(&component, min[i], max[i], format);
            ImGui::PopID();

            ImGui::PopItemWidth();

            if (i < L)
                ImGui::SameLine();
        }

        ImGui::PopStyleVar();

        return changed;
    }

    template <typename T>
    bool Inspector_Vec<T>::Inspect(rttr::variant &var, bool read_only, const meta_getter &get_meta_data)
    {
        static T zero = T(0.0f);
        auto data = var.get_value<T>();

        if (read_only)
        {
            ImGui::TextUnformatted(glm::to_string(data).c_str());
            return false;
        }

        auto default_value_var = get_meta_data("DefaultValue");
        auto min_value_var = get_meta_data("MinValue");
        auto max_value_var = get_meta_data("MaxValue");
        auto format_var = get_meta_data("Format");

        auto default_value = default_value_var ? default_value_var.get_value<T>() : zero;
        auto min_value = min_value_var ? min_value_var.get_value<T>() : zero;
        auto max_value = max_value_var ? max_value_var.get_value<T>() : zero;
        auto format = format_var ? format_var.to_string() : "%.2f";

        if (OnInspectVector(data, default_value, min_value, max_value, format.c_str()))
        {
            var = data;
            return true;
        }

        return false;
    }

    bool Inspector_Color::Inspect(rttr::variant &var, bool read_only, const meta_getter &get_meta_data)
    {
        auto data = var.get_value<Color>();

        if (read_only)
        {
            ImGui::Text("%s", data.to_string().c_str());
            return false;
        }

        auto hdr_var = get_meta_data("HDR");

        auto flags = (hdr_var ? ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float : 0);

        if (ImGui::ColorEdit4("", &data.r, flags))
        {
            var = data;
            return true;
        }

        return false;
    }

    REFLECT_INSPECTOR(Inspector_UUID, UUID)
    REFLECT_INSPECTOR(Inspector_Vec<glm::vec2>, glm::vec2)
    REFLECT_INSPECTOR(Inspector_Vec<glm::vec3>, glm::vec3)
    REFLECT_INSPECTOR(Inspector_Vec<glm::vec4>, glm::vec4)
    REFLECT_INSPECTOR(Inspector_Color, Color);

} // namespace BHive
