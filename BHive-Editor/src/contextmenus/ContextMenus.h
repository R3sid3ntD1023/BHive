#pragma once

#include "asset/FAssetContextMenu.h"
#include "core/EventDelegate.h"

namespace BHive
{
	class AssetEditor;

	template <typename TEditor>
	struct FAssetContextMenuBase : public FAssetContextMenu
	{
		virtual void OnAssetContext(const UUID &handle) override;
		virtual void OnAssetOpen(const UUID &handle) override;

		REFLECTABLEV(FAssetContextMenu)
	};

	DECLARE_EVENT(FOnAssetOpened, const UUID &)

	struct FWorldContentMenu : public FAssetContextMenu
	{
		virtual void OnAssetContext(const UUID &handle) override;
		virtual void OnAssetOpen(const UUID &handle) override;

		static inline FOnAssetOpenedEvent OnAssetOpenedEvent;

		REFLECTABLEV(FAssetContextMenu);
	};

} // namespace BHive
