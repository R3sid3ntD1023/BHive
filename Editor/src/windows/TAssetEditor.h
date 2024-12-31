#pragma once

#include "AssetEditor.h"
#include "inspector/Inspectors.h"
#include "asset/EditorAssetManager.h"
#include "asset/AssetManager.h"
#include "asset/AssetSerializer.h"

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
            return AssetSerializer::serialize(*mAsset, GetAssetPath(path));
        }

        bool OnLoad(const std::filesystem::path &path)
        {
            auto asset = GetNewAsset();
            if (AssetSerializer::deserialize(*asset, GetAssetPath(path)))
            {
                mAsset = asset;
                return true;
            }

            return false;
        }

    protected:
        virtual std::filesystem::path GetAssetPath(const std::filesystem::path &path) const { return path; }

        virtual Ref<T> GetNewAsset() const { return CreateRef<T>(); }

    protected:
        Ref<T> mAsset;
        std::string mLabel;
    };

} // namespace BHive
