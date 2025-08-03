#include "InspectorComponent.h"
#include "world/Component.h"
#include "world/GameObject.h"
#include "gui/ImGuiExtended.h"
#include "Inspect.h"
#include "core/subsystem/SubSystem.h"
#include "undoredo/UndoRedo.h"

namespace BHive
{
	struct FCommandRemoveComponent : public ICommand
	{
		FCommandRemoveComponent(GameObject *obj, Component *component)
			: mObj(obj),
			  mComponent(component)
		{
		}

		virtual void on_undo() override { mComponent->get_type().get_method(EMPLACE_OR_REPLACE_COMPONENT_FUNCTION_NAME).invoke({mObj}, mComponent); }

		virtual void on_redo() override { mComponent->get_type().get_method(REMOVE_COMPONENT_FUNCTION_NAME).invoke({mObj}); }

	private:
		GameObject *mObj;
		Component *mComponent;
	};

	bool InspectorComponent::inspect(const rttr::variant &owner, rttr::variant &var, const MetaGetter &GetMetaData, const bool is_read_only)
	{
		bool changed = false, removed = false;
		auto data = var.get_value<Component *>();

		int flags = ImGuiTreeNodeFlags_SpanAvailWidth;
		auto type = data->get_type();
		auto name = type.get_name();
		bool opened = ImGui::TreeNodeEx(name.data(), flags);

		if (opened)
		{
			auto properties = type.get_properties();

			for (auto property : properties)
			{
				changed |= Inspect::get().inspect(owner, var, property, is_read_only);
			}

			if (type.get_metadata(ClassMetaData_ComponentSpawnable) && !is_read_only)
			{
				auto size = ImGui::GetContentRegionAvail();
				auto button_size = ImVec2{100, ImGui::GetLineHeight()};
				ImGui::SetCursorPosX(size.x - button_size.x);

				if (ImGui::Button("Remove", button_size))
				{
					GetSubSystem<UndoRedo>().add_history_command<FCommandRemoveComponent>("Removed Component", data->GetOwner(), data);
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