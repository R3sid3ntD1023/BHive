#include "EditorContentBrowser.h"
#include "asset/AssetManager.h"
#include "asset/EditorAssetManager.h"
#include "asset/FactoryRegistry.h"
#include "gfx/Texture.h"
#include "asset/AssetContextMenuRegistry.h"
#include "asset/FAssetContextMenu.h"
#include "project/Project.h"
#include "asset/AssetFactory.h"


namespace BHive
{
    void FinishAssetImport(const std::filesystem::path& dir, const std::filesystem::path& rel, const Ref<Asset>& asset, const std::vector<Ref<Asset>>& others)
    {
		auto manager = AssetManager::GetAssetManager<EditorAssetManager>();
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

    EditorContentBrowser::EditorContentBrowser(const std::filesystem::path &directory)
        : ContentBrowserPanel(directory)
    {
    }

    void EditorContentBrowser::OnImportAsset(const std::filesystem::path &directory,
											 const std::filesystem::path &relative)
    {
		auto &registry = FactoryRegistry::Get();
        auto manager = AssetManager::GetAssetManager<EditorAssetManager>();
        auto type = registry.GetTypeFromExtension(relative.extension().string());
		auto factory = registry.Get(type);
        if (factory)
        {
			Ref<Asset> asset;
			factory->OnImportCompleted.bind(
				[=](const Ref<Asset> &asset) {
					FinishAssetImport(directory, relative, asset, factory->GetOtherCreatedAssets());
				});
            if (auto asset = factory->Import(relative))
            {
				
            }
        }
       
    }

    void EditorContentBrowser::OnDeleteAsset(const std::filesystem::path &relative)
    {
        auto manager = AssetManager::GetAssetManager<EditorAssetManager>();
        manager->RemoveAsset(Project::GetResourceDirectory() / relative);

        std::error_code error;
        std::filesystem::remove(Project::GetResourceDirectory() / relative, error);
        if (error)
        {
            LOG_ERROR("EditorContentBrower::DeleteAsset {}", error.message());
        }
    }

    void EditorContentBrowser::OnRenameAsset(const std::filesystem::path &relative_old, const std::filesystem::path &relative_new, bool directory)
    {
        auto old_path = Project::GetResourceDirectory() / relative_old;
        auto new_path = Project::GetResourceDirectory() / relative_new;

        if (std::filesystem::exists(new_path))
        {
            LOG_WARN("File with name \"{}\" already exists!", new_path.filename().string());
            return;
        }

        std::error_code error;
        std::filesystem::rename(old_path, new_path, error);

        if (error)
        {
            LOG_ERROR("EditorAssetManager RenameAsset({}, {}) ERROR: {}", old_path.string(), new_path.string(), error.message());
        }
        else
        {
            LOG_TRACE("EditorAssetManager RenameAsset({}, {})", old_path.string(), new_path.string());
            if (!directory)
            {
                auto manager = AssetManager::GetAssetManager<EditorAssetManager>();
                manager->RenameAsset(old_path, new_path);
            }
        }
    }

    void EditorContentBrowser::OnAssetContextMenu(const std::filesystem::path &relative)
    {
        auto type = FactoryRegistry::Get().GetTypeFromExtension(relative.extension().string());
        auto menu = AssetContextMenuRegistry::Get().GetAssetMenu(type);

        if (menu)
        {
            auto manager = AssetManager::GetAssetManager<EditorAssetManager>();
            auto handle = manager->GetHandle(Project::GetResourceDirectory() / relative);
            if (handle)
                menu->OnAssetContext(handle);
        }
    }

    void EditorContentBrowser::OnAssetDoubleClicked(const std::filesystem::path &relative)
    {
		auto manager = AssetManager::GetAssetManager<EditorAssetManager>();
		auto meta_data = manager->GetMetaData(Project::GetResourceDirectory() / relative);
        auto menu = AssetContextMenuRegistry::Get().GetAssetMenu(meta_data.Type);

        if (menu)
        {
            auto handle = manager->GetHandle(Project::GetResourceDirectory() / relative);
            if (handle)
                menu->OnAssetOpen(handle);
        }
    }

    bool EditorContentBrowser::IsAssetValid(const std::filesystem::path &relative) const
    {
        auto manager = AssetManager::GetAssetManager<EditorAssetManager>();
        auto handle = manager->GetHandle(Project::GetResourceDirectory() / relative);
        return manager->IsAssetHandleValid(handle);
    }

    void EditorContentBrowser::OnCreateAsset(const std::filesystem::path &directory,
											 const Ref<Factory> &factory)
    {
		auto manager = AssetManager::GetAssetManager<EditorAssetManager>();
        if (factory->CanCreateNew())
        {
			if (auto asset = factory->CreateNew())
            {
				asset->SetName(factory->GetDefaultAssetName());
				auto export_path = directory / (asset->GetName() + ".asset");

				AssetFactory asset_factory;
				asset_factory.Export(asset, export_path);

				manager->ImportAsset(export_path, asset->get_type(), asset->GetHandle());
            }
        }
    }

    Ref<Texture> EditorContentBrowser::OnGetIcon(bool directory, const std::filesystem::path &relative)
    {
        if (directory)
        {
            return mThumbnailCache.Get(EDITOR_DATA "/icons/DirectoryIcon.png");
        }

        Ref<Texture> texture;
        auto ext = relative.extension();
        if (auto info = FactoryRegistry::Get().GetTypeInfo<Texture>())
        {
            if (info.mExtensions.Contains(ext.string()))
                texture = mThumbnailCache.Get(Project::GetResourceDirectory() / relative);
        }

        if (!texture)
        {
            texture = mThumbnailCache.Get(EDITOR_DATA "/icons/FileIcon.png");
        }

        return texture;
    }

    bool EditorContentBrowser::GetDragDropData(AssetHandle &data, const std::filesystem::path &relative)
    {
        auto manager = AssetManager::GetAssetManager<EditorAssetManager>();
        data = manager->GetHandle(Project::GetResourceDirectory() / relative);
        return data != AssetHandle(0);
    }

    void EditorContentBrowser::OnWindowContextMenu()
    {
        auto &registry = FactoryRegistry::Get();
        auto &factories = registry.GetRegisteredFentityies();

        for (auto &[type, info] : factories)
        {
            auto &factory = info.mFactory;
            if (!factory->CanCreateNew())
                continue;

            auto name = std::string("Create ") + info.mName;

            if (ImGui::Selectable(name.c_str()))
            {
                OnCreateAsset(CurrentDirectory(), factory);
            }
        }
    }

} // namespace BHive
