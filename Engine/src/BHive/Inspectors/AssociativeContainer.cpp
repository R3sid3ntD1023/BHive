#include "AssociativeContainer.h"
#include "gui/ImGuiExtended.h"
#include "Inspect.h"
#include "AssociativeContainerModifiers.h"

#define CONTEXT_MENU "##KEY_CONTEXT_MENU"

namespace BHive
{

	bool Inspector_AssociativeContainer::Inspect(
		const rttr::variant &instance, rttr::variant &var, bool read_only, const meta_getter &get_meta_data)
	{
		static auto treeflags = ImGuiTreeNodeFlags_SpanAvailWidth;
		static auto table_flags = ImGuiTableFlags_Borders;
		FAssociativeViewModifier *m_modifier = nullptr;

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
				std::pair<bool, bool> element_changed = {false, false};
				auto old_key = key;

				auto key_id = ImGui::GetID(std::format("##Key{}", i).c_str());
				auto value_id = ImGui::GetID(std::format("##Value{}", i).c_str());

				bool is_visable = ImGui::BeginTable("##KeyValueTable", 2, table_flags);

				if (is_visable)
				{
					ImGui::TableNextRow();
					ImGui::TableNextColumn();

					ImGui::PushID(key_id);
					{
						element_changed.first =
							Inspect::inspect(instance, key, false, read_only, 0.0f, Inspect::meta_data_empty);

						if (!read_only)
						{
							ImGui::OpenPopupOnItemClick(CONTEXT_MENU);

							if (ImGui::BeginPopup(CONTEXT_MENU))
							{
								if (ImGui::Selectable("Delete"))
								{
									m_modifier = new FAssociativeViewDelete(key);
								}

								ImGui::EndPopup();
							}
						}
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

					if (element_changed.first || element_changed.second)
					{
						m_modifier = new FAssociativeViewChange(old_key, key, value);
					}
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
				auto key_type = data.get_key_type();
				auto value_type = data.get_value_type();
				m_modifier = new FAssociativeViewAppend(key_type, value_type);
			}

			ImGui::SameLine();

			if (ImGui::Button("-"))
			{
				m_modifier = new FAssociativeViewRemove();
			}

			ImGui::SameLine();

			if (ImGui::Button("Clear"))
			{
				m_modifier = new FAssociativeViewClear();
			}
		}

		if (m_modifier)
		{
			changed |= m_modifier->modify(data);
			delete m_modifier;
		}

		return changed;
	}

	REFLECT_INSPECTOR(Inspector_AssociativeContainer, rttr::variant_associative_view);
} // namespace BHive