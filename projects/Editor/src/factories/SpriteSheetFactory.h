#pragma once

#include "asset/Factory.h"

namespace BHive
{
	class SpriteSheetFactory : public Factory
	{
	private:
		/* data */
	public:
		virtual Ref<Asset> CreateNew() override;

		virtual bool CanCreateNew() const { return true; }

		virtual std::string GetDefaultAssetName() const { return "NewSpriteSheet"; }

		virtual std::string GetDisplayName() const { return "SpriteSheet"; }

		REFLECTABLEV(Factory)
	};

} // namespace BHive
