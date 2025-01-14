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

		auto var = type.create();
		
		if (!var)
		{
			LOG_ERROR("No valid reflected default constructor found");
			return false;
		}

		auto obj = var.get_value<Ref<Asset>>();
		obj->Deserialize(ar);

		asset = obj;

		return true;
	}

	bool AssetFactory::Export(const Ref<Asset>& asset, const std::filesystem::path& path)
	{
		if (!asset)
			return false;

		FileStreamWriter ar(path);
		ar(asset->get_type());
		asset->Serialize(ar);

		return true;
	}

	bool AssetFactory::Export(const Asset* asset, const std::filesystem::path& path)
	{
		if (!asset)
			return false;

		FileStreamWriter ar(path);
		ar(asset->get_type());
		asset->Serialize(ar);

		return true;
	}

	REFLECT_Factory(AssetFactory, Asset, ".asset")
}