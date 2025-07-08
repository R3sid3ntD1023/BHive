#include "LightComponents.h"
#include "renderers/Renderer.h"
#include "world/GameObject.h"

namespace BHive
{
	void DirectionalLightComponent::Render()
	{
		auto owner = GetOwner();
		auto transform = owner->GetWorldTransform();
		Renderer::SubmitDirectionalLight(transform.get_forward(), mLight);
	}

	void DirectionalLightComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(mLight);
	}
	void DirectionalLightComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ar(mLight);
	}

	void PointLightComponent::Render()
	{
		auto owner = GetOwner();
		auto transform = owner->GetWorldTransform();
		Renderer::SubmitPointLight(transform.get_translation(), mLight);
		LineRenderer::DrawSphere(mLight.mRadius, 16, {}, mLight.mColor, transform);
	}

	void PointLightComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(mLight);
	}

	void PointLightComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ar(mLight);
	}

	void SpotLightComponent::Render()
	{
		auto owner = GetOwner();
		auto transform = owner->GetWorldTransform();
		Renderer::SubmitSpotLight(transform.get_forward(), transform.get_translation(), mLight);
		LineRenderer::DrawCone(mLight.mRadius, mLight.mRadius, 16, mLight.mColor, transform);
	}

	void SpotLightComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(mLight);
	}

	void SpotLightComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ar(mLight);
	}

	REFLECT(Light)
	{
		BEGIN_REFLECT(Light)
		REFLECT_PROPERTY("Color", mColor)
		REFLECT_PROPERTY("Brightness", mBrightness);
	}

	REFLECT(PointLight)
	{
		BEGIN_REFLECT(PointLight)
		REFLECT_PROPERTY("Radius", mRadius);
	}

	REFLECT(SpotLight)
	{
		BEGIN_REFLECT(SpotLight)
		REFLECT_PROPERTY("InnerCutOff", mInnerCutOff) REFLECT_PROPERTY("OuterCutOff", mOuterCutOff);
	}

	REFLECT(DirectionalLight)
	{
		BEGIN_REFLECT(DirectionalLight);
	}

	REFLECT(DirectionalLightComponent)
	{
		BEGIN_REFLECT(DirectionalLightComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY("Light", mLight)
			COMPONENT_IMPL();
	}

	REFLECT(PointLightComponent)
	{
		BEGIN_REFLECT(PointLightComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY("Light", mLight)
			COMPONENT_IMPL();
	}

	REFLECT(SpotLightComponent)
	{
		BEGIN_REFLECT(SpotLightComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY("Light", mLight)
			COMPONENT_IMPL();
	}

} // namespace BHive