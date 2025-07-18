#include "VorbisImporter.h"
#include "audio/AudioSource.h"
#pragma warning(push, 0)
#include <stb_vorbis.c>
#pragma warning(pop)
#include <AL/al.h>

namespace BHive
{
    Ref<AudioSource> VorbisImporter::Import(const std::filesystem::path &path)
    {
        int error = 0;
        int channels = 0;
        int sample_rate = 0;
        short *data = nullptr;

        stb_vorbis *f = stb_vorbis_open_filename(path.string().c_str(), &error, nullptr);
        stb_vorbis_info info = stb_vorbis_get_info(f);

        int samples = stb_vorbis_decode_filename(path.string().c_str(), &channels, &sample_rate, &data);
        int buffersize = 2 * channels * samples;

        if (!data || buffersize == 0)
            return nullptr;

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
            return nullptr;
        }

        FAudioSpecification specs{.mFormat = format, .mNumSamples = samples, .mSampleRate = sample_rate};
		auto source = CreateRef<AudioSource>(data, buffersize, specs);

        LOG_TRACE("{} length:{}, time:{}", path.string(), source->GetLengthSeconds(), source->GetLength().to_string());

        free(data);

        return source;
    }
}