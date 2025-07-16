#include "core/subsystem/SubSystem.h"
#include "gui/ImGuiExtended.h"
#include "Inspectors.h"
#include "PropertiesPanel.h"
#include "subsystems/SelectionSubSystem.h"
#include "world/GameObject.h"

namespace BHive
{
	void PropertiesPanel::OnGuiRender()
	{
		auto selection = GetSubSystem<SelectionSubSystem>().GetSelection();
		if (!selection)
			return;

		if (selection)
		{
			ImGui::SeparatorText("Details");

			rttr::variant object_var = selection;
			if (inspect({}, object_var))
			{
				selection = object_var.get_value<GameObject *>();
			}

			ImGui::SeparatorText("Components");

			DrawComponents(selection);

			ImGui::Separator();

			DrawAddComponent(selection);

			for (auto &component : mDeleteComponents)
			{
				component.invoke(selection);
			}

			if (mDeleteComponents.size())
			{
				mDeleteComponents.clear();
			}
		}
	}

	void PropertiesPanel::DrawComponents(GameObject *obj)
	{
		for (auto &component : obj->GetComponents())
		{
			DrawComponent(component);
		}
	}

	void PropertiesPanel::DrawComponent(Component *component)
	{
		auto selection = GetSubSystem<SelectionSubSystem>().GetSelection();

		ImGui::PushID(component);

		int flags = ImGuiTreeNodeFlags_SpanAvailWidth;
		auto type = component->get_type();
		auto name = type.get_name();
		bool opened = ImGui::TreeNodeEx(name.data(), flags);

		if (opened)
		{
			rttr::variant var = component;
			rttr::variant instance = selection;
			if (inspect(instance, var))
			{
				component = var.get_value<Component *>();
			}

			if (type.get_metadata(ClassMetaData_ComponentSpawnable))
			{
				auto size = ImGui::GetContentRegionAvail();
				auto button_size = ImVec2{100, ImGui::GetLineHeight()};
				ImGui::SetCursorPosX(size.x - button_size.x);

				if (ImGui::Button("Remove", button_size))
				{
					auto remove_method = type.get_method(REMOVE_COMPONENT_FUNCTION_NAME);
					mDeleteComponents.push_back(remove_method);
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

		ImGui::SetNextWindowSize({0, 200}, ImGuiCond_Always);

		if (ImGui::BeginPopup(ADD_COMPONENT_FUNCTION_NAME))
		{

			for (auto &type : derived_component_types)
			{
				auto spawnable_var = type.get_metadata(ClassMetaData_ComponentSpawnable);
				auto has_method = type.get_method(HAS_COMPONENT_FUNCTION_NAME).invoke(obj).to_bool();

				if (!spawnable_var || !type.get_constructor() || has_method)
				{
					continue;
				}

				if (ImGui::Selectable(type.get_name().data()))
				{
					type.get_method(ADD_COMPONENT_FUNCTION_NAME).invoke(obj);
				}
			}
			ImGui::EndPopup();
		}
	}

} // namespace BHive
