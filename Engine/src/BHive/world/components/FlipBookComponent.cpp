#include "FlipBookComponent.h"
#include "renderers/Renderer.h"
#include "GameObject.h"

namespace BHive
{
	void FlipBookComponent::Begin()
	{
		Instance = new FlipBook(*FlipBookAsset);

		if (AutoPlay)
			Instance->Play();
	}

	void FlipBookComponent::End()
	{
		delete Instance;
	}

	void FlipBookComponent::Update(float dt)
	{
		if (Instance)
		{
			Instance->Update(dt);
		}
	}
	void FlipBookComponent::Render()
	{
		if (!FlipBookAsset)
			return;

		auto owner = GetOwner();
		auto t = owner->GetTransform();
		FQuadParams params{};
		params.Color = Color;
		params.Size = Size;
		params.Tiling = Tiling;

		auto sprite = Instance ? Instance->GetCurrentSprite() : FlipBookAsset->GetCurrentSprite();
		QuadRenderer::DrawSprite(params, sprite, t);
	}

	void FlipBookComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(AutoPlay, Color, Size, Tiling, TAssetHandle(FlipBookAsset));
	}

	void FlipBookComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ar(AutoPlay, Color, Size, Tiling, TAssetHandle(FlipBookAsset));
	}

	REFLECT(FlipBookComponent)
	{
		BEGIN_REFLECT(FlipBookComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY(AutoPlay)
			REFLECT_PROPERTY(Color) REFLECT_PROPERTY(Size) REFLECT_PROPERTY(Tiling) REFLECT_PROPERTY(FlipBookAsset)
				COMPONENT_IMPL();
	}
} // namespace BHive