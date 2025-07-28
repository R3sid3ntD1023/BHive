#include "InspectorBaseTypes.h"
#include "core/UUID.h"
#include "gui/ImGuiExtended.h"
#include "gfx/Color.h"
#include "Inspect.h"

namespace BHive
{
	bool Inspector_UUID::Inspect(FPropertyData &property_data, const bool is_read_only)
	{
		auto data = property_data.Value.get_value<UUID>();

		ImGui::TextWrapped("%s", ((std::string)data).c_str());

		return false;
	}

	template <glm::length_t L, typename T, glm::qualifier Q>
	bool OnInspectVector(
		glm::vec<L, T, Q> &vec, const glm::vec<L, T, Q> &defaultvalue, const glm::vec<L, T, Q> &min,
		const glm::vec<L, T, Q> &max, const char *format)
	{
		static const char *component_names[] = {"X", "Y", "Z", "W"};
		static ImU32 component_colors[] = {0xFF0000FF, 0xFF00FF00, 0xFFFF0000, 0xFF00AAFF};

		bool changed = false;

		float line_height = ImGui::GetLineHeight();
		auto width = ImGui::GetContentRegionAvail().x;
		ImGui::PushMultiItemsWidths(L, width - (line_height * 3.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0, 0});

		for (int i = 0; i < L; i++)
		{
			auto &component = vec[i];
			ImGui::PushStyleColor(ImGuiCol_Button, component_colors[i]);
			if (ImGui::Button(component_names[i], {line_height}))
			{
				component = defaultvalue[i];
				changed |= true;
			}
			ImGui::PopStyleColor();
			ImGui::SameLine();

			ImGui::PushID(i);
			changed |= Inspect::inspect("", component);

			ImGui::PopID();

			ImGui::PopItemWidth();

			if (i < L)
				ImGui::SameLine();
		}

		ImGui::PopStyleVar();

		return changed;
	}

	template <typename T>
	bool Inspector_Vec<T>::Inspect(FPropertyData &property_data, const bool is_read_only)
	{
		static T zero = T(0.0f);
		auto data = property_data.Value.get_value<T>();

		if (is_read_only)
		{
			ImGui::TextUnformatted(glm::to_string(data).c_str());
			return false;
		}

		auto default_value_var = property_data.GetMetaData(EPropertyMetaData_Default);
		auto min_value_var = property_data.GetMetaData(EPropertyMetaData_Min);
		auto max_value_var = property_data.GetMetaData(EPropertyMetaData_Max);
		auto format_var = property_data.GetMetaData(EPropertyMetaData_CustomFormat);

		auto default_value = default_value_var ? default_value_var.get_value<T>() : zero;
		auto min_value = min_value_var ? min_value_var.get_value<T>() : zero;
		auto max_value = max_value_var ? max_value_var.get_value<T>() : zero;
		auto format = format_var ? format_var.to_string() : "%.2f";

		if (OnInspectVector(data, default_value, min_value, max_value, format.c_str()))
		{
			property_data.Value = data;
			return true;
		}

		return false;
	}

	bool Inspector_Color::Inspect(FPropertyData &property_data, const bool is_read_only)
	{
		bool changed = false;
		auto data = property_data.Value.get_value<FColor>();

		if (is_read_only)
		{
			ImGui::Text("%s", data.to_string().c_str());
			return false;
		}

		auto hdr_var = property_data.GetMetaData(EPropertyMetaData_HDR);

		auto flags = (hdr_var ? ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float : 0);
		ImGui::ColorEdit4("", &data.r, flags);
		changed |= ImGui::IsItemDeactivatedAfterEdit();

		if (changed)
		{
			property_data.Value = data;
			return true;
		}

		return false;
	}

	REFLECT_INSPECTOR(Inspector_UUID, UUID)
	REFLECT_INSPECTOR(Inspector_Vec<glm::vec2>, glm::vec2)
	REFLECT_INSPECTOR(Inspector_Vec<glm::vec3>, glm::vec3)
	REFLECT_INSPECTOR(Inspector_Vec<glm::vec4>, glm::vec4)
	REFLECT_INSPECTOR(Inspector_Color, FColor);

} // namespace BHive
