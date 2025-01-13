#include "SpriteSheetFactory.h"
#include "sprite/SpriteSheet.h"

namespace BHive
{
	Ref<Asset> SpriteSheetFactory::CreateNew()
	{
		auto ss = CreateRef<SpriteSheet>();
		OnImportCompleted.invoke(ss);
		return ss;
	}

    REFLECT_Factory(SpriteSheetFactory, SpriteSheet, ".spritesheet") 
} // namespace BHive
