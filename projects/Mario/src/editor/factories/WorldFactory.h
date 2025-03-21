#pragma once

#include "asset/Factory.h"

namespace BHive
{
	struct WorldFactory : public Factory
	{
		virtual Ref<Asset> CreateNew();

		virtual bool CanCreateNew() const { return true; }

		virtual std::string GetDefaultAssetName() const { return "NewWorld"; }

		virtual std::string GetDisplayName() const { return "World"; }

		REFLECTABLEV(Factory)
	};

} // namespace BHive
