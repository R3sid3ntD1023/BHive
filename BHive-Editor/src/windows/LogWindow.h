#pragma once

#include <spdlog/details/log_msg.h>
#include "ImWindowBase.h"

namespace BHive
{

	class ImLogWindow : public ImWindowBase
	{
		struct LogItem
		{
			spdlog::level::level_enum mLevel;
			std::string mMessage;
		};

	public:
		ImLogWindow();

	private:
		static void OnMessageLogged(const spdlog::details::log_msg &msg);

	protected:
		void OnUpdateContent() override;

		virtual const char *GetName() const override { return "Log"; }

	private:
		bool mAutoScroll{true};
		size_t mMaxMessages{1024};

		static inline bool sScroll{true};
		static inline std::vector<LogItem> sMessages;
	};
} // namespace BHive
