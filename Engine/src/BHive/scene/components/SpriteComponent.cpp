#include "scene/SceneRenderer.h"
#include "SpriteComponent.h"

namespace BHive
{
	void SpriteComponent::OnRender(SceneRenderer *renderer)
	{
		QuadRenderer::DrawSprite(mSize, mColor, GetWorldTransform(), mSprite, mFlags);
	}

	void SpriteComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ShapeComponent::Save(ar);
		ar(mColor, mSize, mFlags, mSprite);
	}

	void SpriteComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ShapeComponent::Load(ar);
		ar(mColor, mSize, mFlags, mSprite);
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