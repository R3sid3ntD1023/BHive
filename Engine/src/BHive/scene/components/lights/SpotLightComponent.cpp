#include "SpotLightComponent.h"
#include "scene/SceneRenderer.h"
#include "importers/TextureImporter.h"

namespace BHive
{

	void SpotLightComponent::OnRender(SceneRenderer *renderer)
	{
		auto transform = GetWorldTransform();
		renderer->SubmitLight(mLight, transform);

		if ((renderer->GetFlags() & ESceneRendererFlags_VisualizeColliders) != 0)
		{
			auto forward = transform.get_forward();

			LineRenderer::DrawSphere(mLight.mRadius, 16, {}, 0xFFFFFFFF, transform);
			LineRenderer::DrawCone(glm::cos(glm::radians(mLight.mOuterCutOff)), mLight.mRadius, 16, 0xFFFF0000, transform);
			LineRenderer::DrawCone(glm::cos(glm::radians(mLight.mInnerCutOff)), mLight.mRadius, 16, 0xFF00FF00, transform);
			// QuadRenderer::DrawBillboard({1.f, 1.f}, 0xFFFFFFFF, transform, GetIcon());
		}
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

	REFLECT(SpotLightComponent)
	{
		BEGIN_REFLECT(SpotLightComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REQUIRED_COMPONENT_FUNCS() REFLECT_PROPERTY("Light", mLight);
	}

} // namespace BHive
