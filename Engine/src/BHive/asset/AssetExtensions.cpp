#include "AssetExtensions.h"

namespace BHive
{
    FAssetExtensions::FAssetExtensions(std::initializer_list<std::string> extensions)
			:supported_extensions(extensions)
		{}

    bool FAssetExtensions::Contains(const std::string& ext) const
    {
        return std::find(supported_extensions.begin(),
            supported_extensions.end(), ext) != supported_extensions.end();
    }

    bool FAssetExtensions::operator==(const FAssetExtensions& rhs) const
    {
        return supported_extensions == rhs.supported_extensions;
    }

} // namespace BHive
