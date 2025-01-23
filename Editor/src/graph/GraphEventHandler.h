#pragma once

#include "gui/ImGuiExtended.h"

namespace BHive
{
	struct GraphEventHandler
	{
		virtual void handle() {}
	};

	struct MouseDragHandler : public GraphEventHandler
	{
		ImU32 mDragRectColor{IM_COL32(180, 200, 255, 255)};

		void handle() override
		{
			bool hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows);
			mIsMouseDragging = hovered && ImGui::IsMouseDown(ImGuiMouseButton_Left);

			if (mIsMouseDragging && !mHasDragStarted)
			{
				mStartDragPos = ImGui::GetMousePos();
				mHasDragStarted = true;
			}

			if (mHasDragStarted && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			{
				mStartDragPos = {};
				mHasDragStarted = false;
			}

			if (!mHasDragStarted)
				return;

			auto min = ImMin(mStartDragPos, ImGui::GetMousePos());
			auto max = ImMax(mStartDragPos, ImGui::GetMousePos());
			mDragRect = ImRect(min, max);

			auto draw_list = ImGui::GetWindowDrawList();
			draw_list->AddRect(min, max, mDragRectColor);
		}

		bool isDragActive() const { return mHasDragStarted; }

		bool isOverlapped(const ImRect &rect) const
		{
			if (!mHasDragStarted)
				return false;

			return mDragRect.Overlaps(rect);
		}

	private:
		bool mIsMouseDragging{false};
		bool mHasDragStarted{false};
		ImVec2 mStartDragPos{};
		ImRect mDragRect{};
	};
} // namespace BHive