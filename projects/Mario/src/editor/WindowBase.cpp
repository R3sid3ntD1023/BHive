#include "core/Application.h"
#include "WindowBase.h"
#include "core/layers/ImGuiLayer.h"

namespace BHive
{
	WindowBase::WindowBase(uint32_t windowFlags)
		: mWindowFlags(windowFlags)
	{
	}

	WindowBase::~WindowBase()
	{
		mCounter.RemoveName(GetName(), mWindowID);
	}

	void WindowBase::OnUpdate()
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

			OnGuiRender();
		}

		ImGui::PopStyleVar(3);
		ImGui::End();
	}
} // namespace BHive