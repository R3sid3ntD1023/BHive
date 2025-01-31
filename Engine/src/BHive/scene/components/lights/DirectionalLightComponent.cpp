#include "DirectionalLightComponent.h"
#include "scene/SceneRenderer.h"
#include "importers/TextureImporter.h"

namespace BHive
{

	void DirectionalLightComponent::OnRender(SceneRenderer *renderer)
	{
		const auto transform = GetWorldTransform();
		renderer->SubmitLight(mLight, transform);

		if ((renderer->GetFlags() & ESceneRendererFlags_VisualizeColliders) != 0)
		{
			auto forward = transform.get_forward();
			LineRenderer::DrawLine({}, -forward, 0xFFFFFFFF, transform);
			// QuadRenderer::DrawBillboard({1.f, 1.f}, 0xFFFFFFFF, transform, GetIcon());
		}
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

	REFLECT(DirectionalLightComponent)
	{
		BEGIN_REFLECT(DirectionalLightComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REQUIRED_COMPONENT_FUNCS() REFLECT_PROPERTY("Light", mLight);
	}

} // namespace BHive
