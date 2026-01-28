#pragma once

#include "core/Core.h"
#include "GUICore.h"
#include "core/Math/Transform.h"

namespace BHive
{
	struct FColor;
	class Texture;
} // namespace BHive

namespace ImGui
{
	BHIVE_API void Image(const BHive::Texture *texture, const ImVec2 size = {0, 0}, const ImVec4 &tint_col = {1, 1, 1, 1}, const ImVec4 &border_col = {0, 0, 0, 0});

	BHIVE_API bool DrawIcon(const std::string &label, BHive::Texture *icon, float size, ImGuiButtonFlags flags = ImGuiButtonFlags_PressedOnDoubleClick);

	BHIVE_API bool DrawEditableText(const std::string &label, std::string &editable_text);

	BHIVE_API void *OnDragDropTarget(const char *type);

	// returns the payload
	BHIVE_API void OnDragDropSource(const char *type, void *data, size_t size);

	// vec2
	BHIVE_API bool DraggablePoint(const char *str_id, float point[2], float size = 1.0f);

	// min-max vec2
	BHIVE_API bool EditableRect(const char *str_id, float min[2], float max[2], float size = 1.0f, float thickness = 2.f);

	BHIVE_API bool Timeline(const char *str_id, int *frame, int max, const ImVec2 &size_arg = {0, 0});

	BHIVE_API bool Timeline(const char *str_id, float *currentTime, float duration, float speed, const ImVec2 &size_arg = {0, 0});

	BHIVE_API ImRect GetItemRect();

	BHIVE_API float GetLineHeight();

	BHIVE_API bool ColorEdit(const char *label, BHive::FColor &color, ImGuiColorEditFlags flags = 0);

} // namespace ImGui

#define DEBUG_DRAW_RECT(color)                                 \
	{                                                          \
		auto drawlist = ImGui::GetWindowDrawList();            \
		auto rect = ImGui::GetItemRect();                      \
		drawlist->AddRect(rect.Min, rect.Max, color, 0, 0, 1); \
	}

#define DEBUG_DRAW_WINDOW_RECT(color)                                                              \
	{                                                                                              \
		auto drawlist = ImGui::GetWindowDrawList();                                                \
		auto rect = ImRect(ImGui::GetWindowPos(), ImGui::GetWindowPos() + ImGui::GetWindowSize()); \
		drawlist->AddRect(rect.Min, rect.Max, color, 0, 0, 5);                                     \
	}