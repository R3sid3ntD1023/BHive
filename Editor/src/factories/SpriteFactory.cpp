#include "SpriteFactory.h"
#include "sprite/Sprite.h"

namespace BHive
{
    Ref<Asset> SpriteFactory::CreateNew()
	{
		auto s = CreateRef<Sprite>();
		OnImportCompleted.invoke(s);
		return s;
	}

    REFLECT_Factory(SpriteFactory, Sprite, ".sprite")

} // namespace BHive
