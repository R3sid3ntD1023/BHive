#pragma once

#include "asset/Factory.h"

namespace BHive
{
	class FlipBookFactory : public Factory
	{
	public:
		virtual Ref<Asset> CreateNew() override;

		virtual bool CanCreateNew() const override { return true; }

		virtual std::string GetDefaultAssetName() const { return "NewFlipbook"; }

		virtual std::string GetDisplayName() const { return "Flipbook"; }

		REFLECTABLEV(Factory)
	};

} // namespace BHive
