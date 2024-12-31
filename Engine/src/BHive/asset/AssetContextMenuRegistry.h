#pragma once

#include "core/Core.h"
#include "asset/AssetType.h"

namespace BHive
{
    struct FAssetContextMenu;

    class AssetContextMenuRegistry
    {
    public:
        AssetContextMenuRegistry();

        static AssetContextMenuRegistry &Get();

        template <typename T, typename M>
        void RegisterAssetContextMenu()
        {
            auto type = AssetType::get<T>();
            auto menu = type.create().get_value<Ref<FAssetContextMenu>>();
            RegisterAssetContextMenu(type, menu);
        }

        void RegisterAssetContextMenu(const AssetType &type, const Ref<FAssetContextMenu> &menu);

        FAssetContextMenu *GetAssetMenu(const AssetType &type);

    private:
        std::unordered_map<AssetType, Ref<FAssetContextMenu>> mRegisteredMenus;
    };

} // namespace BHive
