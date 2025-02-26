#pragma once

#include <imgui.h>

namespace ImGui::ImProfiler
{
	void DrawBar(const char *str_id, float value, const ImVec4 &color, const ImVec2 &size = {0, 0});

	void DrawTimeline(const char *str_id, const ImVec2 &size, float stride = 2.f);
} // namespace ImGui::ImProfiler
