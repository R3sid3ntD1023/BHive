#pragma once

#include "Asset.h"
#include "core/Core.h"

namespace BHive
{
	class BHIVE_API AssetSerializer
	{
	public:
		static Ref<Asset> Import(const std::filesystem::path &path);

		static bool Export(const Ref<Asset> &asset, const std::filesystem::path &path);
	};
} // namespace BHive