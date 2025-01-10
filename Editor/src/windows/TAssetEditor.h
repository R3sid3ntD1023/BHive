#pragma once

#include "AssetEditor.h"
#include "inspector/Inspectors.h"
#include "asset/EditorAssetManager.h"
#include "asset/AssetManager.h"
#include "asset/AssetFactory.h"
#include "asset/Asset.h"

namespace BHive
{
    template <typename T>
    class TAssetEditor : public AssetEditor
    {
    public:
        void SetContext(const UUID &handle)
        {
            auto manager = AssetManager::GetAssetManager<EditorAssetManager>();
            auto metadata = manager->GetMetaData(handle);
            auto asset = AssetManager::GetAsset<T>(handle);

            mCurrentSavePath = metadata.Path;
            mAsset = asset;
            mLabel = metadata.Name;
        }

        virtual void OnWindowRender()
        {
            rttr::variant var = mAsset.get();
            if (inspect(var))
            {
                auto asset_ptr = mAsset.get();
                asset_ptr = var.get_value<T *>();
            }
        }

        virtual const char *GetName() const
        {
            return mLabel.c_str();
        };

        bool OnSave(const std::filesystem::path &path) const
        {
			AssetFactory factory;
			return factory.Export(mAsset, path);
        }

    protected:
        Ref<T> mAsset;
        std::string mLabel;
    };

} // namespace BHive
