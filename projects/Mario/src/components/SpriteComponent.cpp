#include "objects/GameObject.h"
#include "Renderers/Renderer.h"
#include "SpriteComponent.h"

namespace BHive
{
	void SpriteComponent::Update(float)
	{
	}

	void SpriteComponent::Render()
	{
		if (!Sprite)
			return;

		auto owner = GetOwner();
		auto transform = owner->GetTransform();

		FQuadParams params{};
		params.Color = SpriteColor;
		params.Size = SpriteSize;
		params.Tiling = Tiling;
		QuadRenderer::DrawSprite(params, Sprite->Get(), transform);
	}
} // namespace BHive