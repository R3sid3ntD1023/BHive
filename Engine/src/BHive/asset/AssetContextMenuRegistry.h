#pragma once

#include "core/Core.h"

namespace BHive
{
	struct FAssetContextMenu;

	class BHIVE_API AssetContextMenuRegistry
	{
	public:
		AssetContextMenuRegistry();

		static AssetContextMenuRegistry &Get();

		template <typename T, typename M>
		void RegisterAssetContextMenu()
		{
			auto type = rttr::type::get<T>();
			auto menu = type.create().get_value<Ref<FAssetContextMenu>>();
			RegisterAssetContextMenu({type}, menu);
		}

		void RegisterAssetContextMenu(const std::unordered_set<rttr::type> &types, const Ref<FAssetContextMenu> &menu);

		FAssetContextMenu *GetAssetMenu(const rttr::type &type);

	private:
		std::unordered_map<rttr::type, Ref<FAssetContextMenu>> mRegisteredMenus;
	};

} // namespace BHive
