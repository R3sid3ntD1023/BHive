#include "InspectorCoreTypes.h"
#include "gui/ImGuiExtended.h"
#include "Inspect.h"
#include <optional>
#include <string>

namespace BHive
{

	bool Inspector_String::inspect(const rttr::variant &owner, rttr::variant &var, const MetaGetter &GetMetaData, const bool is_read_only)
	{
		auto data = var.to_string();
		if (is_read_only)
		{
			ImGui::TextDisabled(data.c_str());
			return false;
		}

		bool changed = ImGui::InputText("##", &data, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);

		auto default_var = GetMetaData(EPropertyMetaData_Default);
		auto default_value = default_var ? default_var.to_string() : "";
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Reset"))
			{
				data = default_value;
				changed |= true;
			}
			ImGui::EndPopup();
		}

		if (changed)
		{
			var = data;
		}

		return changed;
	}

	bool Inspector_Float::inspect(const rttr::variant &owner, rttr::variant &var, const MetaGetter &GetMetaData, const bool is_read_only)
	{
		auto data = var.to_float();
		bool changed = false;

		if (is_read_only)
		{
			ImGui::Text("%.5f", data);
			return false;
		}

		auto min_var = GetMetaData(EPropertyMetaData_Min);
		auto max_var = GetMetaData(EPropertyMetaData_Max);
		auto step_var = GetMetaData(EPropertyMetaData_Step);
		auto format_var = GetMetaData(EPropertyMetaData_CustomFormat);
		auto flags_var = GetMetaData(EPropertyMetaData_Flags);

		float min = min_var ? min_var.to_float() : 0.0f;
		float max = max_var ? max_var.to_float() : 0.0f;
		float step = step_var ? step_var.to_float() : 0.01f;
		std::string format = format_var ? format_var.to_string() : "%.3f";
		auto flags = flags_var ? flags_var.to_uint32() : 0;

		if ((flags & EPropertyFlags_Slider) != 0)
		{
			changed |= ImGui::SliderFloat("##", &data, min, max, format.c_str());
		}
		else
		{
			changed |= ImGui::DragFloat("##", &data, step, min, max, format.c_str());
		}

		auto default_var = GetMetaData(EPropertyMetaData_Default);
		auto default_value = default_var ? default_var.to_float() : 0.f;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Reset"))
			{
				data = default_value;
				changed |= true;
			}
			ImGui::EndPopup();
		}

		if (changed)
		{
			if (min_var && (data < min))
				data = min;

			if (max_var && (data > max))
				data = max;

			var = data;
		}

		return changed;
	}

	bool Inspector_Bool::inspect(const rttr::variant &owner, rttr::variant &var, const MetaGetter &GetMetaData, const bool is_read_only)
	{
		auto data = var.to_bool();
		bool changed = false;

		if (is_read_only)
		{
			ImGui::Text("%s", data ? "True" : "False");
			return false;
		}

		changed |= ImGui::Checkbox("##", &data);

		auto default_var = GetMetaData(EPropertyMetaData_Default);
		auto default_value = default_var ? default_var.to_bool() : false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Reset"))
			{
				data = default_value;
				changed |= true;
			}
			ImGui::EndPopup();
		}

		if (changed)
		{
			var = data;
		}

		return changed;
	}

	template <typename TIntegerType>
	bool Inspector_Int<TIntegerType>::inspect(const rttr::variant &owner, rttr::variant &var, const MetaGetter &GetMetaData, const bool is_read_only)
	{
		auto data = (int)var.get_value<TIntegerType>();
		bool changed = false;

		if (is_read_only)
		{
			ImGui::Text("%d", data);
			return false;
		}

		auto min_var = GetMetaData(EPropertyMetaData_Min);
		auto max_var = GetMetaData(EPropertyMetaData_Max);
		auto step_var = GetMetaData(EPropertyMetaData_Step);
		auto format_var = GetMetaData(EPropertyMetaData_CustomFormat);
		auto flags_var = GetMetaData(EPropertyMetaData_Flags);
		auto default_var = GetMetaData(EPropertyMetaData_Default);

		auto min = min_var ? min_var.to_int() : 0;
		auto max = max_var ? max_var.to_int() : 0;
		auto step = step_var ? step_var.to_float() : 1.f;
		std::string format = format_var ? format_var.to_string() : "%d";
		auto flags = flags_var ? flags_var.to_uint32() : 0;
		auto default_value = default_var ? default_var.to_int() : 0;

		if ((flags & EPropertyFlags_Slider) != 0)
		{
			changed |= ImGui::SliderInt("##", &data, min, max, format.c_str());
		}
		else
		{
			changed |= ImGui::DragInt("##", &data, step, min, max, format.c_str());
		}

		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Reset"))
			{
				data = default_value;
				changed |= true;
			}
			ImGui::EndPopup();
		}

		if (changed)
		{
			if (min_var && (data <= min))
				data = min;

			if (max_var && (data >= max))
				data = max;

			var = (TIntegerType)data;
		}

		return changed;
	}

	REFLECT_INSPECTOR(Inspector_String, std::string);
	REFLECT_INSPECTOR(Inspector_Float, float);
	REFLECT_INSPECTOR(Inspector_Bool, bool);

	REFLECT_INSPECTOR(Inspector_Int<uint8_t>, uint8_t);
	REFLECT_INSPECTOR(Inspector_Int<uint16_t>, uint16_t);
	REFLECT_INSPECTOR(Inspector_Int<uint32_t>, uint32_t);
	REFLECT_INSPECTOR(Inspector_Int<uint64_t>, uint64_t);

	REFLECT_INSPECTOR(Inspector_Int<int8_t>, int8_t);
	REFLECT_INSPECTOR(Inspector_Int<int16_t>, int16_t);
	REFLECT_INSPECTOR(Inspector_Int<int32_t>, int32_t);
	REFLECT_INSPECTOR(Inspector_Int<int64_t>, int64_t);

} // namespace BHive
