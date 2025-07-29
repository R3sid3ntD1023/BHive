#include "Log.h"
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/callback_sink.h>

namespace BHive
{
	void Log::Init()
	{
		std::vector<spdlog::sink_ptr> sinks(3);
		sinks[0] = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		sinks[1] = std::make_shared<spdlog::sinks::basic_file_sink_mt>("BHive_Log.log", true);
		sinks[2] = std::make_shared<spdlog::sinks::callback_sink_mt>(
			[&](const auto &msg)
			{
				if (OnMessageLogged)
					OnMessageLogged(msg);
			});

		sinks[0]->set_pattern("%^[%T] %n: %v%$");
		sinks[1]->set_pattern("[%T] [%l] %n: %v");
		sinks[2]->set_pattern("%^[%T] %n: %v%$");

		sLogger = std::make_shared<spdlog::logger>("BHive", sinks.begin(), sinks.end());
		spdlog::register_logger(sLogger);

		sLogger->set_level(spdlog::level::trace);
		sLogger->flush_on(spdlog::level::trace);
	}
} // namespace BHive