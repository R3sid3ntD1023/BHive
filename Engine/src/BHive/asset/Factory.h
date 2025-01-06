#pragma once

#include "asset/Asset.h"
#include "AssetExtensions.h"

namespace BHive
{
	class BHIVE Factory
	{
	public:
		virtual ~Factory() = default;

		virtual bool Import(Ref<Asset> &asset, const std::filesystem::path &path) { return false; };

		virtual void Export(Ref<Asset> asset, const std::filesystem::path &path) {};

		virtual void CreateNew(const std::filesystem::path &path) {};

		virtual bool CanCreateNew() const { return false; }

		virtual const char *GetFileFilters() const { return ""; }

		virtual const char *GetDefaultAssetName() const { return ""; }

		virtual const std::vector<Ref<Asset>> &GetOtherCreatedAssets() const { return {}; }

		REFLECTABLEV()
	};

	REFLECT(Factory)
	{
		BEGIN_REFLECT(Factory);
	}
}

#define REFLECT_Factory(cls, asset_type, ...)                                                                           \
	REFLECT(cls)                                                                                                        \
	{                                                                                                                       \
		BEGIN_REFLECT(cls)                                                                                              \
		(META_DATA("Type", rttr::type::get<asset_type>()), META_DATA("Extensions", BHive::FAssetExtensions({__VA_ARGS__}))) \
			REFLECT_CONSTRUCTOR()                                                                                           \
				CONSTRUCTOR_POLICY_SHARED;                                                                                  \
	}