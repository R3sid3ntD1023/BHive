#pragma once

#include "asset/Factory.h"

namespace BHive
{
	struct PrefabFactory : public Factory
	{
		virtual Ref<Asset> CreateNew() override;

		virtual bool CanCreateNew() const { return true; }

		virtual std::string GetDefaultAssetName() const { return "NewPrefab"; }

		virtual std::string GetDisplayName() const { return "Prefab"; }

		REFLECTABLEV(Factory)
	};

} // namespace BHive