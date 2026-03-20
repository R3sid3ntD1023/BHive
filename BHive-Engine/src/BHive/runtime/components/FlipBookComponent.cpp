#include "FlipBookComponent.h"
#include "runtime/GameObject.h"

namespace BHive
{
	void FlipBookComponent::Begin()
	{
		mInstance = new FlipBook(*FlipBookAsset);

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
		}
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
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY(AutoPlay) REFLECT_PROPERTY(Color) REFLECT_PROPERTY(Size) REFLECT_PROPERTY(Tiling)
			REFLECT_PROPERTY(FlipBookAsset) COMPONENT_IMPL();
	}
} // namespace BHive