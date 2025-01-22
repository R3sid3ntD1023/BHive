#include "GraphContext.h"

namespace BHive
{
	void AppendDrawData(ImDrawList *, const ImVec2 &origin, float scale);
	void CopyIOEvents(ImGuiContext *dst, ImGuiContext *src, const ImVec2 &origin, float scale);

	FGraphContext::~FGraphContext()
	{
		if (mCtx)
			ImGui::DestroyContext(mCtx);
	}

	void FGraphContext::begin()
	{

		ImGui::PushID(this);
		ImGui::PushStyleColor(ImGuiCol_ChildBg, mConfig.mColor);
		ImGui::BeginChild("viewport", mConfig.mSize, 0, ImGuiWindowFlags_NoMove);
		ImGui::PopStyleColor();

		mSize = ImGui::GetContentRegionAvail();
		mOrigin = ImGui::GetCursorScreenPos();

		ImGui::PushClipRect(mOrigin, mOrigin + mSize, true);
		auto canvas_clip_rect = ImGui::GetWindowDrawList()->CmdBuffer.back().ClipRect;
		ImGui::PopClipRect();

		mOriginalCtx = ImGui::GetCurrentContext();
		const auto &orginal_style = ImGui::GetStyle();
		if (!mCtx)
		{
			mCtx = ImGui::CreateContext(ImGui::GetIO().Fonts);
		}

		ImGui::SetCurrentContext(mCtx);
		auto &new_style = ImGui::GetStyle();
		new_style = orginal_style;

		CopyIOEvents(mOriginalCtx, mCtx, mOrigin, mScale);

		ImGui::GetIO().DisplaySize = mSize / mScale;
		ImGui::GetIO().ConfigInputTrickleEventQueue = false;

		ImGui::NewFrame();

		ImGui::SetNextWindowPos({});
		ImGui::SetNextWindowSize(ImGui::GetMainViewport()->WorkSize);
		ImGui::Begin(
			"viewport-container", nullptr,
			ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
				ImGuiWindowFlags_NoScrollWithMouse);

		// transform cliprect to local grid space
		canvas_clip_rect = (canvas_clip_rect - ImVec4(mOrigin.x, mOrigin.y, mOrigin.x, mOrigin.y)) / mScale;
		ImGui::PushClipRect({canvas_clip_rect.x, canvas_clip_rect.y}, {canvas_clip_rect.z, canvas_clip_rect.w}, false);
	}

	void FGraphContext::end()
	{
		ImGui::PopClipRect();

		ImGui::End();
		bool is_any_item_hovered = ImGui::IsAnyItemHovered();

		ImGui::Render();

		auto draw_data = ImGui::GetDrawData();

		ImGui::SetCurrentContext(mOriginalCtx);
		mOriginalCtx = nullptr;

		for (int i = 0; i < draw_data->CmdListsCount; i++)
			AppendDrawData(draw_data->CmdLists[i], mOrigin, mScale);

		ImGui::InvisibleButton("Canvas", mSize);
		bool hovered = ImGui::IsItemHovered() && !is_any_item_hovered;

		auto mouse_pos = ImGui::GetMousePos();
		auto mouse_wheel = ImGui::GetIO().MouseWheel;
		auto &scale = mScale;
		auto &scroll = mScroll;
		auto pos = mOrigin;
		auto smoothness = mConfig.mZoomSmoothness;

		if (mouse_wheel != 0.f && hovered)
		{
			mScaleTarget += mouse_wheel / mConfig.mZoomDivisions;
			mScaleTarget = ImClamp(mScaleTarget, mConfig.mMinZoom, mConfig.mMaxZoom);

			if (smoothness == 0.f)
			{
				scroll += (mouse_pos - pos) / mScaleTarget - (mouse_pos - pos) / scale;
				scale = mScaleTarget;
			}
		}

		if (abs(mScaleTarget - scale) >= 0.015f / smoothness)
		{
			float cs = (mScaleTarget - scale) / smoothness;
			scroll += (mouse_pos - pos) / (scale + cs) - (mouse_pos - pos) / scale;
			scale += (mScaleTarget - scale) / smoothness;

			if (abs(mScaleTarget - scale) < 0.015f / smoothness)
			{
				scroll += (mouse_pos - pos) / mScaleTarget - (mouse_pos - pos) / scale;
				scale = mScaleTarget;
			}
		}

		if (ImGui::IsKeyPressed(ImGuiKey_F, false))
			mScaleTarget = mConfig.mDefaultZoom;

		// scrolling
		if (ImGui::IsMouseDown(ImGuiMouseButton_Middle) && hovered && !is_any_item_hovered)
		{
			mScroll += ImGui::GetIO().MouseDelta / mScale;
		}

		ImGui::EndChild();

		ImGui::PopID();
	}

	void AppendDrawData(ImDrawList *src, const ImVec2 &origin, float scale)
	{
		auto dl = ImGui::GetWindowDrawList();
		auto vert_size = dl->VtxBuffer.size();
		auto idx_size = dl->IdxBuffer.size();
		auto cmd_size = dl->CmdBuffer.size();

		auto vert_src = src->VtxBuffer;
		auto idx_src = src->IdxBuffer;
		auto &cmd_src = src->CmdBuffer;

		dl->VtxBuffer.resize(vert_size + vert_src.size());
		dl->IdxBuffer.resize(idx_size + idx_src.size());
		dl->CmdBuffer.reserve(cmd_size + cmd_src.size());

		dl->_VtxWritePtr = dl->VtxBuffer.Data + vert_size;
		dl->_IdxWritePtr = dl->IdxBuffer.Data + idx_size;

		const auto &vert_src_read = vert_src.Data;
		const auto &idx_src_read = idx_src.Data;

		for (int i = 0; i < vert_src.size(); i++)
		{
			dl->_VtxWritePtr->pos = vert_src_read[i].pos * scale + origin;
			dl->_VtxWritePtr->col = vert_src_read[i].col;
			dl->_VtxWritePtr->uv = vert_src_read[i].uv;
			dl->_VtxWritePtr++;
			dl->_VtxCurrentIdx++;
		}

		for (int i = 0; i < idx_src.size(); i++)
		{
			*dl->_IdxWritePtr = idx_src_read[i] + vert_size;
			dl->_IdxWritePtr++;
		}

		for (auto &cmd : cmd_src)
		{
			cmd.IdxOffset += idx_size;
			IM_ASSERT(cmd.VtxOffset == 0);
			cmd.ClipRect = (cmd.ClipRect * scale + ImVec4(origin.x, origin.y, origin.x, origin.y));
			dl->CmdBuffer.push_back(cmd);
		}
	}

	void CopyIOEvents(ImGuiContext *src, ImGuiContext *dst, const ImVec2 &origin, float scale)
	{
		dst->InputEventsQueue = src->InputEventsTrail;
		for (auto &e : dst->InputEventsQueue)
		{
			if (e.Type == ImGuiInputEventType_MousePos)
			{
				e.MousePos.PosX = (e.MousePos.PosX - origin.x) / scale;
				e.MousePos.PosY = (e.MousePos.PosY - origin.y) / scale;
			}
		}
	}
} // namespace BHive