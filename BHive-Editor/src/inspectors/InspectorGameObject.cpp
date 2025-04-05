#include "InspectorGameObject.h"
#include "world/GameObject.h"
#include <imgui.h>

namespace BHive
{
	bool Inspector_GameObject::Inspect(
		const rttr::variant &instance, rttr::variant &var, bool read_only, const meta_getter &get_meta_data)
	{
		bool changed = false;
		auto &data = var.get_value<GameObject *>();
		auto component = instance.get_value<Component *>();
		if (component)
		{
			auto world = component->GetOwner()->GetWorld();
			auto objects = world->GetGameObjects();
			auto name = data ? data->GetName() : "None";
			if (ImGui::BeginCombo("##", name.c_str()))
			{
				if (ImGui::Selectable("None", data == nullptr))
				{
					data = nullptr;
					changed |= true;
				}

				for (auto &[id, object] : objects)
				{
					bool selected = object.get() == data;
					ImGui::PushID(((std::string)id).c_str());

					if (ImGui::Selectable(object->GetName().c_str(), selected))
					{
						data = object.get();
						changed |= true;
					}
					ImGui::PopID();
				}

				ImGui::EndCombo();
			}
		}

		if (changed)
		{
			var = data;
		}

		return changed;
	}

	REFLECT_INSPECTOR(Inspector_GameObject, GameObject *);
} // namespace BHive
