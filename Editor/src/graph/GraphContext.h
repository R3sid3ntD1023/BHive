#pragma once

#include "gui/ImGuiExtended.h"

namespace BHive
{
	struct FContextConfig
	{
		ImVec2 mSize{};
		ImU32 mColor{IM_COL32_WHITE};
		std::string mDragDropName{"NODE"};

		float mDefaultZoom{1.f};
		float mZoomDivisions{10.f};
		float mMaxZoom{2.f};
		float mMinZoom{.3f};
		float mZoomSmoothness = 5.0f;
	};

	struct FGraphContext
	{

		~FGraphContext();

		FContextConfig mConfig;

		ImVec2 mScroll{};
		ImVec2 mSize{};
		ImVec2 mOrigin{};
		float mScale{mConfig.mDefaultZoom};

		void begin();
		void end();
		ImGuiContext *GetRawContext() { return mCtx; }

	private:
		float mScaleTarget{mConfig.mDefaultZoom};
		ImGuiContext *mOriginalCtx{nullptr};
		ImGuiContext *mCtx{nullptr};
		bool mAnyWindowHovered{false};
		bool mAnyItemActive{false};
	};
} // namespace BHive