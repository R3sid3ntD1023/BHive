#include "gui/ImGuiExtended.h"
#include "inspector/Inspectors.h"
#include "SequentialContainer.h"

namespace BHive
{
	bool Inspector_SequentialContainer::Inspect(rttr::variant &var, bool read_only, const meta_getter &get_meta_data)
	{
		auto data = var.create_sequential_view();
		auto type = data.get_value_type();
		auto size = data.get_size();
		auto is_dynamic = data.is_dynamic();

		bool changed = false;

		auto drawlist = ImGui::GetWindowDrawList();
		ImDrawListSplitter splitter;

		auto flags_var = get_meta_data(EPropertyMetaData_Flags);
		auto flags = flags_var ? flags_var.to_uint32() : 0;
		auto fixed_size = ((flags & EPropertyFlags_FixedSize) != 0);

		if (is_dynamic && !fixed_size && !read_only)
		{
			auto width = ImGui::GetContentRegionAvail().x;

			ImGui::PushItemWidth(width - ImGui::GetLineHeight() * 4);

			ImGui::AlignTextToFramePadding();
			ImGui::TextUnformatted("Size");
			ImGui::SameLine();

			int int_size = (int)size;
			if (ImGui::DragInt("##Size", &int_size))
			{
				int_size = ImClamp(int_size, 0, int_size);

				changed |= data.set_size((size_t)int_size);
			}

			ImGui::PopItemWidth();
			ImGui::SameLine();

			if (ImGui::Button("+"))
			{
				data.insert(data.end(), type.create());
				changed |= true;
			}

			ImGui::SameLine();

			if (ImGui::Button("-") && size > 0)
			{
				data.erase(data.end() - 1);
				changed |= true;
			}
		}

		static auto treeflags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowOverlap | ImGuiTreeNodeFlags_OpenOnArrow;
		static ImU32 element_colors[2] = {{0xFF8A8A8A}, {0xFF454545}};

		if (ImGui::TreeNodeEx("##Elements", treeflags))
		{
			auto width = ImGui::GetContentRegionAvail().x;

			for (size_t i = 0; i < data.get_size(); i++)
			{

				splitter.Split(drawlist, 2);

				auto element = data.get_value((size_t)i).extract_wrapped_value();
				std::string name = std::format("Element{}", i);

				splitter.SetCurrentChannel(drawlist, 1);

				{
					ImGui::BeginGroup();

					ImGui::PushID(name.c_str());
					changed |= inspect(element, false, read_only, meta_data_empty);
					ImGui::PopID();

					if (changed)
						data.set_value((size_t)i, element);

					ImGui::EndGroup();
				}

				splitter.SetCurrentChannel(drawlist, 0);

				auto item_rect = ImGui::GetItemRect();
				auto padding = GImGui->Style.FramePadding;
				drawlist->AddRectFilled(item_rect.Min - padding, item_rect.Max + padding, element_colors[i % 2]);

				splitter.Merge(drawlist);

				if (!read_only)
				{
					if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
					{
						ImGui::SetDragDropPayload("VECTOR_ITEM", &i, sizeof(size_t));

						ImGui::EndDragDropSource();
					}

					if (ImGui::BeginDragDropTarget())
					{
						if (auto payload = ImGui::AcceptDragDropPayload("VECTOR_ITEM"))
						{
							auto index = *(size_t *)payload->Data;
							auto valueA = data.get_value(index).extract_wrapped_value();
							auto valueB = data.get_value(i).extract_wrapped_value();

							if (data.set_value(index, valueB) && data.set_value(i, valueA))
							{
								changed |= true;
							}
						}

						ImGui::EndDragDropTarget();
					}
				}
			}

			ImGui::TreePop();
		}

		return changed;
	}

	REFLECT_INSPECTOR(Inspector_SequentialContainer, rttr::variant_sequential_view);

} // namespace BHive