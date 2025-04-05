#include "GameObject.h"
#include "Renderers/Renderer.h"
#include "SpriteComponent.h"

namespace BHive
{
	void SpriteComponent::Render()
	{
		auto owner = GetOwner();
		auto transform = owner->GetTransform();

		FQuadParams params{};
		params.Color = Color;
		params.Size = Size;
		params.Tiling = Tiling;
		QuadRenderer::DrawSprite(params, SpriteAsset, transform);
	}

	void SpriteComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(Color, Size, Tiling, TAssetHandle(SpriteAsset));
	}

	void SpriteComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ar(Color, Size, Tiling, TAssetHandle(SpriteAsset));
	}

	REFLECT(SpriteComponent)
	{
		BEGIN_REFLECT(SpriteComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY(Tiling)
			REFLECT_PROPERTY(Size) REFLECT_PROPERTY(Color) REFLECT_PROPERTY(SpriteAsset) COMPONENT_IMPL();
	}
} // namespace BHive