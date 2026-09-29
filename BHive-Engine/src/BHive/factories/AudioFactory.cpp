#include "AudioFactory.h"
#include "audio/AudioImporter.h"
#include "audio/AudioSource.h"

namespace BHive
{
	Ref<Asset> AudioFactory::Import(const std::filesystem::path &path)
	{
		auto decoded = AudioImporter::Import(path);
		return CreateRef<AudioSource>(decoded.Data, decoded.Specification);
	}

	rttr::type AudioFactory::GetAssetType() const
	{
		return rttr::type::get<AudioSource>();
	}
} // namespace BHive