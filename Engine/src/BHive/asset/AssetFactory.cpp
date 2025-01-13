#include "AssetFactory.h"
#include "serialization/Serialization.h"

namespace BHive
{
	bool AssetFactory::Import(Ref<Asset>& asset, const std::filesystem::path& path)
	{
		std::ifstream in(path, std::ios::in);
		if (!in)
			return false;
		
		cereal::JSONInputArchive ar(in);

		AssetType type = InvalidType;
		ar(MAKE_NVP("AssetType", type));

		if (!type)
			return false;

		auto var = type.create();
		
		if (!var)
		{
			LOG_ERROR("No valid reflected default constructor found");
			return false;
		}

		auto obj = var.get_value<Ref<Asset>>();
		obj->Load(ar);

		asset = obj;

		return true;
	}

	bool AssetFactory::Export(const Ref<Asset>& asset, const std::filesystem::path& path)
	{
		return Export(asset.get(), path);
	}

	bool AssetFactory::Export(const Asset* asset, const std::filesystem::path& path)
	{
		if (!asset)
			return false;

		std::ofstream out(path, std::ios::out);

		if (!out)
			return false;

		cereal::JSONOutputArchive ar(out);
		ar(MAKE_NVP("AssetType", asset->get_type()));
		asset->Save(ar);

		return true;
	}

	REFLECT_Factory(AssetFactory, Asset, ".asset")
}