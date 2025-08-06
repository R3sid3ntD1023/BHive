#include "asset/AssetContextMenuRegistry.h"
#include "asset/AssetFactory.h"
#include "asset/AssetManager.h"
#include "asset/EditorAssetManager.h"
#include "asset/FactoryRegistry.h"
#include "asset/FAssetContextMenu.h"
#include "EditorContentBrowser.h"
#include "gfx/textures/Texture2D.h"
#include "project/Project.h"
#include "core/subsystem/subsystem.h"
#include "ThumbnailCache.h"
#include "importers/TextureImporter.h"

namespace BHive
{
	template <typename T>
	void FinishAssetImport(const std::filesystem::path &dir, const std::filesystem::path &rel, const Ref<Asset> &asset, const std::vector<Ref<Asset>> &others)
	{
		auto manager = AssetManager::GetAssetManager<T>();
		auto export_path = dir / (rel.stem().string() + ".asset");
		AssetFactory asset_factory;
		asset_factory.Export(asset, export_path);

		manager->ImportAsset(export_path, asset->get_type(), asset->GetHandle());

		for (auto &other : others)
		{
			export_path = dir / (other->GetName() + ".asset");
			AssetFactory asset_factory;
			asset_factory.Export(other, export_path);
			manager->ImportAsset(export_path, other->get_type(), other->GetHandle());
		}
	}

	template <typename T>
	void FinishCreateAsset(const std::string &name, const std::filesystem::path &path, Ref<Asset> asset)
	{
		asset->SetName(name);
		auto export_path = path / (asset->GetName() + ".asset");

		AssetFactory asset_factory;
		asset_factory.Export(asset, export_path);

		auto manager = AssetManager::GetAssetManager<T>();
		manager->ImportAsset(export_path, asset->get_type(), asset->GetHandle());
	}

	template <typename T>
	EditorContentBrowser<T>::EditorContentBrowser<T>(const std::filesystem::path &directory)
		: ImContentBrowserWindow(directory)
	{
	}

	template <typename T>
	void EditorContentBrowser<T>::OnImportAsset(const std::filesystem::path &directory, const std::filesystem::path &relative)
	{
		auto &registry = FactoryRegistry::Get();
		auto manager = AssetManager::GetAssetManager<T>();
		auto factory = registry.Get(relative.extension().string());
		if (factory)
		{
			factory->OnImportCompleted.bind([=](const Ref<Asset> &asset) { FinishAssetImport<T>(directory, relative, asset, factory->GetOtherCreatedAssets()); });
			factory->Import(directory / relative);
		}
	}

	template <typename T>
	void EditorContentBrowser<T>::OnDeleteAsset(const std::filesystem::path &path)
	{
		auto relative = std::filesystem::relative(path, Project::GetResourceDirectory());
		auto manager = AssetManager::GetAssetManager<T>();
		manager->RemoveAsset(relative);

		std::error_code error;
		std::filesystem::remove(path, error);
		if (error)
		{
			LOG_ERROR("EditorContentBrower::DeleteAsset {}", error.message());
		}
	}

	template <typename T>
	void EditorContentBrowser<T>::RenameAsset(const std::filesystem::path &_old, const std::filesystem::path &_new, const std::filesystem::recursive_directory_iterator &it)
	{
		auto relative_old = std::filesystem::relative(_old, Project::GetResourceDirectory());
		auto relative_new = std::filesystem::relative(_new, Project::GetResourceDirectory());

		if (auto manager = AssetManager::GetAssetManager<T>())
		{
			manager->RenameAsset(relative_old, relative_new);

			for (auto &entry : it)
			{
				auto entry_relative = std::filesystem::relative(entry, Project::GetResourceDirectory());
				auto new_relative = std::filesystem::relative(_new / entry.path().filename(), Project::GetResourceDirectory());

				manager->RenameAsset(entry_relative, new_relative);
			}
		}
	}

	template <typename T>
	void EditorContentBrowser<T>::OnRenameAsset(const std::filesystem::path &old_path, const std::filesystem::path &new_path)
	{
		if (std::filesystem::exists(new_path))
		{
			return;
		}

		std::filesystem::recursive_directory_iterator old_entries;
		if (std::filesystem::is_directory(old_path))
			old_entries = std::filesystem::recursive_directory_iterator(old_path);

		std::error_code error;
		std::filesystem::rename(old_path, new_path, error);

		if (error)
		{
			LOG_ERROR("EditorAssetManager RenameAsset({}, {}) ERROR: {}", old_path.string(), new_path.string(), error.message());
			return;
		}

		RenameAsset(old_path, new_path, old_entries);

		LOG_TRACE("Renamed Asset({}, {})", old_path.string(), new_path.string());
	}

