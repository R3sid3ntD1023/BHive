#include "AssociativeContainer.h"
#include "gui/ImGuiExtended.h"
#include "Inspect.h"

#define CONTEXT_MENU "##KEY_CONTEXT_MENU"

namespace BHive
{
	using edit_associative_conainter_func = std::function<bool(rttr::variant_associative_view &)>;

	bool Inspector_AssociativeContainer::Inspect(
		const rttr::variant &instance, rttr::variant &var, bool read_only, const meta_getter &get_meta_data)
	{
		static auto treeflags = ImGuiTreeNodeFlags_SpanAvailWidth;
		static auto table_flags = ImGuiTableFlags_Borders;

		edit_associative_conainter_func edit_func = nullptr;

		auto data = var.create_associative_view();
		bool changed = false;

		if (ImGui::TreeNodeEx("Elements", treeflags))
		{
			size_t i = 0;

			for (auto it = data.begin(); it != data.end(); it++, i++)
			{
				auto &element = (*it);
				auto key = element.first.extract_wrapped_value();
				auto value = element.second.extract_wrapped_value();
				auto old_key = key;

				auto key_id = ImGui::GetID(std::format("##Key{}", i).c_str());
				auto value_id = ImGui::GetID(std::format("##Value{}", i).c_str());

				bool is_visible = ImGui::BeginTable("##KeyValueTable", 2, table_flags);

				if (is_visible)
				{
					ImGui::TableNextRow();
					ImGui::TableNextColumn();

					ImGui::PushID(key_id);
					{

						if (Inspect::inspect(instance, key, false, read_only, 0.0f, Inspect::meta_data_empty))
						{
							edit_func = [key, old_key, value](rttr::variant_associative_view &view)
							{
								view.erase(old_key);
								return view.insert(key, value).second;
							};
						}

						if (!read_only)
						{
							ImGui::OpenPopupOnItemClick(CONTEXT_MENU);

							if (ImGui::BeginPopup(CONTEXT_MENU))
							{
								if (ImGui::Selectable("Delete"))
								{
									edit_func = [key](rttr::variant_associative_view &view) { return view.erase(key); };
								}

								ImGui::EndPopup();
							}
						}
					}
					ImGui::PopID();

					ImGui::TableNextColumn();

					ImGui::PushID(value_id);

					if (Inspect::inspect(instance, value, false, read_only, 0.0f, Inspect::meta_data_empty))
					{
						edit_func = [key, old_key, value](rttr::variant_associative_view &view)
						{
							view.erase(old_key);
							return view.insert(key, value).second;
						};
					}

					ImGui::PopID();

					ImGui::EndTable();
				}
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
				edit_func = [](rttr::variant_associative_view &view)
				{
					auto key = view.get_key_type().create();
					auto value = view.get_value_type().create();

					return view.insert(key, value).second;
				};
			}

			ImGui::SameLine();

			if (ImGui::Button("-"))
			{
				edit_func = [](rttr::variant_associative_view &view)
				{
					if (!view.get_size())
						return false;

					auto key = view.end() - 1;
					return view.erase(key.get_key().extract_wrapped_value()) != 0;
				};
			}

			ImGui::SameLine();

			if (ImGui::Button("Clear"))
			{
				edit_func = [](rttr::variant_associative_view &view)
				{
					view.clear();
					return true;
				};
			}
		}

		if (edit_func)
		{
			changed |= edit_func(data);
		}

		return changed;
	}

	REFLECT_INSPECTOR(Inspector_AssociativeContainer, rttr::variant_associative_view);
} // namespace BHive