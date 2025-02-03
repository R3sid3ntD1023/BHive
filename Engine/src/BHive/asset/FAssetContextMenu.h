#pragma once

#include "core/Core.h"
#include "asset/AssetHandle.h"
#include "core/reflection/Reflection.h"

namespace BHive
{
	class Asset;

	struct FAssetContextMenu
	{
		virtual ~FAssetContextMenu() = default;

		virtual void OnAssetContext(const AssetHandle &handle) = 0;
		virtual void OnAssetOpen(const AssetHandle &handle) = 0;

		REFLECTABLEV()
	};

	REFLECT(FAssetContextMenu)
	{
		BEGIN_REFLECT(FAssetContextMenu);
	}

} // namespace BHive

#define REFLECT_ASSET_MENU(cls, type)                                                                \
	REFLECT(cls)                                                                                     \
	{                                                                                                \
		BEGIN_REFLECT(cls)                                                                           \
		(META_DATA("Type", AssetType::get<type>())) REFLECT_CONSTRUCTOR() CONSTRUCTOR_POLICY_SHARED; \
	}