	template <typename T>
	void EditorContentBrowser<T>::OnReimportAsset(const std::filesystem::path &path)
	{
		Ref<Asset> asset;
		AssetFactory factory;
		if (factory.Import(asset, path))
		{
			auto manager = AssetManager::GetAssetManager<T>();
			manager->ImportAsset(path, asset->get_type(), asset->GetHandle());
		}
	}

	template <typename T>
	void EditorContentBrowser<T>::OnAssetContextMenu(const std::filesystem::path &path)
	{
		auto relative = std::filesystem::relative(path, Project::GetResourceDirectory());
		auto manager = AssetManager::GetAssetManager<T>();
		auto meta_data = manager->GetMetaData(relative);
		auto menu = AssetContextMenuRegistry::Get().GetAssetMenu(meta_data.Type);

		if (menu && manager)
		{
			auto handle = manager->GetHandle(relative);
			if (handle)
				menu->OnAssetContext(handle);
		}
	}

	template <typename T>
	void EditorContentBrowser<T>::OnAssetDoubleClicked(const std::filesystem::path &path)
	{
		auto relative = std::filesystem::relative(path, Project::GetResourceDirectory());
		auto manager = AssetManager::GetAssetManager<T>();
		auto meta_data = manager->GetMetaData(relative);
		auto menu = AssetContextMenuRegistry::Get().GetAssetMenu(meta_data.Type);

		if (menu)
		{
			auto handle = manager->GetHandle(relative);
			if (handle)
				menu->OnAssetOpen(handle);
		}
	}

	template <typename T>
	bool EditorContentBrowser<T>::IsAssetValid(const std::filesystem::path &path) const
	{
		auto relative = std::filesystem::relative(path, Project::GetResourceDirectory());
		auto manager = AssetManager::GetAssetManager<T>();
		auto handle = manager->GetHandle(relative);
		return manager->IsAssetHandleValid(handle);
	}

	template <typename T>
	void EditorContentBrowser<T>::OnCreateAsset(const std::filesystem::path &directory, const Ref<Factory> &factory)
	{

		if (factory->CanCreateNew())
		{
			factory->OnAssetCreated.bind([=](Ref<Asset> asset) { FinishCreateAsset<T>(factory->GetDefaultAssetName(), directory, asset); });
			factory->CreateNew();
		}
	}

	template <typename T>
	Ref<Texture2D> EditorContentBrowser<T>::OnGetIcon(const std::filesystem::path &path)
	{
		auto &thumbnail_cache = GetSubSystem<ThumbnailCache>();
		Ref<Texture2D> texture;

		auto asset_manager = AssetManager::GetAssetManager<EditorAssetManager>();
		if (asset_manager)
		{
			auto relative = std::filesystem::relative(path, Project::GetResourceDirectory());
			auto handle = asset_manager->GetHandle(relative);
			if (auto type = AssetManager::GetAssetType(handle))
			{
				texture = thumbnail_cache.GetAssetIcon(type.get_name().data());
			}
		}

		if (!texture)
		{
			if (std::filesystem::is_directory(path))
			{
				bool is_empty = std::filesystem::is_empty(path);
				return thumbnail_cache.Get(is_empty ? "FolderIconEmpty" : "FolderIcon");
			}

			auto ext = path.extension().string();

			if (ext == ".png" || ext == ".jpg" || ext == ".jpeg")
			{
				texture = thumbnail_cache.Get(path);
			}

			if (!texture)
				texture = thumbnail_cache.Get("Invalid");
		}

		return texture;
	}

	template <typename T>
	void EditorContentBrowser<T>::OnCreateAssetMenu()
	{
		ImContentBrowserWindow::OnCreateAssetMenu();

		auto &registry = FactoryRegistry::Get();
		auto &factories = registry.GetRegisteredFactories();

		for (auto &factory : factories)
		{
			if (!factory->CanCreateNew())
				continue;

			auto name = std::format("Create {}", factory->GetDisplayName());

			if (ImGui::Selectable(name.c_str()))
			{
				OnCreateAsset(CurrentDirectory(), factory);
			}
		}
	}

} // namespace BHive
