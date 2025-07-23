#include "AssetFactory.h"

namespace BHive
{
	bool AssetFactory::Import(Ref<Asset> &asset, const std::filesystem::path &path)
	{

		try
		{
			std::ifstream in(path, std::ios::in | std::ios::binary);

			cereal::BinaryInputArchive ar(in);

			rttr::type type = InvalidType;
			ar(type);

			if (!type.get_constructor())
			{
				LOG_ERROR("AssetFactory::Import() no default constructor found for type", type);
				return false;
			}

			auto var = type.create();
			auto obj = var.get_value<Ref<Asset>>();
			obj->Load(ar);

			asset = obj;

			LOG_TRACE("AssetFactory::Import() Imported asset from {}", path);

			return true;
		}
		catch (std::exception &e)
		{
			LOG_ERROR("AssetFactory::Import() Exception - {}", e.what());
		}

		return false;
	}

	bool AssetFactory::Export(const Ref<Asset> &asset, const std::filesystem::path &path)
	{
		return Export(asset.get(), path);
	}

	bool AssetFactory::Export(const Asset *asset, const std::filesystem::path &path)
	{
		if (!asset)
			return false;

		std::ofstream out(path, std::ios::out | std::ios::binary);
		cereal::BinaryOutputArchive ar(out);

		try
		{
			auto parent_directory = path.parent_path();
			if (!std::filesystem::exists(parent_directory))
			{
				std::filesystem::create_directory(parent_directory);
			}

			ar(asset->get_type());
			asset->Save(ar);

			LOG_TRACE("AssetFactory::Export() Exported asset to {}", path);
			return true;
		}
		catch (const std::exception &e)
		{
			LOG_ERROR("AssetFactory::Export() Exception - {}", e.what());
		}

		return false;
	}

	REFLECT_FACTORY(AssetFactory)
} // namespace BHive