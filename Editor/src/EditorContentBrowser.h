#pragma once

#include "ContentBrowser.h"
#include "ThumbnailCache.h"

namespace BHive
{
    class Factory;

    class EditorContentBrowser : public ContentBrowserPanel
    {
    private:
        /* data */
    public:
        EditorContentBrowser() = default;
        EditorContentBrowser(const std::filesystem::path &directory);

        virtual void OnImportAsset(const std::filesystem::path & directory, const std::filesystem::path &relative);
        virtual void OnDeleteAsset(const std::filesystem::path &relative);
        virtual void OnRenameAsset(const std::filesystem::path &relative_old, const std::filesystem::path &relative_new, bool directory);
        virtual void OnAssetContextMenu(const std::filesystem::path &relative);
        virtual void OnAssetDoubleClicked(const std::filesystem::path &relative);
        virtual bool IsAssetValid(const std::filesystem::path &relative) const;

        virtual Ref<Texture> OnGetIcon(bool directory, const std::filesystem::path &relative);
        virtual bool GetDragDropData(AssetHandle &data, const std::filesystem::path &relative);
        virtual void OnWindowContextMenu();

    private:
        virtual void OnCreateAsset(const std::filesystem::path &directory, const Ref<Factory>& factory);

    private:
        ThumbnailCache mThumbnailCache;
    };

} // namespace BHive
