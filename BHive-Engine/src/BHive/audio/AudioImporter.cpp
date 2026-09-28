#include "AudioImporter.h"
#include "AudioSource.h"
#include <AL/al.h>
#include <WaveParser.h>
#include <frames/v23/id3_Frame_TXXX.h>
#include <stb_vorbis.c>

namespace BHive
{
	void WaveLoggerCallback(WAVE::Logger::LogLevel level, const char *message)
	{
		switch (level)
		{
		case WAVE::Logger::info:
			// LOG_INFO(message);
			break;
		case WAVE::Logger::trace:
			// LOG_TRACE(message);
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

	DecodedAudio ImportVorbis(const std::filesystem::path &path)
	{
		int error = 0;
		int channels = 0;
		int sample_rate = 0;
		short *data = nullptr;

		stb_vorbis *f = stb_vorbis_open_filename(path.string().c_str(), &error, nullptr);
		if (!f)
			return {};

		stb_vorbis_info info = stb_vorbis_get_info(f);

		int samples = stb_vorbis_decode_filename(path.string().c_str(), &channels, &sample_rate, &data);
		int buffersize = 2 * channels * samples;

		if (!data || buffersize == 0)
			return {};

		buffersize = buffersize - buffersize % 4;
		// buffersize = 2 * channels * sample_rate;

		int format = -1;
		if (channels == 1)
		{
			format = AL_FORMAT_MONO16;
		}
		else if (channels == 2)
		{
			format = AL_FORMAT_STEREO16;
		}

		if (alGetError() != AL_NO_ERROR)
		{
			LOG_ERROR("failed to setup sound source");
			return {};
		}

		FAudioSpecification specification{};
		specification.mFormat = format;
		specification.mNumSamples = samples;
		specification.mSampleRate = sample_rate;

		DecodedAudio decoded{};
		decoded.Specification = specification;
		decoded.Data.Allocate(data, buffersize);

		free(data);

		return decoded;
	}

	DecodedAudio ImportWave(const std::filesystem::path &path)
	{
		WAVE::Logger::SetCallback(WaveLoggerCallback);

		try
		{
			WAVE::Parser parser(path.string().c_str());
			WAVE::wave_t wave{};

			if (!parser.parse(wave))
			{
				return {};
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

			FAudioSpecification specification;
			specification.mFormat = format;
			specification.mNumSamples = wave.get_num_samples_per_channel();
			specification.mSampleRate = wave.fmt.sample_rate;

			if (wave.list.id3_chunk.has_tag("LOOP_START"))
			{
				auto value = wave.list.id3_chunk.get_tag<WAVE::id3_Frame_TXXX>("LOOP_START")->Value;
				specification.mStartLoop = stoi(value);
			}
			if (wave.list.id3_chunk.has_tag("LOOP_END"))
			{
				auto value = wave.list.id3_chunk.get_tag<WAVE::id3_Frame_TXXX>("LOOP_END")->Value;
				specification.mEndLoop = stoi(value);
			}

			DecodedAudio decoded{};
			decoded.Specification = specification;
			decoded.Data.Allocate(wave.get_samples(), (size_t)wave.get_buffer_size());

			return decoded;
		}
		catch (const std::runtime_error &e)
		{
			LOG_WARN("Exception: {}", e.what());
		}

		return {};
	}

	DecodedAudio AudioImporter::Import(const std::filesystem::path &path)
	{
		auto ext = path.extension().string();
		if (ext == ".ogg")
		{
			return ImportVorbis(path);
		}
		else if (ext == ".wav")
		{
			return ImportWave(path);
		}

		return {};
	}

} // namespace BHive