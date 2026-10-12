#include "FlipBookComponent.h"
#include "gfx/renderers/Renderer.h"
#include "gfx/sprite/FlipBook.h"
#include "runtime/GameObject.h"

namespace BHive
{
	void FlipBookComponent::Begin()
	{
		if (FlipBookHandle)
			mInstance = new FlipBook(*FlipBookHandle.Get());

		if (AutoPlay)
			mInstance->Play();
	}

	void FlipBookComponent::End()
	{
		delete mInstance;
	}

	void FlipBookComponent::Update(float dt)
	{
		if (mInstance)
		{
			mInstance->Update(dt);
			MarkDirty();
		}
	}

	void FlipBookComponent::OnRender(const FComponentRenderContext &context)
	{
		if (!FlipBookHandle)
			return;

		FQuadParams params{};
		params.Color = Color;
		params.Size = Size;
		params.Tiling = Tiling;

		const auto sprite = mInstance ? mInstance->GetCurrentSprite() : FlipBookHandle->GetCurrentSprite();
		context.EngineRenderer.Quad.DrawSprite(params, *sprite.Get(), GetWorldTransform(), context.EntityID);
	}

	void FlipBookComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(AutoPlay, Color, Size, Tiling, FlipBookHandle);
	}

	void FlipBookComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ar(AutoPlay, Color, Size, Tiling, FlipBookHandle);
	}

	REFLECT(FlipBookComponent)
	{
		BEGIN_REFLECT(FlipBookComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY(AutoPlay) REFLECT_PROPERTY(Color) REFLECT_PROPERTY(Size) REFLECT_PROPERTY(Tiling)
			REFLECT_PROPERTY(FlipBookHandle) COMPONENT_IMPL();
	}
} // namespace BHive