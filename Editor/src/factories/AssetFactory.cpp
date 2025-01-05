#include "AssetFactory.h"
#include "serialization/Serialization.h"

namespace BHive
{
	bool AssetFactory::Import(Ref<Asset>& asset, const std::filesystem::path& path)
	{
		FileStreamReader ar(path);
		
		AssetType type = InvalidType;
		ar(type);

		if (!type)
			return false;

		auto obj = type.create().get_value<Ref<Asset>>();
		obj->Deserialize(ar);

		return true;
	}

	bool AssetFactory::Export(const Ref<Asset>& asset, const std::filesystem::path& path)
	{
		FileStreamWriter ar(path);
		ar(asset->GetType());
		asset->Serialize(ar);

		return true;
	}

	REFLECT_FACTORY(AssetFactory, Asset, ".asset")
}