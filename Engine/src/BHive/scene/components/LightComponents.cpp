#include "LightComponents.h"
#include "scene/SceneRenderer.h"
#include "scene/Entity.h"

namespace BHive
{

	void SpotLightComponent::OnRender(SceneRenderer *renderer)
	{
		auto transform = GetWorldTransform();
		renderer->SubmitLight(mLight, transform);

		LineRenderer::DrawSphere(mLight.mRadius, 16, {}, mLight.mColor, transform);
		LineRenderer::DrawCone(glm::cos(glm::radians(mLight.mOuterCutOff)), mLight.mRadius, 16,
							   0xFFFF0000, transform);
		LineRenderer::DrawCone(glm::cos(glm::radians(mLight.mInnerCutOff)), mLight.mRadius, 16,
							   0xFF00FF00, transform);
	}

	void SpotLightComponent::Save(cereal::JSONOutputArchive &ar) const
	{
		LightComponent::Save(ar);
		ar(MAKE_NVP("Light", mLight));
	}

	void SpotLightComponent::Load(cereal::JSONInputArchive &ar)
	{
		LightComponent::Load(ar);
		ar(MAKE_NVP("Light", mLight));
	}

	void PointLightComponent::OnRender(SceneRenderer *renderer)
	{
		auto transform = GetWorldTransform();

		renderer->SubmitLight(mLight, transform);
		LineRenderer::DrawSphere(mLight.mRadius, 16, {}, mLight.mColor, transform);
	}

	void PointLightComponent::Save(cereal::JSONOutputArchive &ar) const
	{
		LightComponent::Save(ar);
		ar(MAKE_NVP("Light", mLight));
	}

	void PointLightComponent::Load(cereal::JSONInputArchive &ar)
	{
		LightComponent::Load(ar);
		ar(MAKE_NVP("Light", mLight));
	}

	void DirectionalLightComponent::OnRender(SceneRenderer *renderer)
	{
		renderer->SubmitLight(mLight, GetWorldTransform());

		auto forward = GetWorldTransform().get_forward();
		LineRenderer::DrawLine({}, -forward, mLight.mColor, GetWorldTransform());
	}

	void DirectionalLightComponent::Save(cereal::JSONOutputArchive &ar) const
	{
		LightComponent::Save(ar);
		ar(MAKE_NVP("Light", mLight));
	}

	void DirectionalLightComponent::Load(cereal::JSONInputArchive &ar)
	{
		LightComponent::Load(ar);
		ar(MAKE_NVP("Light", mLight));
	}

	REFLECT(LightComponent)
	{
		BEGIN_REFLECT(LightComponent);
	}

	REFLECT(PointLightComponent)
	{
		BEGIN_REFLECT(PointLightComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REQUIRED_COMPONENT_FUNCS()
			REFLECT_PROPERTY("Light", mLight);
	}

	REFLECT(SpotLightComponent)
	{
		BEGIN_REFLECT(SpotLightComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REQUIRED_COMPONENT_FUNCS()
			REFLECT_PROPERTY("Light", mLight);
	}

	REFLECT(DirectionalLightComponent)
	{
		BEGIN_REFLECT(DirectionalLightComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REQUIRED_COMPONENT_FUNCS()
			REFLECT_PROPERTY("Light", mLight);
	}

} // namespace BHive
