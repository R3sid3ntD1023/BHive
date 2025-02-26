#include "ImProfiler.h"
#include <imgui_internal.h>

namespace ImGui::ImProfiler
{
	void DrawBar(const char *str_id, float value, const ImVec4 &color, const ImVec2 &size_arg)
	{
		auto &g = *GImGui;
		auto window = GetCurrentWindow();
		if (window->SkipItems)
			return;

		char label[256];
		sprintf(label, "%.2f ms", value);
		if (value > 1000)
			sprintf(label, "%.2f s", value / 1000);
		auto label_size = CalcTextSize(label);
		auto pos = GetCursorScreenPos();
		auto bb = ImRect(pos, pos + size_arg);
		auto id = GetID(str_id);

		ItemSize(bb.GetSize(), label_size.y + g.Style.FramePadding.y);
		if (!ItemAdd(bb, id))
			return;

		auto drawlist = window->DrawList;
		drawlist->AddRectFilled(bb.Min, bb.Max, GetColorU32(color));
		drawlist->AddText(bb.GetCenter() - label_size * .5f, IM_COL32_BLACK, label);
	}

	void DrawTimeline(const char *str_id, const ImVec2 &size_arg, float stride)
	{
		auto &g = *GImGui;
		auto window = GetCurrentWindow();
		if (window->SkipItems)
			return;

		auto pos = GetCursorScreenPos();
		auto bb = ImRect(pos, pos + size_arg);
		auto id = GetID(str_id);

		ItemSize(bb.GetSize(), g.Style.FramePadding.y);
		if (!ItemAdd(bb, id))
			return;
	}
} // namespace ImGui::ImProfiler
