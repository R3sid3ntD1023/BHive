#include "AudioFactory.h"
#include "audio/AudioImporter.h"
#include "audio/AudioSource.h"
#include "asset/AssetSerializer.h"

namespace BHive
{
    bool AudioFactory::Import(Ref<Asset> &asset, const std::filesystem::path &path)
    {
        AudioImporter importer;
        auto source = importer.Import(path);

        if (!source)
            return false;

        auto audio_meta_path = path.parent_path() / (path.stem().string() + ".meta");

        if (std::filesystem::exists(audio_meta_path))
        {
            // Deserialize Settings
            AssetSerializer::deserialize(*source, audio_meta_path);
        }
        else
        {
            // Serialize Settings
            AssetSerializer::serialize(*source, audio_meta_path);
        }

        asset = source;

        return source != nullptr ? true : false;
    }

    REFLECT_Factory(AudioFactory, AudioSource, ".ogg", ".wav")

} // namespace BHive
