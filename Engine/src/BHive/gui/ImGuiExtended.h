#pragma once

#include "gfx/Color.h"
#include "gfx/Texture.h"
#include "GUICore.h"
#include "math/Transform.h"
#include <string>

namespace ImGui
{
	void
	Image(const BHive::Texture *texture, const ImVec2 size = {0, 0}, const ImVec4 &tint_col = {1, 1, 1, 1}, const ImVec4 &border_col = {0, 0, 0, 0});

	bool DrawIcon(const std::string &label, BHive::Texture *icon, float size, ImGuiButtonFlags flags = ImGuiButtonFlags_PressedOnDoubleClick);

	bool DrawEditableText(const char *str_id, const std::string &label, std::string &editable_text);

	void *OnDragDropTarget(const char *type);

	// returns the payload
	void OnDragDropSource(const char *type, void *data, size_t size);

	// vec2
	bool DraggablePoint(const char *str_id, float point[2], float size = 1.0f);

	// min-max vec2
	bool EditableRect(const char *str_id, float min[2], float max[2], float size = 1.0f);

	bool Timeline(const char *str_id, int *frame, int max, const ImVec2 &size_arg = {0, 0});

	bool Timeline(const char *str_id, float *currentTime, float duration, float speed, const ImVec2 &size_arg = {0, 0});

	ImRect GetItemRect();

	float GetLineHeight();

	bool DragTransform(
		const char *label, BHive::FTransform &transform, float speed = 1.0f, float min = 0.0f, float max = 0.0f, const char *format = "%.2f",
		ImGuiSliderFlags flags = 0, const BHive::FTransform &reset_value = {});

	bool ColorEdit(const char *label, BHive::Color &color, ImGuiColorEditFlags flags = 0);
} // namespace ImGui

// templates
namespace ImGui
{
	template <glm::length_t L, typename T, glm::qualifier Q>
	bool DragVector(
		const char *label, glm::vec<L, T, Q> &data, const glm::vec<L, T, Q> &resetValue = {}, float speed = 1.0f, float min = 0.0f, float max = 0.0f,
		const char *format = "%.2f", ImGuiSliderFlags flags = 0)
	{
		static const char *labels[4] = {"x", "y", "z", "w"};
		static const uint32_t colors[4] = {0xFF0000FF, 0xFF00FF00, 0xFFFF0000, 0xFF00AAFF};

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = {lineHeight, lineHeight};

		bool changed = false;

		ImGui::PushID(label);
		ImGui::BeginColumns(label, 2);

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(label);
		ImGui::SameLine();

		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(L, ImGui::GetContentRegionAvail().x - (buttonSize.x * 3));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

		for (int32_t i = 0; i < L; i++)
		{
			ImGui::PushID(i);
			ImGui::PushStyleColor(ImGuiCol_Button, colors[i]);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, colors[i]);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colors[i] * (255 / 2));
			if (ImGui::Button(labels[i]))
			{
				data[i] = resetValue[i];
				changed |= true;
			}
			ImGui::PopStyleColor(3);

			ImGui::SameLine();

			changed |= ImGui::DragFloat("##", &data[i], speed, min, max, format, flags | ImGuiInputTextFlags_EnterReturnsTrue);
			if (ImGui::IsItemDeactivatedAfterEdit())
			{
				changed |= true;
			}
			ImGui::PopID();
			ImGui::PopItemWidth();

			if (i < L - 1)
				ImGui::SameLine();
		}
		ImGui::PopStyleVar();
		ImGui::EndColumns();
		ImGui::PopID();

		return changed;
	}

} // namespace ImGui

#define DEBUG_DRAW_RECT(color)                                 \
	{                                                          \
		auto drawlist = ImGui::GetWindowDrawList();            \
		auto rect = ImGui::GetItemRect();                      \
		drawlist->AddRect(rect.Min, rect.Max, color, 0, 0, 5); \
	}

#define DEBUG_DRAW_WINDOW_RECT(color)                                                              \
	{                                                                                              \
		auto drawlist = ImGui::GetWindowDrawList();                                                \
		auto rect = ImRect(ImGui::GetWindowPos(), ImGui::GetWindowPos() + ImGui::GetWindowSize()); \
		drawlist->AddRect(rect.Min, rect.Max, color, 0, 0, 5);                                     \
	}