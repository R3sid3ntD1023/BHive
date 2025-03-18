#include "FlipBookComponent.h"
#include "renderers/Renderer.h"
#include "objects/GameObject.h"

namespace BHive
{
	void FlipBookComponent::Begin()
	{
		if (AutoPlay)
			FlipBook->Play();
	}

	void FlipBookComponent::Update(float dt)
	{
		if (FlipBook)
		{
			FlipBook->Update(dt);
		}
	}
	void FlipBookComponent::Render()
	{
		if (!FlipBook)
			return;

		auto owner = GetOwner();
		auto t = owner->GetTransform();
		FQuadParams params{};
		params.Color = Color;
		params.Size = Size;
		params.Tiling = Tiling;
		QuadRenderer::DrawSprite(params, FlipBook->GetCurrentSprite(), t);
	}

	REFLECT(FlipBookComponent)
	{
		BEGIN_REFLECT(FlipBookComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY(AutoPlay)
			REFLECT_PROPERTY(Color) REFLECT_PROPERTY(Size) REFLECT_PROPERTY(Tiling) REFLECT_PROPERTY(FlipBook)
				REFLECT_METHOD(ADD_COMPONENT_FUNCTION_NAME, &GameObject::AddComponent<FlipBookComponent>);
	}
} // namespace BHive