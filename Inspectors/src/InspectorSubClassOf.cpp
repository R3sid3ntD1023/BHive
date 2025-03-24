#include "core/SubClassOf.h"
#include "gui/ImGuiExtended.h"
#include "InspectorSubclassOf.h"

namespace BHive
{
	bool Inspector_SubClassOf::Inspect(
		const rttr::variant &instance, rttr::variant &var, bool read_only, const meta_getter &get_meta_data)
	{
		bool changed = false;

		auto &data = var.extract_wrapped_value().get_value<rttr::type>();
		auto template_arguments = var.get_type().get_template_arguments();
		auto base_type = *template_arguments.begin();
		auto var_type = var.get_type();

		auto name = (data ? data.get_name() : "None");

		if (read_only)
		{
			ImGui::TextUnformatted(name.data());
			return changed;
		}

		if (ImGui::BeginCombo("##Classes", name.data()))
		{
			auto derived_types = base_type.get_derived_classes();

			if (ImGui::Selectable("None", data == InvalidType))
			{
				data = InvalidType;
				changed |= true;
			}

			if (ImGui::Selectable(base_type.get_name().data(), data == base_type))
			{
				data = base_type;
				changed |= true;
			}

			for (auto &type : derived_types)
			{
				if (ImGui::Selectable(type.get_name().data(), data == type))
				{
					data = type;
					changed |= true;
				}
			}

			ImGui::EndCombo();
		}

		if (changed)
		{
			var = var_type.create({data});
		}
		return changed;
	}

	REFLECT_INSPECTOR(Inspector_SubClassOf, SubClassOf)
} // namespace BHive