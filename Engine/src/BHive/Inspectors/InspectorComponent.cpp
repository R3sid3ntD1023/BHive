#include "InspectorComponent.h"
#include "world/Component.h"
#include "world/GameObject.h"
#include "gui/ImGuiExtended.h"
#include "Inspect.h"

namespace BHive
{
	bool InspectorComponent::Inspect(FPropertyData &property_data, const bool is_read_only)
	{
		bool changed = false, removed = false;
		auto data = property_data.Value.get_value<Component *>();

		int flags = ImGuiTreeNodeFlags_SpanAvailWidth;
		auto type = data->get_type();
		auto name = type.get_name();
		bool opened = ImGui::TreeNodeEx(name.data(), flags);

		if (opened)
		{
			auto properties = type.get_properties();
			for (auto property : properties)
			{
				rttr::instance obj = property_data.Value;
				changed |= Inspect::inspect({}, obj, property, is_read_only);
			}

			if (type.get_metadata(ClassMetaData_ComponentSpawnable) && !is_read_only)
			{
				auto size = ImGui::GetContentRegionAvail();
				auto button_size = ImVec2{100, ImGui::GetLineHeight()};
				ImGui::SetCursorPosX(size.x - button_size.x);

				if (ImGui::Button("Remove", button_size))
				{
					removed |= true;
				}
			}

			ImGui::TreePop();
		}

		if (removed)
		{
			auto remove_method = type.get_method(REMOVE_COMPONENT_FUNCTION_NAME);
			remove_method.invoke({data->GetOwner()});
		}

		return changed;
	}

	REFLECT_INSPECTOR(InspectorComponent, Component)
} // namespace BHive