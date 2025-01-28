#pragma once

#include "asset/Factory.h"

namespace BHive
{
	class MaterialFactory : public Factory
	{
	public:
		virtual Ref<Asset> CreateNew() override;

		virtual bool CanCreateNew() const { return true; }

		virtual std::string GetDefaultAssetName() const { return "NewMaterial"; }

		virtual std::string GetDisplayName() const { return "Material"; }

		REFLECTABLEV(Factory)
	};
} // namespace BHive
