#include "audio/AudioImporter.h"
#include "audio/AudioSource.h"
#include "AudioFactory.h"

namespace BHive
{
	Ref<Asset> AudioFactory::Import(const std::filesystem::path &path)
	{
		AudioImporter importer;
		auto source = importer.Import(path);

		OnImportCompleted.invoke(source);

		return source;
	}

	REFLECT_FACTORY(AudioFactory, AudioSource, ".ogg", ".wav")

} // namespace BHive
