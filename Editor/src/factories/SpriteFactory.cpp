#include "sprite/Sprite.h"
#include "SpriteFactory.h"

namespace BHive
{
	Ref<Asset> SpriteFactory::CreateNew()
	{
		auto s = CreateRef<Sprite>();
		OnAssetCreated.invoke(s);
		return s;
	}

	REFLECT_FACTORY(SpriteFactory, Sprite, ".sprite")

} // namespace BHive
