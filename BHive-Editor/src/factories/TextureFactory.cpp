#include "TextureFactory.h"
#include "importers/TextureImporter.h"

namespace BHive
{

	Ref<Asset> TextureFactory::Import(const std::filesystem::path &path)
	{
		auto t = TextureLoader::Import(path);
		OnImportCompleted.invoke(t);
		return t;
	}

	Ref<Asset> TextureFactory::Import(const uint8_t *data, size_t size)
	{
		auto t = TextureLoader::LoadFromMemory(data, size);
		OnImportCompleted.invoke(t);
		return t;
	}

	REFLECT_FACTORY(TextureFactory)
} // namespace BHive
