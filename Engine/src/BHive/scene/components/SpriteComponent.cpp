#include "SpriteComponent.h"
#include "scene/SceneRenderer.h"
#include "scene/Entity.h"

namespace BHive
{
	void SpriteComponent::OnRender(SceneRenderer *renderer)
	{
		QuadRenderer::DrawSprite(mSize, mColor, GetWorldTransform(), mSprite, mFlags);
	}

	void SpriteComponent::Save(cereal::JSONOutputArchive &ar) const
	{
		ShapeComponent::Save(ar);
		ar(MAKE_NVP("Color", mColor), MAKE_NVP("Size", mSize), MAKE_NVP("Flags", mFlags),
		   MAKE_NVP("Sprite", mSprite));
	}

	void SpriteComponent::Load(cereal::JSONInputArchive &ar)
	{
		ShapeComponent::Load(ar);
		ar(MAKE_NVP("Color", mColor), MAKE_NVP("Size", mSize), MAKE_NVP("Flags", mFlags),
		   MAKE_NVP("Sprite", mSprite));
	}

	REFLECT(QuadRendererFlags_)
	{
		BEGIN_REFLECT_ENUM(QuadRendererFlags_)
		(ENUM_VALUE(QuadRendererFlags_Lit), ENUM_VALUE(QuadRendererFlags_None));
	}

	REFLECT(SpriteComponent)
	{
		BEGIN_REFLECT(SpriteComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REQUIRED_COMPONENT_FUNCS()
			REFLECT_PROPERTY("Color", mColor) REFLECT_PROPERTY("Sprite", mSprite)
				REFLECT_PROPERTY("Size", mSize) REFLECT_PROPERTY("Flags", mFlags)(
					META_DATA(EPropertyMetaData_Flags, EPropertyFlags_BitFlags));
	}
} // namespace BHive