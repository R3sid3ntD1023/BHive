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

    REFLECT_Factory(TextureFactory, Texture, ".png", ".jpg", ".jpeg") 
} // namespace BHive
