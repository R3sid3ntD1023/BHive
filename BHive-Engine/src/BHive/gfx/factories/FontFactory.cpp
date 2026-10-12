#include "FontFactory.h"

namespace BHive
{
	Ref<class Font> FontFactory::Create(const std::filesystem::path &path, uint32_t fontSize)
	{
		return CreateRef<Font>(path, fontSize);
	}
} // namespace BHive
