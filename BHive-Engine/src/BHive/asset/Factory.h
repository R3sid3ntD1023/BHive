#pragma once

#include "asset/Asset.h"
#include "core/Core.h"
#include "core/delegates/EventDelegate.h"

namespace BHive
{
	class Texture;

	DECLARE_EVENT(OnImportCompleted, const Ref<Asset> &);
	DECLARE_EVENT(OnAssetCreated, Ref<Asset>);

	class BHIVE_API Factory
	{
	public:
		virtual ~Factory() = default;

		virtual Ref<Asset> Import(const std::filesystem::path &path) { return nullptr; };

		// virtual void Export(Ref<Asset> asset, const std::filesystem::path &path) {};

		// virtual Ref<Asset> Reimport(Ref<Asset> asset, const std::filesystem::path &path) { return nullptr; };

		// virtual bool ValidateImport(const std::filesystem::path &path, std::string &err) { return true; };

		virtual Ref<Asset> CreateNew() { return nullptr; };

		virtual bool CanCreateNew() const { return false; }

		// virtual bool CanReimport() const { return false; }

		virtual std::vector<Ref<Asset>> GetOtherCreatedAssets() { return {}; }

		virtual Ref<Texture> GenerateThumbnail(Ref<Asset> asset) { return nullptr; }

		virtual std::string GetDefaultAssetName() const { return "New_" + GetDisplayName(); }

		virtual rttr::type GetAssetType() const = 0;

		virtual uint8_t GetCategory() const = 0;

		virtual std::string GetDisplayName() const = 0;

		OnImportCompletedEvent OnImportCompleted;

		OnAssetCreatedEvent OnAssetCreated;

		REFLECTABLEV()
	};

	REFLECT_INLINE(Factory)
	{
		BEGIN_REFLECT(Factory);
	}
} // namespace BHive

#define REFLECT_FACTORY(cls)                      \
	REFLECT(cls)                                  \
	{                                             \
		BEGIN_REFLECT(cls) REFLECT_CONSTRUCTOR(); \
	}