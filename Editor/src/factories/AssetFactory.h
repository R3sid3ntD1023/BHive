#pragma once

#include "asset/Factory.h"

namespace BHive
{
	class AssetFactory : public Factory
	{
	public:

		bool Import(Ref<Asset>& asset, const std::filesystem::path& path) override;

		bool Export(const Ref<Asset>& asset, const std::filesystem::path& path);

		virtual const char* GetFileFilters() const { return "Asset (*.asset)\0*.asset\0"; }
	};
}