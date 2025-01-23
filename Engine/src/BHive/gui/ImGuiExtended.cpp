#include "ImGuiExtended.h"
#include <misc/cpp/imgui_stdlib.h>

namespace ImGui
{
	void Image(const BHive::Texture *texture, const ImVec2 size, const ImVec4 &tint_col, const ImVec4 &border_col)
	{
		if (!texture)
			return;

		ImGui::Image((ImTextureID)(uint64_t)(uint32_t)texture->GetRendererID(), size, {0, 1}, {1, 0}, tint_col, border_col);
	}

	bool DrawIcon(const std::string &label, BHive::Texture *icon, float size, ImGuiButtonFlags flags)
	{
		bool pressed = false;

		if (icon)
		{
			auto id = ImGui::GetID(label.c_str());
			pressed = ImageButtonEx(id, (ImTextureID)(uint64_t)(uint32_t)*icon, {size, size}, {0, 1}, {1, 0}, {0, 0, 0, 0}, {1, 1, 1, 1}, flags);
		}
		else
		{
			pressed = ButtonEx("##icon", {size, size}, flags);
		}

		return pressed;
	}

	bool DrawEditableText(const char *str_id, const std::string &label, std::string &editable_text)
	{
		static ImGuiID current_id = -1;
		ImGuiID id = ImGui::GetID(str_id);

		IM_ASSERT(GImGui);
		auto &g = *GImGui;

		if (g.CurrentWindow->SkipItems)
			return false;

		if (current_id != id)
		{
			TextWrapped(label.c_str());
			if (IsMouseDoubleClicked(0) && IsItemHovered())
			{
				current_id = id;
				editable_text = label;
			}
		}
		else
		{
			bool finish_editing =
				(IsWindowHovered(ImGuiHoveredFlags_AnyWindow) && IsMouseClicked(ImGuiMouseButton_Left)) || IsKeyPressed(ImGuiKey_Escape);
			if (finish_editing)
			{
				current_id = -1;
			}

			ImGui::SetKeyboardFocusHere();
			if (InputText("##RenamingName", &editable_text, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			{
				current_id = -1;
				return true;
			}
		}

		return false;
	}

	void OnDragDropSource(const char *type, void *data, size_t size)
	{
		if (BeginDragDropSource())
		{
			SetDragDropPayload(type, data, size);
			EndDragDropSource();
		}
	}

	bool DraggablePoint(const char *str_id, float point[2], float size)
	{

		auto window = GImGui->CurrentWindow;
		if (window->SkipItems)
			return false;

		bool changed = false;
		const ImGuiID id = window->GetID(str_id);

		auto pos = window->DC.CursorPos;
		auto item_size = ImVec2(size);
		auto offset = item_size * .5f;
		ImVec2 center = {point[0], point[1]};
		ImRect rect = {pos + center - offset, pos + center + offset};

		if (!ItemAdd(rect, id))
			return false;

		const bool hovered = ItemHoverable(rect, id, GImGui->LastItemData.ItemFlags);
		const bool clicked = hovered && IsMouseClicked(ImGuiMouseButton_Left, ImGuiInputFlags_None, id);

		if (clicked || GImGui->NavActivateId == id)
		{
			if (clicked)
				SetKeyOwner(ImGuiKey_MouseLeft, id);
			SetActiveID(id, window);
			SetFocusID(id, window);
			FocusWindow(window);
			GImGui->ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Up) | (1 << ImGuiDir_Down);
		}

		bool active = GImGui->ActiveId == id;

		if (GImGui->ActiveIdSource == ImGuiInputSource_Mouse && active)
		{
			if (!GImGui->IO.MouseDown[0])
			{
				ClearActiveID();
			}
		}

		float threshold = -1.0f;
		bool is_dragging = IsMouseDragging(ImGuiMouseButton_Left, threshold);
		if (active && hovered && is_dragging)
		{
			auto delta = GetMousePos() - pos;
			point[0] = delta.x;
			point[1] = delta.y;
			changed |= true;
		}

		if (changed)
			MarkItemEdited(id);

		ImU32 color = active ? 0xFF00FF00 : hovered ? 0xFF00FFFF : 0xFFFFFFFF;

		auto drawlist = window->DrawList;
		drawlist->AddRectFilled(rect.Min, rect.Max, color);

		return changed;
	}

	bool EditableRect(const char *str_id, float min[2], float max[2], float size)
	{
		bool changed = false;

		auto window = GImGui->CurrentWindow;
		if (window->SkipItems)
			return false;

		auto id = GetID(str_id);
		PushID(id);

		auto drawlist = window->DrawList;
		auto pos = window->DC.CursorPos;

		ImVec2 points[4] = {{min[0], min[1]}, {max[0], min[1]}, {max[0], max[1]}, {min[0], max[1]}};

		drawlist->AddLine(pos + points[0], pos + points[1], 0xFF0000FF);
		drawlist->AddLine(pos + points[1], pos + points[2], 0xFF0000FF);
		drawlist->AddLine(pos + points[2], pos + points[3], 0xFF0000FF);
		drawlist->AddLine(pos + points[3], pos + points[0], 0xFF0000FF);

		if (DraggablePoint("##min", min, size))
		{
			changed |= true;
		}

		if (DraggablePoint("##max", max, size))
		{
			changed |= true;
		}

		PopID();

		return changed;
	}

	void *OnDragDropTarget(const char *type)
	{
		if (BeginDragDropTarget())
		{
			if (const ImGuiPayload *payload = AcceptDragDropPayload(type))
			{
				return payload->Data;
			}

			EndDragDropTarget();
		}

		return nullptr;
	}

	bool Timeline(const char *str_id, int *frame, int max, const ImVec2 &size_arg)
	{
		IM_ASSERT(GImGui);

		auto &g = *GImGui;
		auto &style = g.Style;

		auto window = g.CurrentWindow;
		if (window->SkipItems)
			return false;

		const auto id = window->GetID(str_id);
		const auto cursor_pos = window->DC.CursorPos;
		const auto frame_padding = style.FramePadding;

		auto drawlist = window->DrawList;

		const auto w = GetContentRegionAvail().x;
		const auto h = 50.0f;

		const auto size = CalcItemSize(size_arg, w, h);
		const ImRect bb = {cursor_pos, cursor_pos + size};

		ItemSize(size);
		if (!ItemAdd(bb, id, &bb, ImGuiItemFlags_AllowOverlap))
			return false;

		auto background_color = ImVec4(.3f, .3f, .3f, 1.f);
		auto border_color = ImVec4(.5f, .5f, .5f, 1.f);
		auto divider_color = ImVec4(.5f, .5f, .5f, 1.f);

		drawlist->AddRectFilled(bb.Min, bb.Max, GetColorU32(background_color));
		drawlist->AddRect(bb.Min, bb.Max, GetColorU32(border_color));

		const auto divider_step = w / (float)max;

		for (int i = 1; i < max; i++)
		{
			auto x = bb.Min.x + divider_step * i;
			auto min = ImVec2(x, bb.Min.y);
			auto max = ImVec2(x, bb.Max.y);
			drawlist->AddLine(min, max, GetColorU32(divider_color));
		}

		// slider
		const float slider_size = 10.0f;
		const auto slider_bg_bb =
			ImRect({cursor_pos.x, bb.Max.y + frame_padding.y}, {cursor_pos.x + size.x, bb.Max.y + slider_size + frame_padding.y});

		auto slider_offset = (*frame * divider_step);
		const auto slider_bb =
			ImRect({slider_bg_bb.Min.x + slider_offset, slider_bg_bb.Min.y}, {slider_bg_bb.Min.x + slider_offset + divider_step, slider_bg_bb.Max.y});

		auto slider_id = window->GetID("##slider");

		drawlist->AddRectFilled(slider_bg_bb.Min, slider_bg_bb.Max, 0xFF000000);

		ItemSize(slider_bb);
		if (!ItemAdd(slider_bb, slider_id, nullptr, ImGuiItemFlags_NoNav | ImGuiItemFlags_AllowOverlap))
			return false;

		bool hovered = false;
		bool held = false;
		ButtonBehavior(slider_bb, slider_id, &hovered, &held, ImGuiButtonFlags_MouseButtonLeft);

		float threshold = -1.0f;
		bool is_dragging = IsMouseDragging(ImGuiMouseButton_Left, threshold);

		bool active = IsItemActive();

		bool changed = false;
		if (active && hovered && is_dragging)
		{
			int dir = GetMouseDragDelta().x > 0 ? 1 : -1;
			*frame = ImClamp(*frame + dir, 0, max - 1);

			changed |= true;
		}

		if (changed)
			MarkItemEdited(slider_id);

		auto slider_color = active ? ImVec4(1.f, 1.f, 1.f, 1.f) : hovered ? ImVec4(.7f, .7f, .7f, 1.f) : ImVec4(.5f, .5f, .5f, .7f);
		drawlist->AddRectFilled(slider_bb.Min, slider_bb.Max, GetColorU32(slider_color));

		return changed;
	}

	bool Timeline(const char *str_id, float *currentTime, float duration, float speed, const ImVec2 &size_arg)
	{
		IM_ASSERT(GImGui);

		auto &g = *GImGui;
		auto &style = g.Style;

		auto window = g.CurrentWindow;
		if (window->SkipItems)
			return false;

		const ImGuiID id = window->GetID(str_id);
		const ImGuiID marker_id = window->GetID((std::string(str_id) + "marker").c_str());

		const float width = GetContentRegionAvail().x;
		const ImVec2 size = CalcItemSize(size_arg, width - style.FramePadding.x * 2.0f, 50.0f - style.FramePadding.x * 2.0f);
		const auto cursor_pos = window->DC.CursorPos;
		const ImRect bb = {cursor_pos, cursor_pos + size};

		ItemSize(bb, style.FramePadding.y);
		if (!ItemAdd(bb, id, &bb, ImGuiItemFlags_AllowOverlap))
			return false;

		auto drawlist = window->DrawList;

		// background
		drawlist->AddRect(bb.Min, bb.Max, 0xFFFFFFFF, 0.0f, 0, 1.0f);
		drawlist->AddRectFilled(bb.Min, bb.Max, GetColorU32({.4f, .4f, .4f, .5f}));

		// marker
		float marker_width = 10.0f;
		ImVec2 marker_size = ImVec2{marker_width, bb.GetSize().y - style.FramePadding.y};

		float fraction = duration != 0 ? ((*currentTime / duration) * (bb.GetSize().x - marker_size.x)) : 1.0f;

		ImVec2 marker_pos = cursor_pos + ImVec2{fraction, style.FramePadding.y * .5f};
		ImRect marker_bb = {marker_pos, marker_pos + marker_size};
		ImRect button_bb = {marker_pos - 20.0f, marker_pos + 20.0f};

		ItemSize(button_bb, style.FramePadding.y);
		if (!ItemAdd(button_bb, marker_id))
			return false;

		bool hovered = false;
		bool held = false;

		ButtonBehavior(button_bb, marker_id, &hovered, &held, ImGuiButtonFlags_MouseButtonLeft);

		if (hovered)
			SetMouseCursor(ImGuiMouseCursor_ResizeEW);

		float threshold = -1.0f;
		bool is_dragging = IsMouseDragging(ImGuiMouseButton_Left, threshold);

		bool active = IsItemActive();
		bool changed = false;

		if (held)
		{

			if (active && is_dragging)
			{
				float dir = GetMouseDragDelta().x * speed;
				*currentTime = ImClamp(*currentTime + dir, 0.0f, duration);
				changed |= true;
			}
		}

		if (changed)
			MarkItemEdited(marker_id);

		ImVec4 color = active ? ImVec4(1.0f, 0.f, 0.f, 1.f) : hovered ? ImVec4(3.f, 0.f, .0f, .4f) : ImVec4(.6f, 0.f, 0.f, .4f);

		drawlist->AddRectFilled(marker_bb.Min, marker_bb.Max, GetColorU32(color));

		return changed;
	}

	ImRect GetItemRect()
	{
		return {GetItemRectMin(), GetItemRectMax()};
	}

	float GetLineHeight()
	{
		return GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	}

	bool DragTransform(
		const char *label, BHive::FTransform &transform, float speed, float min, float max, const char *format, ImGuiSliderFlags flags,
		const BHive::FTransform &reset_value)
	{
		bool changed = false;

		auto t = transform.get_translation();
		auto r = transform.get_rotation();
		auto s = transform.get_scale();

		ImGui::BeginGroup();

		if (DragVector("Translation", t, reset_value.get_translation(), speed, min, max, format, flags))
		{
			transform.set_translation(t);
			changed |= true;
		}

		if (DragVector("Rotation", r, reset_value.get_rotation(), speed, min, max, format, flags))
		{
			transform.set_rotation(r);
			changed |= true;
		}

		if (DragVector("Scale", s, reset_value.get_scale(), speed, min, max, format, flags))
		{
			transform.set_scale(s);
			changed |= true;
		}

		ImGui::EndGroup();

		return changed;
	}

	bool ColorEdit(const char *label, BHive::Color &color, ImGuiColorEditFlags flags)
	{
		return ImGui::ColorEdit4(label, &color.r, flags);
	}
} // namespace ImGui