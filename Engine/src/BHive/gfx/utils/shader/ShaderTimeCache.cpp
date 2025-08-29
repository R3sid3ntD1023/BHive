#include "ShaderTimeCache.h"
#include "ShaderUtils.h"

namespace BHive
{
	ShaderTimeCache::ShaderTimeCache()
	{
		size_t count = 0;
		auto file = ShaderUtils::GetCacheDirectory() / "ShaderWriteTimes.txt";
		char token;

		if (std::filesystem::exists(file))
		{
			std::ifstream in(file, std::ios::in);
			in >> count;
			for (size_t i = 0; i < count; i++)
			{
				std::string path;
				long long time;

				in >> path >> token;
				in >> time;

				mModifiedTimes.emplace(path, time);
			}
		}

		std::filesystem::recursive_directory_iterator it(ENGINE_SHADER_PATH);
		for (const auto &entry : it)
		{
			auto modified_time = std::chrono::duration_cast<std::chrono::seconds>(std::filesystem::last_write_time(entry).time_since_epoch()).count();
			mLastTimes.emplace(entry, modified_time);
		}
	}

	ShaderTimeCache::~ShaderTimeCache()
	{
		auto file = ShaderUtils::GetCacheDirectory() / "ShaderWriteTimes.txt";
		std::ofstream out(file, std::ios::out);
		if (!out)
			return;

		auto count = mLastTimes.size();
		out << count << '\n';

		for (const auto &[path, time] : mLastTimes)
		{
			auto path_str = path.string();

			out << path_str << " : " << time;
			out << '\n';
		}
	}

	bool ShaderTimeCache::WasFileModified(const std::filesystem::path &file)
	{
		return mModifiedTimes[file] != mLastTimes[file];
	}
} // namespace BHive