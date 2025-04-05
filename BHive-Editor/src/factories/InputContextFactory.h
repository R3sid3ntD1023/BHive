#pragma once

#include "asset/Factory.h"

namespace BHive
{
	class InputContextFactory : public Factory
	{
	public:
		virtual Ref<Asset> CreateNew() override;

		virtual bool CanCreateNew() const override { return true; }

		virtual std::string GetDefaultAssetName() const override { return "NewInputContext"; }

		virtual std::string GetDisplayName() const { return "InputContext"; }

		REFLECTABLEV(Factory)
	};

} // namespace BHive
