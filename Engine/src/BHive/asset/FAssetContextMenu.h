#pragma once

#include "core/Core.h"
#include "core/UUID.h"
#include "core/reflection/Reflection.h"

namespace BHive
{
	class Asset;

	struct FAssetContextMenu
	{
		virtual ~FAssetContextMenu() = default;

		virtual void OnAssetContext(const UUID &handle) = 0;
		virtual void OnAssetOpen(const UUID &handle) = 0;

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