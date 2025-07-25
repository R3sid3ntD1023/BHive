#include "Inspector_Blackboard.h"
#include "Animator/anim_player/Blackboard.h"
#include "Inspect.h"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

namespace BHive
{
#define ADD_KEY_LIST "Add Key List"

	bool Inspector_Blackboard::Inspect(
		const rttr::variant &instance, rttr::variant &var, bool read_only, const meta_getter &get_meta_data)
	{
		static const auto key_types = rttr::type::get<BlackBoardKey>().get_derived_classes();
		bool changed = false;
		auto data = var.get_value<BlackBoard>();

		if (ImGui::Button("Add"))
		{
			ImGui::OpenPopup(ADD_KEY_LIST);
		}

		if (ImGui::BeginPopup(ADD_KEY_LIST))
		{
			static std::string key_name = "New Key";
			ImGui::InputTextWithHint("Key Name", "New Key", &key_name);

			for (const auto &type : key_types)
			{
				if (ImGui::Selectable(type.get_name().data()))
				{
					auto new_key = type.create().get_value<Ref<BlackBoardKey>>();
					changed |= data.AddKey(key_name, new_key);
				}
			}
			ImGui::EndPopup();
		}

		auto &keys = data.GetKeys();
		ImGui::TextUnformatted("Keys");
		changed |= Inspect::inspect("", keys);

		if (changed)
		{
			var = data;
		}

		return changed;
	}

	REFLECT_INSPECTOR(Inspector_Blackboard, BlackBoard)
} // namespace BHive