#include "EditorSubSystem.h"

namespace BHive
{
	Ref<Texture2D> BHive::EditorSubSystem::GetIcon(const std::filesystem::path &path)
	{
		return mIconCache.Get(path);
	}
} // namespace BHive
