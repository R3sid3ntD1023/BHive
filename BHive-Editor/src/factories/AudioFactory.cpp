#include "audio/AudioImporter.h"
#include "audio/AudioSource.h"
#include "AudioFactory.h"

namespace BHive
{
	Ref<Asset> AudioFactory::Import(const std::filesystem::path &path)
	{
		AudioImporter importer;
		auto source = importer.Import(path);
		source->SetName(path.stem().string());

		OnImportCompleted.invoke(source);

		return source;
	}

	REFLECT_FACTORY(AudioFactory)

} // namespace BHive
