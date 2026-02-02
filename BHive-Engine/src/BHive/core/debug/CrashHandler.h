#pragma once

#include "core/Core.h"

namespace BHive
{
	class BHIVE_API CrashHandler
	{
	public:
		virtual ~CrashHandler() = default;

		void Init(const std::string &app_name, const std::string &log_path, std::function<void(std::ofstream &)> log_info = nullptr);

		void HandleException(const std::exception &e);

		void SetLogInfo(std::function<void(std::ofstream &)> log_info);

		static CrashHandler &Get();

	private:
		std::function<void(std::ofstream &)> mLogInfo;
		bool mInitialized = false;
	};

} // namespace BHive