#include "AudioImporter.h"
#include "AudioSource.h"
#include <AL/al.h>
#include <WaveParser.h>
#include <frames/v23/id3_Frame_TXXX.h>
#include <stb_vorbis.c>

namespace BHive
{

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
		waveparser::Parser parser(path.string().c_str());
		waveparser::Wave wave{};

		if (!parser.Parse(wave))
		{
			return {};
		}

		if (wave.GetAudioFormat() != 1)
		{
			LOG_ERROR("unsupported WAV encoding: expected PCM");
			return {};
		}

		ALenum format = 0;
		switch (wave.Fmt.BitsPerSample)
		{
		case 8:
			switch (wave.GetNumChannels())
			{
			case 1:
				format = AL_FORMAT_MONO8;
				break;
			case 2:
				format = AL_FORMAT_STEREO8;
				break;
			default:
				LOG_ERROR("unsupported WAV channel count: {}", wave.GetNumChannels());
				return {};
			}
			break;
		case 16:
			switch (wave.GetNumChannels())
			{
			case 1:
				format = AL_FORMAT_MONO16;
				break;
			case 2:
				format = AL_FORMAT_STEREO16;
				break;
			default:
				LOG_ERROR("unsupported WAV channel count: {}", wave.GetNumChannels());
				return {};
			}
			break;
		default:
			LOG_ERROR("unsupported WAV bit depth: {}", wave.Fmt.BitsPerSample);
			return {};
		}

		FAudioSpecification specification{};
		specification.mFormat = format;
		specification.mNumSamples = wave.GetNumSamples();
		specification.mSampleRate = wave.GetSampleRate();

		auto frameCount = wave.GetNumSamplesPerChannel();

		auto loopStartTag = wave.Id3Chunk.GetTXXXByDescription("LOOP_START");
		auto loopEndTag = wave.Id3Chunk.GetTXXXByDescription("LOOP_END");

		if (loopStartTag.size())
		{
			specification.mStartLoop = std::stoi(loopStartTag[0]->GetValue());
		}
		if (loopEndTag.size())
		{
			specification.mEndLoop = std::stoi(loopEndTag[0]->GetValue());
		}
		if (specification.mStartLoop && specification.mEndLoop)
		{
			const auto loopStart = *specification.mStartLoop;
			const auto loopEnd = *specification.mEndLoop;
			if (loopStart < 0 || loopEnd <= loopStart || static_cast<size_t>(loopEnd) > frameCount)
			{
				LOG_ERROR("invalid WAV loop points: {} to {} for {} frames", loopStart, loopEnd, frameCount);
				return {};
			}
		}

		DecodedAudio decoded{};
		decoded.Specification = specification;
		auto &data = wave.GetData();
		const short *dataPtr = reinterpret_cast<const short *>(data.data());
		decoded.Data.Allocate(dataPtr, data.size());

		return decoded;
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