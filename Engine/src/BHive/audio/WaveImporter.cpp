#include "WaveImporter.h"
#include "audio/AudioSource.h"
#include <WaveParser.h>
#include <frames/v23/id3_Frame_TXXX.h>
#include <AL/al.h>

namespace BHive
{
	Ref<AudioSource> WaveImporter::Import(const std::filesystem::path &path)
	{
		try
		{
			waveparser::Parser parser(path);
			waveparser::Wave wave{};

			if (!parser.Parse(wave))
			{
				return nullptr;
			}

			auto format = 0;
			switch (wave.GetNumChannels())
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
			specification.mNumSamples = wave.GetNumSamplesPerChannel();
			specification.mSampleRate = wave.GetSampleRate();

			auto start = wave.Id3Chunk.GetTXXXByDescription("LOOP_START");
			auto end = wave.Id3Chunk.GetTXXXByDescription("LOOP_END");

			if (start.size())
			{
				auto value = start[0]->GetValue();
				specification.mStartLoop = stoi(value);
			}
			if (end.size())
			{
				auto value = end[0]->GetValue();
				specification.mEndLoop = stoi(value);
			}

			auto &data = wave.GetData();
			return CreateRef<AudioSource>(reinterpret_cast<int16_t *>(data.data()), int(data.size()), specification);
		}
		catch (const std::runtime_error &e)
		{
			LOG_WARN("Exception: {}", e.what());
		}

		return nullptr;
	}
} // namespace BHive