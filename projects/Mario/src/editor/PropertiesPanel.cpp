#include "PropertiesPanel.h"
#include "gui/ImGuiExtended.h"
#include "objects/GameObject.h"
#include "editor/SelectionSubSystem.h"
#include "subsystem/SubSystem.h"
#include "Inspectors.h"

namespace BHive
{
	void PropertiesPanel::OnGuiRender()
	{
		auto selection = SubSystemContext::Get().GetSubSystem<SelectionSubSystem>().GetSelection();
		if (!selection)
			return;

		if (selection)
		{
			DrawComponents(selection);

			ImGui::SeparatorText("Details");

			DrawAddComponent(selection);
		}
	}

	void PropertiesPanel::DrawComponents(GameObject *obj)
	{
		for (auto &component : obj->GetComponents())
		{
			DrawComponent(component.second);
		}
	}

	void PropertiesPanel::DrawComponent(Component *component)
	{
		auto selection = SubSystemContext::Get().GetSubSystem<SelectionSubSystem>().GetSelection();

		ImGui::PushID(component);

		int flags = ImGuiTreeNodeFlags_SpanAvailWidth;
		auto type = component->get_type();
		auto name = type.get_name();
		bool opened = ImGui::TreeNodeEx(name.data(), flags);

		if (opened)
		{
			rttr::variant var = component;
			if (inspect(var))
			{
				component = var.get_value<Component *>();
			}

			if (type.get_metadata(ClassMetaData_ComponentSpawnable))
			{
				auto size = ImGui::GetContentRegionAvail();
				auto button_size = ImVec2{100, ImGui::GetLineHeight()};
				ImGui::SetCursorPosX(size.x - button_size.x);

				if (auto method = type.get_method(REMOVE_COMPONENT_FUNCTION_NAME);
					method && ImGui::Button("Remove", button_size))
				{
					method.invoke(selection);
				}
			}

			ImGui::TreePop();
		}

		ImGui::PopID();
	}

	void PropertiesPanel::DrawAddComponent(GameObject *obj)
	{
		auto selection = SubSystemContext::Get().GetSubSystem<SelectionSubSystem>().GetSelection();
		static auto derived_component_types = rttr::type::get<Component>().get_derived_classes();

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

		if (ImGui::BeginPopup(ADD_COMPONENT_FUNCTION_NAME))
		{
			for (auto &type : derived_component_types)
			{
				auto spawnable_var = type.get_metadata(ClassMetaData_ComponentSpawnable);

				if (!spawnable_var || !type.get_constructor())
				{
					continue;
				}

				if (ImGui::Selectable(type.get_name().data()))
				{
					if (auto method = type.get_method(ADD_COMPONENT_FUNCTION_NAME))
					{
						method.invoke(selection);
					}
				}
			}
			ImGui::EndPopup();
		}
	}

} // namespace BHive
