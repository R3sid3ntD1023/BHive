#pragma once

#include <spdlog/details/log_msg.h>
#include "WindowBase.h"

namespace BHive
{
	struct LogItem
	{
		spdlog::level::level_enum mLevel;
		std::string mMessage;
	};

	class LogPanel : public WindowBase
	{
	public:
		LogPanel();

		static void LogMessage(const spdlog::details::log_msg &msg);

	protected:
		void OnGuiRender() override;

		virtual const char *GetName() const { return "Log"; }

	private:
		bool mAutoScroll{true};
		size_t mMaxMessages{1024};

		static inline bool sScroll{true};
		static inline std::vector<LogItem> sMessages;
	};
} // namespace BHive
