#include "SpriteComponent.h"
#include "runtime/GameObject.h"

namespace BHive
{

	void SpriteComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(Color, Size, Tiling, Sprite);
	}

	void SpriteComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ar(Color, Size, Tiling, Sprite);
	}

	REFLECT(SpriteComponent)
	{
		BEGIN_REFLECT(SpriteComponent)(META_DATA(ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY(Color) REFLECT_PROPERTY(Size) REFLECT_PROPERTY(Tiling)
			REFLECT_PROPERTY(Sprite) COMPONENT_IMPL();
	}
} // namespace BHive