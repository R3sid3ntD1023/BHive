#include "LogPanel.h"
#include "gui/ImGuiExtended.h"

namespace BHive
{
	const std::unordered_map<spdlog::level::level_enum, ImVec4> sLogColors = {
		{spdlog::level::trace, {1, 1, 1, 1}},
		{spdlog::level::info, {0, 1, 0, 1}},
		{spdlog::level::warn, {1, 1, 0, 1}},
		{spdlog::level::err, {1, 0, 0, 1}},
		{spdlog::level::critical, {1, 0, 0, 1}}};

	void LogPanel::OnGuiRender()
	{
		if (ImGui::BeginMenuBar())
		{
			ImGui::Checkbox("AutoScroll", &mAutoScroll);

			ImGui::EndMenuBar();
		}

		size_t start = sMessages.size() > mMaxMessages ? sMessages.size() - mMaxMessages : 0;
		for (size_t i = start; i < sMessages.size(); i++)
		{
			const auto msg = sMessages[i];
			const auto color = sLogColors.at(msg.mLevel);

			ImGui::TextColored(color, msg.mMessage.c_str());
		}

		if (sScroll && mAutoScroll)
		{
			ImGui::SetScrollHereY();
			sScroll = false;
		}
	}

	LogPanel::LogPanel()
		: WindowBase(ImGuiWindowFlags_MenuBar)
	{
		if (!Log::OnMessageLogged)
		{
			Log::OnMessageLogged = [](const spdlog::details::log_msg &msg) { LogPanel::LogMessage(msg); };
		}
	}

	void LogPanel::LogMessage(const spdlog::details::log_msg &msg)
	{
		sMessages.push_back({msg.level, std::string(msg.payload.data(), msg.payload.size())});
		sScroll = true;
	}

} // namespace BHive
