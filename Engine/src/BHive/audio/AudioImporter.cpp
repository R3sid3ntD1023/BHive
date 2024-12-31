#include "AudioImporter.h"
#include "AudioSource.h"
#include "VorbisImporter.h"
#include "WaveImporter.h"

namespace BHive
{
    Ref<AudioSource> AudioImporter::Import(const std::filesystem::path& path)
    {
        Ref<AudioSource> asset;
        auto ext = path.extension().string();
        if (ext == ".ogg")
        {
            asset = VorbisImporter::Import(path);
        }
        
        if (ext == ".wav")
        {
            asset = WaveImporter::Import(path);
        }

        return asset;
    }

   
}