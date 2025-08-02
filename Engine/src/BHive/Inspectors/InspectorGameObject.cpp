#include "InspectorGameObject.h"
#include "world/GameObject.h"
#include "Inspect.h"
#include "Gui/ImGuiExtended.h"

namespace BHive
{
	bool Inspector_GameObject::Inspect(const rttr::variant &owner, rttr::variant &var, const MetaGetter &GetMetaData, const bool is_read_only)
	{
		bool changed = false;
		auto &data = var.get_value<GameObject *>();
		auto type = data->get_type();
		auto properties = type.get_properties();

		for (auto property : properties)
		{
			changed |= Inspect::inspect(owner, var, property, is_read_only);
		}

		ImGui::SeparatorText("Components");

		for (auto &component : data->GetComponents())
		{
			changed |= Inspect::inspect("", component, false, is_read_only);
		}

		if (is_read_only)
			return false;

		static auto derived_component_types = rttr::type::get<Component>().get_derived_classes();

		ImGui::Separator();

		auto line_height = ImGui::GetLineHeight();
		auto button_size = ImVec2(200, line_height);
		auto pos = ImGui::GetCursorPosX() + (ImGui::GetContentRegionAvail().x * .5f) - (button_size.x * .5f);
		ImGui::SetCursorPosX(pos);
		ImGui::PushStyleColor(ImGuiCol_Button, {0, .5f, 0, 1});
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, {0, .5f, 0, 1});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {.2f, .6f, .2f, 1});

		if (ImGui::Button("Add Component", button_size))
		{
			ImGui::OpenPopup(ADD_COMPONENT_FUNCTION_NAME);
		}

		ImGui::PopStyleColor(3);

		ImGui::SetNextWindowSize({0, 200}, ImGuiCond_Always);

		if (ImGui::BeginPopup(ADD_COMPONENT_FUNCTION_NAME))
		{

			for (auto &type : derived_component_types)
			{
				auto spawnable_var = type.get_metadata(ClassMetaData_ComponentSpawnable);
				auto has_method = type.get_method(HAS_COMPONENT_FUNCTION_NAME).invoke({data}).to_bool();

				if (!spawnable_var || !type.get_constructor() || has_method)
				{
					continue;
				}

				if (ImGui::Selectable(type.get_name().data()))
				{
					type.get_method(ADD_COMPONENT_FUNCTION_NAME).invoke({data});
					changed |= true;
				}
			}
			ImGui::EndPopup();
		}

		return changed;
	}

	REFLECT_INSPECTOR(Inspector_GameObject, GameObject);
} // namespace BHive
