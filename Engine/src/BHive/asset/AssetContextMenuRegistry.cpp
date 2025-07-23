#include "AssetContextMenuRegistry.h"
#include "asset/FAssetContextMenu.h"

namespace BHive
{
	AssetContextMenuRegistry::AssetContextMenuRegistry()
	{
		auto derived_types = rttr::type::get<FAssetContextMenu>().get_derived_classes();
		for (auto &derived : derived_types)
		{
			auto type_var = derived.get_metadata("SupportedTypes").get_value<std::unordered_set<rttr::type>>();
			auto menu = derived.create().get_value<Ref<FAssetContextMenu>>();
			RegisterAssetContextMenu(type_var, menu);
		}
	}

	AssetContextMenuRegistry &AssetContextMenuRegistry::Get()
	{
		static AssetContextMenuRegistry registry;
		return registry;
	}

	void AssetContextMenuRegistry::RegisterAssetContextMenu(
		const std::unordered_set<rttr::type> &types, const Ref<FAssetContextMenu> &menu)
	{
		for (const auto &type : types)
		{
			if (mRegisteredMenus.contains(type))
			{
				LOG_ERROR("Asset context menu for type {} is already registered!", type);
				continue;
			}

			mRegisteredMenus.emplace(type, menu);
		}
	}

	FAssetContextMenu *AssetContextMenuRegistry::GetAssetMenu(const rttr::type &type)
	{
		for (auto &[supported_type, menu] : mRegisteredMenus)
		{
			// Check if the type matches or is derived from the supported type
			if (type == supported_type || type.is_derived_from(supported_type))
				return mRegisteredMenus.at(supported_type).get();
		}

		return nullptr;
	}
} // namespace BHive