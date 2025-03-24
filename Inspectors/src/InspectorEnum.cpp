#include "InspectorEnum.h"
#include "gui/ImGuiExtended.h"
#include "core/EnumAsByte.h"

namespace BHive
{

	Inspector_Enum::EnumNames &Inspector_Enum::GetEnumNameValues(const rttr::enumeration &enumeration)
	{
		auto enum_id = enumeration.get_type().get_id();
		if (mEnumNameCache.contains(enum_id))
			return mEnumNameCache.at(enum_id);

		for (auto &name : enumeration.get_names())
		{
			auto value = enumeration.name_to_value(name).to_int();
			mEnumNameCache[enum_id].emplace(value, name.data());
		}

		return mEnumNameCache.at(enum_id);
	}

	bool Inspector_Enum::Inspect(
		const rttr::variant &instance, rttr::variant &var, bool read_only, const meta_getter &get_meta_data)
	{
		auto type = var.get_type();
		auto data = var.to_int();
		auto enumeration = type.get_enumeration();
		auto &name_value = GetEnumNameValues(enumeration);

		std::string display_name = "None";
		if (name_value.contains(data))
			display_name = name_value.at(data);

		bool changed = false;

		if (read_only)
		{
			ImGui::Text("%s", display_name.c_str());
			return false;
		}

		if (ImGui::BeginCombo("##", display_name.c_str()))
		{

			for (auto &[value, name] : name_value)
			{
				if (ImGui::Selectable(name.c_str(), value == data))
				{

					data = value;
					changed |= true;
				}
			}

			ImGui::EndCombo();
		}

		if (changed)
		{
			rttr::variant arg(data);
			arg.convert(var.get_type());
			var = arg;
		}

		return changed;
	}

	bool Inspector_EnumAsByte::Inspect(
		const rttr::variant &instance, rttr::variant &var, bool read_only, const meta_getter &get_meta_data)
	{
		auto data = &var.get_value<TEnumAsByteBase>();
		auto enumeration = data->GetEnumeration();
		auto &name_value = GetEnumNameValues(enumeration);
		std::string display_name = "";
		auto current_value = data->Get();

		if (read_only)
		{
			ImGui::Text("%s", display_name.c_str());
			return false;
		}

		for (auto &[value, name] : name_value)
		{
			bool has_flag = (current_value & value) != 0;
			if (has_flag)
				display_name += name + "|";
		}

		if (display_name.empty())
			display_name = "None";

		bool changed = false;
		if (ImGui::BeginCombo("##", display_name.c_str()))
		{
			for (auto &[value, name] : name_value)
			{
				if (value == 0)
					continue;

				changed |= ImGui::CheckboxFlags(name.c_str(), &current_value, value);
			}

			ImGui::EndCombo();
		}

		if (changed)
		{
			data->Set(current_value);
		}

		return changed;
	}

	REFLECT_INSPECTOR(Inspector_Enum, rttr::enumeration);
	REFLECT_INSPECTOR(Inspector_EnumAsByte, TEnumAsByteBase);

} // namespace BHive