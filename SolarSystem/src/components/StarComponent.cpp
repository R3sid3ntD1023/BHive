#include "StarComponent.h"
#include "CelestrialBody.h"
#include "renderer/RenderPipeline.h"

void StarComponent::Update(float dt)
{
	BHive::PointLight light;
	light.mBrightness = Brightness;
	light.mRadius = Radius;
	light.mColor = Color;

	BHive::UniverseRenderPipeline::GetPipeline().SubmitLight({.Transform = GetOwner()->GetTransform(), .PointLight = light});
}

void StarComponent::Save(cereal::JSONOutputArchive &ar) const
{
	ar(MAKE_NVP(Brightness), MAKE_NVP(Radius), MAKE_NVP(Color));
}

void StarComponent::Load(cereal::JSONInputArchive &ar)
{
	ar(MAKE_NVP(Brightness), MAKE_NVP(Radius), MAKE_NVP(Color));
}

REFLECT(StarComponent)
{
	BEGIN_REFLECT(StarComponent)
	REFLECT_CONSTRUCTOR();
}