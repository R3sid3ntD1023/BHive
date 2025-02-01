#pragma once

#include "asset/FAssetContextMenu.h"

namespace BHive
{
	class AssetEditor;

	template <typename TEditor>
	struct FAssetContextMenuBase : public FAssetContextMenu
	{
		virtual void OnAssetContext(const AssetHandle &handle) override;
		virtual void OnAssetOpen(const AssetHandle &handle) override;

		REFLECTABLEV(FAssetContextMenu)
	};

} // namespace BHive
