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
		QuadRenderer::DrawSprite(params, Sprite, transform);
	}

	REFLECT(SpriteComponent)
	{
		BEGIN_REFLECT(SpriteComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY(Tiling)
			REFLECT_PROPERTY(SpriteSize) REFLECT_PROPERTY(SpriteColor) REFLECT_PROPERTY(Sprite)
				REFLECT_METHOD(ADD_COMPONENT_FUNCTION_NAME, &GameObject::AddComponent<SpriteComponent>)
					REFLECT_METHOD(REMOVE_COMPONENT_FUNCTION_NAME, &GameObject::RemoveComponent<SpriteComponent>);
	}
} // namespace BHive