#include "AssetContextMenuRegistry.h"
#include "asset/FAssetContextMenu.h"

namespace BHive
{
    AssetContextMenuRegistry::AssetContextMenuRegistry()
    {
        auto derived_types = AssetType::get<FAssetContextMenu>().get_derived_classes();
        for (auto &derived : derived_types)
        {
            auto type_var = derived.get_metadata("Type").get_value<AssetType>();
            auto menu = derived.create().get_value<Ref<FAssetContextMenu>>();
            RegisterAssetContextMenu(type_var, menu);
        }
    }

    AssetContextMenuRegistry &AssetContextMenuRegistry::Get()
    {
        static AssetContextMenuRegistry registry;
        return registry;
    }

    void AssetContextMenuRegistry::RegisterAssetContextMenu(const AssetType &type, const Ref<FAssetContextMenu> &menu)
    {
        ASSERT(!mRegisteredMenus.contains(type));

        mRegisteredMenus.emplace(type, menu);
    }

    FAssetContextMenu *AssetContextMenuRegistry::GetAssetMenu(const AssetType &type)
    {
        if (mRegisteredMenus.contains(type))
            return mRegisteredMenus.at(type).get();

        return nullptr;
    }
}