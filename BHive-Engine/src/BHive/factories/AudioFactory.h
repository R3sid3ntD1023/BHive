#pragma once

#include "asset/Factory.h"

namespace BHive
{
	class BHIVE_API AudioFactory : public Factory
	{
	public:
		Ref<Asset> Import(const std::filesystem::path &path) override;

		virtual rttr::type GetAssetType() const;

		virtual uint8_t GetCategory() const { return 1; }

		std::string GetDisplayName() const override { return "Audio"; }
	};
} // namespace BHive