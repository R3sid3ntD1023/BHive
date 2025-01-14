#include "AudioFactory.h"
#include "audio/AudioImporter.h"
#include "audio/AudioSource.h"

namespace BHive
{
	Ref<Asset> AudioFactory::Import(const std::filesystem::path &path)
	{
		AudioImporter importer;
		auto source = importer.Import(path);

		OnImportCompleted.invoke(source);

		return source;
	}


    REFLECT_Factory(AudioFactory, AudioSource, ".ogg", ".wav")

} // namespace BHive
