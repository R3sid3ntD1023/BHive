#include "sprite/SpriteSheet.h"
#include "SpriteSheetFactory.h"

namespace BHive
{
	Ref<Asset> SpriteSheetFactory::CreateNew()
	{
		auto ss = CreateRef<SpriteSheet>();
		OnAssetCreated.invoke(ss);
		return ss;
	}

	REFLECT_FACTORY(SpriteSheetFactory)
} // namespace BHive
