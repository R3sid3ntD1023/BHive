#include "ShaderTimeCache.h"
#include "ShaderUtils.h"

namespace BHive
{
	ShaderTimeCache::ShaderTimeCache()
	{
		std::filesystem::recursive_directory_iterator it(ENGINE_SHADER_PATH);
		for (const auto &entry : it)
		{
			auto modified_time = std::chrono::duration_cast<std::chrono::seconds>(std::filesystem::last_write_time(entry).time_since_epoch()).count();
			mTimes.emplace(entry, FileTime{.Modified = modified_time});
		}

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
				int64_t last_time = 0;

				in >> path >> token;
				in >> last_time;

				mTimes[path].Last = last_time;
			}
		}
	}

	ShaderTimeCache::~ShaderTimeCache()
	{
		auto file = ShaderUtils::GetCacheDirectory() / "ShaderWriteTimes.txt";
		std::ofstream out(file, std::ios::out);
		if (!out)
			return;

		auto count = mTimes.size();
		out << count << '\n';

		for (const auto &[path, time] : mTimes)
		{
			auto path_str = path.string();

			out << path_str << " : " << time.Last;
			out << '\n';
		}
	}

	bool ShaderTimeCache::WasFileModified(const std::filesystem::path &file, FileTime *time)
	{
		if (!mTimes.contains(file))
			return false;

		if (time)
			*time = mTimes[file];

		return !time;
	}
} // namespace BHive