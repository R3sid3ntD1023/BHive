#pragma once

#include "asset/Factory.h"

namespace BHive
{
	class PhysicsMaterialFactory : public Factory
	{
	public:
		virtual Ref<Asset> CreateNew();

		virtual bool CanCreateNew() const { return true; }

		virtual std::string GetDefaultAssetName() const { return "NewPhysicsMaterial"; }

		virtual std::string GetDisplayName() const { return "PhysicsMaterial"; }

		REFLECTABLEV(Factory)
	};
} // namespace BHive
