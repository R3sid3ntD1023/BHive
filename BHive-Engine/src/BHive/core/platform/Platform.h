#pragma once

#include "core/Core.h"

namespace BHive
{
	struct BHIVE_API Platform
	{
		struct FileInfo
		{
			std::filesystem::path Path{};

			operator bool() const { return !Path.empty(); }

			std::string AsString() const { return Path.string(); }

			std::string FileNameWithExtension() const { return Path.filename().string(); }

			std::string FileName() const { return Path.stem().string(); }

			operator const std::filesystem::path &() const { return Path; }
		};

		static void ExecuteProcess(const char *process, char *args);

		static FileInfo OpenFile(const char *filter, const char *title = "Open File");

		static FileInfo SaveFile(const char *filter, const char *title = "Save File");

		static FileInfo GetDirectory();

		static bool MoveToRecycleBin(const std::filesystem::path &path);

		static void *GetNativeWindow();

		static void *GetAPIContext();
	};
} // namespace BHive