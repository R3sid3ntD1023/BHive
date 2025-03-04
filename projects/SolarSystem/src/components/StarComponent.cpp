#include "CelestrialBody.h"
#include "renderer/RenderPipeline.h"
#include "StarComponent.h"

BEGIN_NAMESPACE(BHive)

void StarComponent::Update(float dt)
{
	if (auto pipeline = BHive::GetRenderPipelineManager().GetCurrentPipeline())
	{
		pipeline->SubmitLight(mLight, GetOwner()->GetTransform());
	}
}

void StarComponent::Save(cereal::JSONOutputArchive &ar) const
{
	ar(MAKE_NVP(Brightness), MAKE_NVP(Radius), MAKE_NVP(Color));
}

void StarComponent::Load(cereal::JSONInputArchive &ar)
{
	ar(MAKE_NVP(Brightness), MAKE_NVP(Radius), MAKE_NVP(Color));

	mLight.mBrightness = Brightness;
	mLight.mRadius = Radius;
	mLight.mColor = Color;
}

REFLECT(StarComponent)
{
	BEGIN_REFLECT(StarComponent)
	REFLECT_CONSTRUCTOR();
}

END_NAMESPACE