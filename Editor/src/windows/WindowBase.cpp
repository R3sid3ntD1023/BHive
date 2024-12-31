#include "WindowBase.h"
#include "core/Application.h"

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

        ImGui::SetNextWindowSize({800, 600}, ImGuiCond_Appearing);
        if (ImGui::Begin(mWindowIDStr.c_str(), &mIsOpen, mWindowFlags))
        {
            mIsHovered = ImGui::IsWindowHovered();
            mIsFocused = ImGui::IsWindowFocused();

            Application::Get().GetImGuiLayer().BlockEvents(!mIsHovered);

            OnUpdateContent();
        }

        ImGui::End();
    }
}