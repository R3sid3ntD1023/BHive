#include "WaveImporter.h"
#include "audio/AudioSource.h"
#include <WaveParser.h>
#include <frames/v23/id3_Frame_TXXX.h>
#include <AL/al.h>

void WaveLoggerCallback(WAVE::Logger::LogLevel level, const char *message)
{
	switch (level)
	{
	case WAVE::Logger::info:
		LOG_INFO(message);
		break;
	case WAVE::Logger::trace:
		LOG_TRACE(message);
		break;
	case WAVE::Logger::warn:
		LOG_WARN(message);
		break;
	case WAVE::Logger::error:
		LOG_ERROR(message);
		break;
	default:
		break;
	}
}

namespace BHive
{
	Ref<AudioSource> WaveImporter::Import(const std::filesystem::path &path)
	{
		WAVE::Logger::SetCallback(WaveLoggerCallback);

		try
		{
			WAVE::Parser parser(path.string().c_str());
			WAVE::wave_t wave{};

			if (!parser.parse(wave))
			{
				return nullptr;
			}

			auto format = 0;
			switch (wave.fmt.num_channels)
			{
			case 1:
				format = AL_FORMAT_MONO16;
				break;
			case 2:
				format = AL_FORMAT_STEREO16;
				break;
			default:
				break;
			}

			AudioSpecification specification;
			if (wave.list.id3_chunk.has_tag("LOOP_START"))
			{
				auto value = wave.list.id3_chunk.get_tag<WAVE::id3_Frame_TXXX>("LOOP_START")->Value;
				specification.StartLoop = stoi(value);
			}
			if (wave.list.id3_chunk.has_tag("LOOP_END"))
			{
				auto value = wave.list.id3_chunk.get_tag<WAVE::id3_Frame_TXXX>("LOOP_END")->Value;
				specification.EndLoop = stoi(value);
			}

			return CreateRef<AudioSource>(format, wave.get_samples(), wave.get_buffer_size(), wave.fmt.sample_rate, wave.get_length(), specification);
		}
		catch (const std::runtime_error &e)
		{
			LOG_WARN("Exception: {}", e.what());
		}

		return nullptr;
	}
}