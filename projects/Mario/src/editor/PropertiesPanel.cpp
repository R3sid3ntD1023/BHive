#include "PropertiesPanel.h"
#include "gui/ImGuiExtended.h"
#include "objects/GameObject.h"
#include "Inspectors.h"

namespace BHive
{
	void PropertiesPanel::OnGuiRender()
	{
		if (!GetSelectedObjectEvent)
			return;

		if (auto obj = GetSelectedObjectEvent.invoke())
		{
			DrawComponents(obj);

			ImGui::SeparatorText("Details");

			DrawAddComponent(obj);

			if (mDeletedComponents.size())
			{
				for (auto component : mDeletedComponents)
				{
				}

				mDeletedComponents.clear();
			}
		}
	}

	void PropertiesPanel::DrawComponents(GameObject *obj)
	{
		for (auto &component : obj->GetComponents())
		{
			DrawComponent(component.second.get());
		}
	}

	void PropertiesPanel::DrawComponent(Component *component)
	{
		ImGui::PushID(component);

		int flags = ImGuiTreeNodeFlags_SpanAvailWidth;
		auto name = component->get_type().get_name();
		bool opened = ImGui::TreeNodeEx(name.data(), flags);

		if (opened)
		{
			rttr::variant var = component;
			if (inspect(var))
			{
				component = var.get_value<Component *>();
			}

			ImGui::TreePop();
		}

		ImGui::PopID();
	}

	void PropertiesPanel::DrawAddComponent(GameObject *obj)
	{
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
				auto is_spawnable = spawnable_var ? spawnable_var.to_bool() : false;

				if (!is_spawnable || !type.get_constructor())
				{
					continue;
				}

				if (ImGui::Selectable(type.get_name().data()))
				{
					auto object = GetSelectedObjectEvent.invoke();
					if (auto method = type.get_method(ADD_COMPONENT_FUNCTION_NAME))
					{
						method.invoke(object);
					}
				}
			}
			ImGui::EndPopup();
		}
	}

} // namespace BHive
