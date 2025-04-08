#pragma once

#include "asset/Factory.h"

namespace BHive
{
	class SpriteFactory : public Factory
	{
	public:
		virtual Ref<Asset> CreateNew();

		virtual bool CanCreateNew() const { return true; }

		virtual std::string GetDefaultAssetName() const { return "NewSprite"; }

		virtual std::string GetDisplayName() const { return "Sprite"; }

		REFLECTABLEV(Factory)
	};

} // namespace BHive
