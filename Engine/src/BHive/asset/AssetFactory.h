#pragma once

#include "asset/Factory.h"

namespace BHive
{
	class AssetFactory
	{
	public:
		static bool Import(Ref<Asset> &asset, const std::filesystem::path &path);

		static bool Export(const Ref<Asset> &asset, const std::filesystem::path &path);

		static bool Export(const Asset *asset, const std::filesystem::path &path);

		static const char *GetFileFilters() { return "Asset (*.asset)\0*.asset\0"; }
	};
} // namespace BHive