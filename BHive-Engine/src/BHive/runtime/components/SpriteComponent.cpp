#include "gfx/renderers/Renderer.h"
#include "gfx/sprite/Sprite.h"
#include "runtime/GameObject.h"
#include "SpriteComponent.h"

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

	void SpriteComponent::OnRender(const FComponentRenderContext &context)
	{
		if (!Sprite)
			return;

		FQuadParams params;
		params.Color = Color;
		params.Size = Size;
		params.Tiling = Tiling;
		context.EngineRenderer.Quad.DrawSprite(params, *Sprite.Get(), GetWorldTransform(), context.EntityID);
	}

	REFLECT(SpriteComponent)
	{
		BEGIN_REFLECT(SpriteComponent)(META_DATA(ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY(Color) REFLECT_PROPERTY(Size)
			REFLECT_PROPERTY(Tiling) REFLECT_PROPERTY(Sprite) COMPONENT_IMPL();
	}
} // namespace BHive