#pragma once

#include "asset/Factory.h"

namespace BHive
{
	class SpriteFactory : public Factory
	{
	public:
		Ref<Asset> CreateNew() override;

		bool CanCreateNew() const override { return true; }

		rttr::type GetAssetType() const;

		uint8_t GetCategory() const { return 3; }

		std::string GetDisplayName() const override { return "Sprite"; }
	};
} // namespace BHive