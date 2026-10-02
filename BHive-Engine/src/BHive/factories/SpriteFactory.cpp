#include "SpriteFactory.h"
#include "gfx/sprite/Sprite.h"

namespace BHive
{
	Ref<Asset> SpriteFactory::CreateNew()
	{
		return CreateRef<Sprite>();
	}

	rttr::type SpriteFactory::GetAssetType() const
	{
		return rttr::type::get<Sprite>();
	}
} // namespace BHive