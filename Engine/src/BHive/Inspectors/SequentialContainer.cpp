#include "SequentialContainer.h"
#include "gui/ImGuiExtended.h"
#include "Inspect.h"

namespace BHive
{
	using edit_sequential_conainter_func = std::function<bool(rttr::variant_sequential_view &)>;

	bool Inspector_SequentialContainer::Inspect(
		const rttr::variant &instance, rttr::variant &var, bool read_only, const meta_getter &get_meta_data)
	{
		auto data = var.create_sequential_view();
		auto type = data.get_value_type();
		auto size = data.get_size();
		auto is_dynamic = data.is_dynamic();

		bool changed = false;
		edit_sequential_conainter_func edit_func = nullptr;

		auto drawlist = ImGui::GetWindowDrawList();
		ImDrawListSplitter splitter;

		auto flags_var = get_meta_data(EPropertyMetaData_Flags);
		auto flags = flags_var ? flags_var.to_uint32() : 0;
		auto fixed_size = ((flags & EPropertyFlags_FixedSize) != 0);

		// header buttons
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

				edit_func = [int_size](rttr::variant_sequential_view &view) { return view.set_size((size_t)int_size); };
			}

			ImGui::PopItemWidth();
			ImGui::SameLine();

			if (ImGui::Button("+"))
			{

				edit_func = [](rttr::variant_sequential_view &view)
				{
					view.insert(view.end(), view.get_value_type().create());
					return true;
				};
			}

			ImGui::SameLine();

			if (ImGui::Button("-") && size > 0)
			{
				edit_func = [](rttr::variant_sequential_view &view)
				{
					view.erase(view.end() - 1);
					return true;
				};
			}
		}

		static auto treeflags =
			ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowOverlap | ImGuiTreeNodeFlags_OpenOnArrow;
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
					if (Inspect::inspect(instance, element, false, read_only, 0.0f, Inspect::meta_data_empty))
					{
						edit_func = [i, element](rttr::variant_sequential_view &view)
						{
							view.set_value((size_t)i, element);
							return true;
						};
					}
					ImGui::PopID();

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

							edit_func = [index, i, valueA, valueB](rttr::variant_sequential_view &view)
							{ return view.set_value(index, valueB) && view.set_value(i, valueA); };
						}

						ImGui::EndDragDropTarget();
					}
				}
			}

			ImGui::TreePop();
		}

		if (edit_func)
		{
			changed |= edit_func(data);
		}

		return changed;
	}

	REFLECT_INSPECTOR(Inspector_SequentialContainer, rttr::variant_sequential_view);

} // namespace BHive