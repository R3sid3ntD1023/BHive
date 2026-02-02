#include "CrashHandler.h"
#include "Windows.h"
#include <csignal>

namespace BHive
{
	std::string s_app_name;
	std::string s_log_path;

	void log_system_info(std::ofstream &log)
	{
		log << "Application: " << s_app_name << "\n";
		log << "TimeStamp: " << std::chrono::system_clock::now() << "\n";

#ifdef _WIN32
		log << "OS: Windows" << "\n";
#endif // _WIN32

		log << "CPU Cores: " << std::thread::hardware_concurrency() << "\n";

#ifdef _WIN32
		MEMORYSTATUSEX mem_info;
		mem_info.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&mem_info);

		log << "Total Physical Memory: " << mem_info.ullTotalPhys / (1024 * 1024) << " MB \n";
		log << "Available Memory: " << mem_info.ullAvailPhys / (1024 * 1024) << " MB \n";
#endif
	}

	void signal_handler(int32_t signal)
	{
		try
		{
			std::ofstream log(s_log_path, std::ios::app);
			log << "=== Crash Report ==== \n";
			log_system_info(log);

			log << "Signal: " << signal << "(";
			switch (signal)
			{
			case SIGSEGV:
				log << "SIGSEGV - Segmentation fault\n";
				break;
			case SIGILL:
				log << "SIGILL - Illegal instruction\n";
				break;
			case SIGFPE:
				log << "SIGFPE - Floating point exception\n";
				break;
			case SIGABRT:
				log << "SIGABRT - Abort\n";
				break;
			default:
				log << "Unknown signal\n";
				break;
			}
			log << "=== End of Crash Report ===" << "\n\n";
			log.close();
		}
		catch (...)
		{
			LOG_ERROR("Failed to write crash log");
		}

		std::signal(signal, SIG_DFL);
		std::raise(signal);
	}

	void CrashHandler::Init(const std::string &app_name, const std::string &log_path, std::function<void(std::ofstream&)> log_info)
	{
		if (mInitialized)
			return;

		s_app_name = app_name;
		s_log_path = log_path;
		mLogInfo = log_info;

		signal(SIGSEGV, signal_handler);
		signal(SIGILL, signal_handler);
		signal(SIGFPE, signal_handler);
		signal(SIGABRT, signal_handler);

		mInitialized = true;
	}

	

	void CrashHandler::HandleException(const std::exception &e)
	{
		try
		{
			std::ofstream log(s_log_path, std::ios::app);
			log << "=== Crash Report ==== \n";
			log_system_info(log);
			if (mLogInfo)
				mLogInfo(log);

			log << "Exception: " << e.what() << "\n";
			log << "=== End of Crash Report ===" << "\n\n";
			log.close();
		}
		catch (...)
		{
			LOG_ERROR("Failed to write crash log");
		}
	}

	void CrashHandler::SetLogInfo(std::function<void(std::ofstream &)> log_info)
	{
		mLogInfo = log_info;
	}

	CrashHandler &BHive::CrashHandler::Get()
	{
		static CrashHandler handler;
		return handler;
	}
}
