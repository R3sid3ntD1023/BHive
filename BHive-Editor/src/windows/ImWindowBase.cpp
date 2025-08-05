#include "core/Application.h"
#include "ImWindowBase.h"
#include "core/layers/ImGuiLayer.h"

namespace BHive
{
	ImWindowBase::ImWindowBase(uint32_t windowFlags)
		: mWindowFlags(windowFlags)
	{
	}

	ImWindowBase::~ImWindowBase()
	{
		mCounter.RemoveName(GetName(), mWindowID);
	}

	void ImWindowBase::OnUpdate()
	{
		if (!mIsOpen)
			return;

		if (mWindowIDStr.empty())
			mWindowIDStr = mCounter.GetName(GetName(), mWindowID);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 2.f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, {.5f, 0.f});

		ImGui::SetNextWindowSize({800, 600}, ImGuiCond_Appearing);
		if (ImGui::Begin(mWindowIDStr.c_str(), &mIsOpen, mWindowFlags))
		{
			mIsHovered = ImGui::IsWindowHovered();
			mIsFocused = ImGui::IsWindowFocused();

			Application::Get().GetImGuiLayer().BlockEvents(!mIsHovered);

			if (ImGui::BeginMenuBar())
			{
				OnMenuBar();

				ImGui::EndMenuBar();
			}

			OnUpdateContent();
		}

		ImGui::PopStyleVar(3);
		ImGui::End();
	}

} // namespace BHive