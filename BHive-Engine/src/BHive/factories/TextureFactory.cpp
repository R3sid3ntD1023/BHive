#include "TextureFactory.h"
#include "gfx/Texture.h"
#include "importers/TextureImporter.h"

namespace BHive
{
	Ref<Asset> TextureFactory::Import(const std::filesystem::path &filename)
	{
		auto decoded = TextureImporter::FromFile(filename);
		return CreateRef<Texture2D>(decoded.Size, decoded.CreateInfo, decoded.Data);
	}

	rttr::type TextureFactory::GetAssetType() const
	{
		return rttr::type::get<Texture2D>();
	}
} // namespace BHive