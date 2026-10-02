#pragma once

#include "asset/Factory.h"

namespace BHive
{

	class TextureFactory : public Factory
	{
	public:
		Ref<Asset> Import(const std::filesystem::path &filename) override;

		rttr::type GetAssetType() const override;

		uint8_t GetCategory() const override { return 2; }

		std::string GetDisplayName() const override { return "Texture"; }
	};
} // namespace BHive