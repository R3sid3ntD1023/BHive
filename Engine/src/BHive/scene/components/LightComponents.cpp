#include "LightComponents.h"
#include "scene/SceneRenderer.h"
#include "importers/TextureImporter.h"

namespace BHive
{

	void SpotLightComponent::OnRender(SceneRenderer *renderer)
	{
		auto transform = GetWorldTransform();
		renderer->SubmitLight(mLight, transform);

		LineRenderer::DrawSphere(mLight.mRadius, 16, {}, mLight.mColor, transform);
		LineRenderer::DrawCone(glm::cos(glm::radians(mLight.mOuterCutOff)), mLight.mRadius, 16, 0xFFFF0000, transform);
		LineRenderer::DrawCone(glm::cos(glm::radians(mLight.mInnerCutOff)), mLight.mRadius, 16, 0xFF00FF00, transform);
	}

	void SpotLightComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		LightComponent::Save(ar);
		ar(mLight);
	}

	void SpotLightComponent::Load(cereal::BinaryInputArchive &ar)
	{
		LightComponent::Load(ar);
		ar(mLight);
	}

	void PointLightComponent::OnRender(SceneRenderer *renderer)
	{
		auto transform = GetWorldTransform();

		renderer->SubmitLight(mLight, transform);
		LineRenderer::DrawSphere(mLight.mRadius, 16, {}, mLight.mColor, transform);
		QuadRenderer::DrawBillboard({1.f, 1.f}, mLight.mColor, transform, GetIcon());
	}

	void PointLightComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		LightComponent::Save(ar);
		ar(mLight);
	}

	void PointLightComponent::Load(cereal::BinaryInputArchive &ar)
	{
		LightComponent::Load(ar);
		ar(mLight);
	}

	const Ref<Texture> &PointLightComponent::GetIcon()
	{
		static Ref<Texture> icon;
		if (!icon)
		{
			icon = TextureImporter::Import(ENGINE_PATH "/data/textures/ic_pointlight.png");
		}

		return icon;
	}

	void DirectionalLightComponent::OnRender(SceneRenderer *renderer)
	{
		const auto transform = GetWorldTransform();
		renderer->SubmitLight(mLight, transform);

		auto forward = transform.get_forward();
		LineRenderer::DrawLine({}, -forward, mLight.mColor, transform);
	}

	void DirectionalLightComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		LightComponent::Save(ar);
		ar(mLight);
	}

	void DirectionalLightComponent::Load(cereal::BinaryInputArchive &ar)
	{
		LightComponent::Load(ar);
		ar(mLight);
	}

	REFLECT(LightComponent)
	{
		BEGIN_REFLECT(LightComponent);
	}

	REFLECT(PointLightComponent)
	{
		BEGIN_REFLECT(PointLightComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REQUIRED_COMPONENT_FUNCS() REFLECT_PROPERTY("Light", mLight);
	}

	REFLECT(SpotLightComponent)
	{
		BEGIN_REFLECT(SpotLightComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REQUIRED_COMPONENT_FUNCS() REFLECT_PROPERTY("Light", mLight);
	}

	REFLECT(DirectionalLightComponent)
	{
		BEGIN_REFLECT(DirectionalLightComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REQUIRED_COMPONENT_FUNCS() REFLECT_PROPERTY("Light", mLight);
	}

} // namespace BHive
