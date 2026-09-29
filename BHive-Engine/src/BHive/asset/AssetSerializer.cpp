#include "AssetSerializer.h"

namespace BHive
{
	Ref<Asset> AssetSerializer::Import(const std::filesystem::path &path)
	{

		try
		{
			std::ifstream in(path, std::ios::in | std::ios::binary);

			cereal::BinaryInputArchive ar(in);

			rttr::type type = InvalidType;
			ar(type);

			if (!type || !type.get_constructor())
			{
				LOG_ERROR("AssetFactory::Import() no default constructor found for type {} for {}", type, path);
				return nullptr;
			}

			auto var = type.create();
			if (!var.is_valid())
			{
				LOG_ERROR("AssetFactory::Import() failed to create instance of type", type);
				return nullptr;
			}

			auto asset = var.get_value<Ref<Asset>>();
			asset->Load(ar);

			LOG_TRACE("AssetFactory::Import() Imported asset from {}", path);

			return asset;
		}
		catch (std::exception &e)
		{
			LOG_ERROR("AssetFactory::Import() Exception - {}", e.what());
		}

		return nullptr;
	}

	bool AssetSerializer::Export(const Ref<Asset> &asset, const std::filesystem::path &path)
	{
		if (!asset)
			return false;

		try
		{
			auto parent_directory = path.parent_path();
			if (!std::filesystem::exists(parent_directory))
			{
				std::filesystem::create_directory(parent_directory);
			}

			std::ofstream out(path, std::ios::out | std::ios::binary);
			cereal::BinaryOutputArchive ar(out);

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
} // namespace BHive