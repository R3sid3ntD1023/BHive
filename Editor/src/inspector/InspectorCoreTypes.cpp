#include "gui/ImGuiExtended.h"
#include "inspector/InspectorCoreTypes.h"
#include "inspector/Inspectors.h"
#include <optional>
#include <string>

namespace BHive
{

	bool
	Inspector_String::Inspect(rttr::variant &var, bool read_only, const meta_getter &get_meta_data)
	{
		auto data = var.to_string();
		if (read_only)
		{
			ImGui::TextDisabled(data.c_str());
			return false;
		}

		if (ImGui::InputText(
				"##", &data,
				ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		{
			var = data;
			return true;
		}

		return false;
	}

	bool
	Inspector_Float::Inspect(rttr::variant &var, bool read_only, const meta_getter &get_meta_data)
	{
		auto data = var.to_float();
		bool changed = false;

		if (read_only)
		{
			ImGui::Text("%.2f", data);
			return false;
		}

		auto min_var = get_meta_data(EPropertyMetaData_Min);
		auto max_var = get_meta_data(EPropertyMetaData_Max);
		auto step_var = get_meta_data(EPropertyMetaData_Step);
		auto format_var = get_meta_data(EPropertyMetaData_CustomFormat);
		auto flags_var = get_meta_data(EPropertyMetaData_Flags);

		float min = min_var ? min_var.to_float() : 0.0f;
		float max = max_var ? max_var.to_float() : 0.0f;
		float step = step_var ? step_var.to_float() : 0.01f;
		std::string format = format_var ? format_var.to_string() : "%.2f";
		auto flags = flags_var ? flags_var.to_uint32() : 0;

		if ((flags & EPropertyFlags_Slider) != 0)
		{
			changed |= ImGui::SliderFloat("##", &data, min, max, format.c_str());
		}
		else
		{
			changed |= ImGui::DragFloat("##", &data, step, min, max, format.c_str());
		}

		if (changed)
		{
			if (min_var && (data < min))
				data = min;

			if (max_var && (data > max))
				data = max;

			var = data;
		}

		return changed;
	}

	bool
	Inspector_Bool::Inspect(rttr::variant &var, bool read_only, const meta_getter &get_meta_data)
	{
		auto data = var.to_bool();
		if (read_only)
		{
			ImGui::Text("%s", data ? "True" : "False");
			return false;
		}

		if (ImGui::Checkbox("##", &data))
		{
			var = data;
			return true;
		}

		return false;
	}

	template <typename TIntegerType>
	bool Inspector_Int<TIntegerType>::Inspect(
		rttr::variant &var, bool read_only, const meta_getter &get_meta_data)
	{
		auto data = (int)var.get_value<TIntegerType>();
		bool changed = false;

		if (read_only)
		{
			ImGui::Text("%d", data);
			return false;
		}

		auto min_var = get_meta_data(EPropertyMetaData_Min);
		auto max_var = get_meta_data(EPropertyMetaData_Max);
		auto step_var = get_meta_data(EPropertyMetaData_Step);
		auto format_var = get_meta_data(EPropertyMetaData_CustomFormat);
		auto flags_var = get_meta_data(EPropertyMetaData_Flags);

		auto min = min_var ? min_var.to_int() : 0;
		auto max = max_var ? max_var.to_int() : 0;
		auto step = step_var ? step_var.to_int() : 1;
		std::string format = format_var ? format_var.to_string() : "%d";
		auto flags = flags_var ? flags_var.to_uint32() : 0;

		if ((flags & EPropertyFlags_Slider) != 0)
		{
			changed |= ImGui::SliderInt("##", &data, min, max, format.c_str());
		}
		else
		{
			changed |= ImGui::DragInt("##", &data, step, min, max, format.c_str());
		}

		if (changed)
		{
			if (min_var && (data <= min))
				data = min;

			if (max_var && (data >= max))
				data = max;

			var = (TIntegerType)data;
		}

		return changed;
	}

	bool Inspector_StdVector::Inspect(
		rttr::variant &var, bool read_only, const meta_getter &get_meta_data)
	{
		auto data = var.create_sequential_view();
		auto type = data.get_value_type();
		auto size = data.get_size();
		auto is_dynamic = data.is_dynamic();

		bool changed = false;

		auto drawlist = ImGui::GetWindowDrawList();

		static auto treeflags = ImGuiTreeNodeFlags_SpanAvailWidth |
								ImGuiTreeNodeFlags_AllowOverlap | ImGuiTreeNodeFlags_OpenOnArrow;
		if (ImGui::TreeNodeEx("##Elements", treeflags))
		{
			auto width = ImGui::GetContentRegionAvail().x;
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0, 20});

			for (size_t i = 0; i < size; i++)
			{
				auto element = data.get_value((size_t)i).extract_wrapped_value();
				std::string name = "Element";
				name += std::to_string(i);

				// drawlist->ChannelsSplit(2);
				// drawlist->ChannelsSetCurrent(1);
				ImGui::BeginGroup();

				{
					ScopedPropertyLayout layout(name);

					if (inspect(element, false, read_only, meta_data_empty))
					{

						changed |= true;
						if (changed)
							data.set_value((size_t)i, element);
					}
				}
				ImGui::EndGroup();

				// auto item_rect = ImGui::GetItemRect();
				// item_rect.Min -= GImGui->Style.FramePadding;
				// item_rect.Max += GImGui->Style.FramePadding;

				// drawlist->ChannelsSetCurrent(0);

				// drawlist->AddRectFilled(item_rect.Min, item_rect.Max, ImGui::GetColorU32({.15f,
				// .15f, .15f, 1.f}));

				// drawlist->ChannelsMerge();

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

			ImGui::PopStyleVar();
			ImGui::TreePop();
		}

		auto flags_var = get_meta_data(EPropertyMetaData_Flags);
		auto flags = flags_var ? flags_var.to_uint32() : 0;
		auto fixed_size = ((flags & EPropertyFlags_FixedSize) != 0);

		if (is_dynamic && !fixed_size)
		{
			if (ImGui::Button("+"))
			{
				data.insert(data.end(), type.create());
				changed |= true;
			}

			ImGui::SameLine();

			if (ImGui::Button("-"))
			{
				data.erase(data.end() - 1);
				changed |= true;
			}

			ImGui::SameLine();

			auto int_size = (int)size;
			if (ImGui::InputInt("Size", &int_size))
			{
				changed |= data.set_size((size_t)int_size);
			}
		}

		return changed;
	}

	bool
	Inspector_StdMap::Inspect(rttr::variant &var, bool read_only, const meta_getter &get_meta_data)
	{
		auto data = var.create_associative_view();
		auto key_type = data.get_key_type();
		auto value_type = data.get_value_type();
		auto size = data.get_size();

		bool changed = false;

		static auto treeflags = ImGuiTreeNodeFlags_SpanAvailWidth |
								ImGuiTreeNodeFlags_AllowOverlap | ImGuiTreeNodeFlags_OpenOnArrow;
		if (ImGui::TreeNodeEx("Elements", treeflags))
		{
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {10, 10});

			size_t i = 0;

			for (auto it = data.begin(); it != data.end();)
			{
				auto &element = (*it);
				auto oldkey = element.first.extract_wrapped_value();
				auto newkey = oldkey;
				auto value = element.second.extract_wrapped_value();
				std::pair<bool, bool> element_changed = {false, false};

				auto key_id = "Key" + std::to_string(i);
				auto value_id = "Value" + std::to_string(i);

				ImGui::BeginTable("##KeyValueTable", 2, ImGuiTableFlags_Borders);

				ImGui::TableNextRow();
				ImGui::TableNextColumn();

				ImGui::PushID(key_id.c_str());
				element_changed.first = inspect(newkey, false, read_only, meta_data_empty);
				ImGui::PopID();

				ImGui::TableNextColumn();

				ImGui::PushID(value_id.c_str());
				element_changed.second = inspect(value, false, read_only, meta_data_empty);
				ImGui::PopID();

				ImGui::EndTable();

				// DEBUG_DRAW_RECT(0xFF00FF00)

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

			ImGui::PopStyleVar();
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
				auto it = data.insert(key_type.create(), value_type.create());
				changed |= it.second;
			}

			ImGui::SameLine();

			if (ImGui::Button("-"))
			{
				data.erase(data.end() - 1);
				changed |= true;
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

	REFLECT_INSPECTOR(Inspector_String, std::string);
	REFLECT_INSPECTOR(Inspector_Float, float);
	REFLECT_INSPECTOR(Inspector_Bool, bool);
	REFLECT_INSPECTOR(Inspector_StdVector, rttr::variant_sequential_view);
	REFLECT_INSPECTOR(Inspector_StdMap, rttr::variant_associative_view);

	REFLECT_INSPECTOR(Inspector_Int<uint8_t>, uint8_t);
	REFLECT_INSPECTOR(Inspector_Int<uint32_t>, uint32_t);
	REFLECT_INSPECTOR(Inspector_Int<uint64_t>, uint64_t);

	REFLECT_INSPECTOR(Inspector_Int<int8_t>, int8_t);
	REFLECT_INSPECTOR(Inspector_Int<int32_t>, int32_t);
	REFLECT_INSPECTOR(Inspector_Int<int64_t>, int64_t);

} // namespace BHive
