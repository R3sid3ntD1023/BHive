#pragma once

#include "core/Core.h"

namespace BHive
{
    struct BHIVE FAssetExtensions
	{
		FAssetExtensions() = default;
		FAssetExtensions(std::initializer_list<std::string> extensions);

		bool Contains(const std::string& ext) const;

		bool operator==(const FAssetExtensions& rhs) const;

	private:
		std::vector<std::string> supported_extensions;
	};
} // namespace BHive
