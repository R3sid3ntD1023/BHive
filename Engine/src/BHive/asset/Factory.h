#pragma once

#include "core/Core.h"
#include "asset/Asset.h"
#include "core/EventDelegate.h"

namespace BHive
{
	DECLARE_EVENT(OnImportCompleted, const Ref<Asset> &);
	DECLARE_EVENT(OnAssetCreated, Ref<Asset>);

	class BHIVE_API Factory
	{
	public:
		virtual ~Factory() = default;

		virtual Ref<Asset> Import(const std::filesystem::path &path) { return nullptr; };

		virtual void Export(Ref<Asset> asset, const std::filesystem::path &path) {};

		virtual Ref<Asset> CreateNew() { return nullptr; };

		virtual bool CanCreateNew() const { return false; }

		virtual std::string GetDefaultAssetName() const { return ""; }

		virtual std::vector<Ref<Asset>> GetOtherCreatedAssets() { return {}; }

		virtual std::vector<std::string> GetSupportedExtensions() { return {}; }

		virtual std::string GetDisplayName() const { return "Factory"; }

		OnImportCompletedEvent OnImportCompleted;

		OnAssetCreatedEvent OnAssetCreated;

		REFLECTABLEV()
	};

	REFLECT(Factory)
	{
		BEGIN_REFLECT(Factory);
	}
} // namespace BHive

#define REFLECT_FACTORY(cls)                      \
	REFLECT(cls)                                  \
	{                                             \
		BEGIN_REFLECT(cls) REFLECT_CONSTRUCTOR(); \
	}