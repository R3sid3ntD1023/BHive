#include "LightComponents.h"
#include "gfx/renderers/Lights.h"
#include "gfx/renderers/SceneRenderer.h"
#include "runtime/GameObject.h"

namespace BHive
{

	void DirectionalLightComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(MAKE_NVP(Color));
	}
	void DirectionalLightComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ar(MAKE_NVP(Color));
	}

	void DirectionalLightComponent::OnRender(const FComponentRenderContext &context)
	{
		DirectionalLight light{};
		light.SetColor(Color).SetDirection(GetWorldTransform().GetForward()).SetIntensity(Color.a);
		context.Scene.Submit(light);
	}

	void PointLightComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(MAKE_NVP(Color), MAKE_NVP(Radius));
	}

	void PointLightComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ar(MAKE_NVP(Color), MAKE_NVP(Radius));
	}

	void PointLightComponent::OnRender(const FComponentRenderContext &context)
	{
		const auto transform = GetWorldTransform();
		PointLight light{};
		light.SetColor(Color)
			.SetPosition(transform.Translation)
			.SetRadius(Radius * glm::compMax(transform.Scale))
			.SetIntensity(Color.a);
		context.Scene.Submit(light);
	}

	void SpotLightComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(MAKE_NVP(Color), MAKE_NVP(Radius), MAKE_NVP(InnerCutoff), MAKE_NVP(OuterCutoff));
	}

	void SpotLightComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ar(MAKE_NVP(Color), MAKE_NVP(Radius), MAKE_NVP(InnerCutoff), MAKE_NVP(OuterCutoff));
	}

	void SpotLightComponent::OnRender(const FComponentRenderContext &context)
	{
		const auto transform = GetWorldTransform();
		SpotLight light{};
		light.SetColor(Color)
			.SetDirection(transform.GetForward())
			.SetPosition(transform.Translation)
			.SetRadius(Radius * glm::compMax(transform.Scale))
			.SetIntensity(Color.a)
			.SetInnerAngleDegrees(InnerCutoff)
			.SetOuterAngleDegrees(OuterCutoff);
		context.Scene.Submit(light);
	}

	REFLECT(DirectionalLightComponent)
	{
		BEGIN_REFLECT(DirectionalLightComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY(Color)(META_DATA(EPropertyMetaData_HDR, true)) COMPONENT_IMPL();
	}

	REFLECT(PointLightComponent)
	{
		BEGIN_REFLECT(PointLightComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY(Color)(META_DATA(EPropertyMetaData_HDR, true))
			REFLECT_PROPERTY(Radius)(META_DATA(EPropertyMetaData_Min, 0.0f)) COMPONENT_IMPL();
	}

	REFLECT(SpotLightComponent)
	{
		BEGIN_REFLECT(SpotLightComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY(Color)(META_DATA(EPropertyMetaData_HDR, true))
			REFLECT_PROPERTY(Radius)(META_DATA(EPropertyMetaData_Min, 0.0f)) REFLECT_PROPERTY(InnerCutoff)(META_DATA(EPropertyMetaData_Min, 0.0f))
				REFLECT_PROPERTY(OuterCutoff)(META_DATA(EPropertyMetaData_Min, 0.0f)) COMPONENT_IMPL();
	}

} // namespace BHive