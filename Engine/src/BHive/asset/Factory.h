#pragma once

#include "asset/Asset.h"
#include "AssetExtensions.h"

namespace BHive
{
	DECLARE_EVENT(OnImportCompleted, const Ref<Asset>&);

	class BHIVE Factory
	{
	public:
		virtual ~Factory() = default;

		virtual Ref<Asset> Import(const std::filesystem::path &path) { return nullptr; };

		virtual void Export(Ref<Asset> asset, const std::filesystem::path &path) {};

		virtual Ref<Asset> CreateNew()
		{
			return nullptr;
		};

		virtual bool CanCreateNew() const { return false; }

		virtual const char *GetFileFilters() const { return ""; }

		virtual const char *GetDefaultAssetName() const { return ""; }

		virtual const std::vector<Ref<Asset>>& GetOtherCreatedAssets()
		{
			return {};
		}

		OnImportCompletedEvent OnImportCompleted;

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