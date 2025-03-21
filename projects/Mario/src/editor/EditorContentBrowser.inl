#include "asset/AssetContextMenuRegistry.h"
#include "asset/AssetFactory.h"
#include "asset/AssetManager.h"
#include "asset/EditorAssetManager.h"
#include "asset/FactoryRegistry.h"
#include "asset/FAssetContextMenu.h"
#include "EditorContentBrowser.h"
#include "gfx/textures/Texture2D.h"
#include "project/Project.h"

namespace BHive
{
	template <typename T>
	void FinishAssetImport(
		const std::filesystem::path &dir, const std::filesystem::path &rel, const Ref<Asset> &asset,
		const std::vector<Ref<Asset>> &others)
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
		: ContentBrowserPanel(directory)
	{
	}

	template <typename T>
	void
	EditorContentBrowser<T>::OnImportAsset(const std::filesystem::path &directory, const std::filesystem::path &relative)
	{
		auto &registry = FactoryRegistry::Get();
		auto manager = AssetManager::GetAssetManager<T>();
		auto factory = registry.Get(relative.extension().string());
		if (factory)
		{
			factory->OnImportCompleted.bind(
				[=](const Ref<Asset> &asset)
				{ FinishAssetImport<T>(directory, relative, asset, factory->GetOtherCreatedAssets()); });
			factory->Import(directory / relative);
		}
	}

	template <typename T>
	void EditorContentBrowser<T>::OnDeleteAsset(const std::filesystem::path &relative)
	{
		auto manager = AssetManager::GetAssetManager<T>();
		manager->RemoveAsset(relative);

		std::error_code error;
		std::filesystem::remove(Project::GetResourceDirectory() / relative, error);
		if (error)
		{
			LOG_ERROR("EditorContentBrower::DeleteAsset {}", error.message());
		}
	}

	template <typename T>
	void EditorContentBrowser<T>::OnRenameAsset(
		const std::filesystem::path &relative_old, const std::filesystem::path &relative_new, bool directory)
	{
		auto old_path = Project::GetResourceDirectory() / relative_old;
		auto new_path = Project::GetResourceDirectory() / relative_new;

		if (std::filesystem::exists(new_path) && !std::filesystem::is_directory(new_path))
		{
			LOG_WARN("File with name \"{}\" already exists!", new_path.filename().string());
			return;
		}

		std::error_code error;

		std::filesystem::rename(old_path, new_path, error);

		if (error)
		{
			LOG_ERROR(
				"EditorAssetManager RenameAsset({}, {}) ERROR: {}", old_path.string(), new_path.string(), error.message());
		}
		else
		{
			LOG_TRACE("EditorAssetManager RenameAsset({}, {})", old_path.string(), new_path.string());
			if (!directory)
			{
				auto manager = AssetManager::GetAssetManager<T>();
				manager->RenameAsset(relative_old, relative_new);
			}
		}
	}

	template <typename T>
	void EditorContentBrowser<T>::OnReimportAsset(const std::filesystem::path &relative)
	{
		auto path = Project::GetResourceDirectory() / relative;

		Ref<Asset> asset;
		AssetFactory factory;
		if (factory.Import(asset, path))
		{
			auto manager = AssetManager::GetAssetManager<T>();
			manager->ImportAsset(path, asset->get_type(), asset->GetHandle());
		}
	}

	template <typename T>
	void EditorContentBrowser<T>::OnAssetContextMenu(const std::filesystem::path &relative)
	{
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
	void EditorContentBrowser<T>::OnAssetDoubleClicked(const std::filesystem::path &relative)
	{
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
	bool EditorContentBrowser<T>::IsAssetValid(const std::filesystem::path &relative) const
	{
		auto manager = AssetManager::GetAssetManager<T>();
		auto handle = manager->GetHandle(relative);
		return manager->IsAssetHandleValid(handle);
	}

	template <typename T>
	void EditorContentBrowser<T>::OnCreateAsset(const std::filesystem::path &directory, const Ref<Factory> &factory)
	{

		if (factory->CanCreateNew())
		{
			factory->OnAssetCreated.bind([=](Ref<Asset> asset)
										 { FinishCreateAsset<T>(factory->GetDefaultAssetName(), directory, asset); });
			factory->CreateNew();
		}
	}

	template <typename T>
	Ref<Texture2D> EditorContentBrowser<T>::OnGetIcon(bool directory, const std::filesystem::path &relative)
	{
		if (directory)
		{
			return mThumbnailCache.Get(RESOURCE_PATH "/icons/DirectoryIcon.png");
		}

		Ref<Texture2D> texture;
		auto ext = relative.extension();

		if (ext == ".png" || ext == ".jpg" || ext == ".jpeg")
		{
			texture = mThumbnailCache.Get(Project::GetResourceDirectory() / relative);
		}

		if (!texture)
		{
			texture = mThumbnailCache.Get(RESOURCE_PATH "/icons/FileIcon.png");
		}

		return texture;
	}

	template <typename T>
	bool EditorContentBrowser<T>::GetDragDropData(UUID &data, const std::filesystem::path &relative)
	{
		auto manager = AssetManager::GetAssetManager<T>();
		data = manager->GetHandle(relative);
		return data != UUID::Null;
	}

	template <typename T>
	void EditorContentBrowser<T>::OnWindowContextMenu()
	{
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
