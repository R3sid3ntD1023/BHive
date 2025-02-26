#include "TextureFactory.h"
#include "importers/TextureImporter.h"

namespace BHive
{

	Ref<Asset> TextureFactory::Import(const std::filesystem::path &path)
	{
		auto t = TextureImporter::Import(path);
		OnImportCompleted.invoke(t);
		return t;
	}

	Ref<Asset> TextureFactory::Import(uint8_t *data, size_t size)
	{
		auto t = TextureImporter::LoadFromMemory(data, size);
		OnImportCompleted.invoke(t);
		return t;
	}

	REFLECT_FACTORY(TextureFactory)
} // namespace BHive
