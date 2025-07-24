#include "AssociativeContainer.h"
#include "gui/ImGuiExtended.h"
#include "Inspect.h"

namespace BHive
{
	bool Inspector_AssociativeContainer::Inspect(
		const rttr::variant &instance, rttr::variant &var, bool read_only, const meta_getter &get_meta_data)
	{
		auto data = var.create_associative_view();
		auto key_type = data.get_key_type();
		auto value_type = data.get_value_type();
		auto size = data.get_size();

		bool changed = false;

		static auto treeflags = ImGuiTreeNodeFlags_SpanAvailWidth;
		static auto table_flags = ImGuiTableFlags_Borders;

		float width = ImGui::GetContentRegionAvail().x;
		if (ImGui::TreeNodeEx("Elements", treeflags))
		{
			size_t i = 0;

			for (auto it = data.begin(); it != data.end();)
			{
				auto &element = (*it);
				auto oldkey = element.first.extract_wrapped_value();
				auto newkey = oldkey;
				auto value = element.second.extract_wrapped_value();
				std::pair<bool, bool> element_changed = {false, false};

				auto key_id = ImGui::GetID(std::format("##Key{}", i).c_str());
				auto value_id = ImGui::GetID(std::format("##Value{}", i).c_str());

				ImGui::BeginTable("##KeyValueTable", 2, table_flags);

				ImGui::TableNextRow();
				ImGui::TableNextColumn();

				ImGui::PushID(key_id);
				{
					element_changed.first =
						Inspect::inspect(instance, newkey, false, read_only, 0.0f, Inspect::meta_data_empty);
				}
				ImGui::PopID();

				ImGui::TableNextColumn();

				ImGui::PushID(value_id);

				{
					element_changed.second =
						Inspect::inspect(instance, value, false, read_only, 0.0f, Inspect::meta_data_empty);
				}

				ImGui::PopID();

				ImGui::EndTable();

				changed |= element_changed.first || element_changed.second;

				if (element_changed.first || element_changed.second)
				{
					data.erase(oldkey);
					it = data.insert(newkey, value).first;
				}
				else
				{
					it++;
				}

				i++;
			}

			ImGui::TreePop();
		}

		if (read_only)
			return false;

		auto flags_var = get_meta_data(EPropertyMetaData_Flags);
		auto flags = flags_var ? flags_var.to_uint32() : 0;
		auto fixed_size = ((flags & EPropertyFlags_FixedSize) != 0);

		if (!fixed_size)
		{
			if (ImGui::Button("+"))
			{
				auto key = key_type.create();
				auto value = value_type.create();

				auto it = data.insert(key, value);
				changed |= it.second;
			}

			ImGui::SameLine();

			if (ImGui::Button("-"))
			{
				if (size > 0)
				{
					auto key = data.end() - 1;
					changed |= (data.erase(*key) > 0);
				}
			}

			ImGui::SameLine();

			if (ImGui::Button("Clear"))
			{
				data.clear();
				changed |= true;
			}
		}

		return changed;
	}

	REFLECT_INSPECTOR(Inspector_AssociativeContainer, rttr::variant_associative_view);
} // namespace BHive