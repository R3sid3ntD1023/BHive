#pragma once

#include "asset/Factory.h"

namespace BHive
{
	class AnimGraphFactory : public Factory
	{
	public:
		virtual Ref<Asset> CreateNew() override;

		bool CanCreateNew() const { return true; }

		virtual std::string GetDefaultAssetName() const { return "NewAnimGraph"; }

		virtual std::string GetDisplayName() const { return "AnimGraph"; }

		REFLECTABLEV(Factory)
	};
} // namespace BHive