#include "PointLightComponent.h"
#include "scene/SceneRenderer.h"
#include "importers/TextureImporter.h"

namespace BHive
{

	void PointLightComponent::OnRender(SceneRenderer *renderer)
	{
		auto transform = GetWorldTransform();

		renderer->SubmitLight(mLight, transform);

		if ((renderer->GetFlags() & ESceneRendererFlags_VisualizeColliders) != 0 && mLight.mRadius > 0)
		{
			LineRenderer::DrawSphere(mLight.mRadius, 16, {}, 0xFFFFFFFF, transform);
			QuadRenderer::DrawBillboard({1.f, 1.f}, 0xFFFFFFFF, transform, GetPointLightIcon());
		}
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

	Ref<class Texture> PointLightComponent::GetPointLightIcon()
	{
		static Ref<Texture> icon;
		if (!icon)
		{
			icon = TextureImporter::Import(ENGINE_PATH "/data/textures/ic_pointlight.png");
		}

		return icon;
	}

	REFLECT(PointLightComponent)
	{
		BEGIN_REFLECT(PointLightComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REQUIRED_COMPONENT_FUNCS() REFLECT_PROPERTY("Light", mLight);
	}

} // namespace BHive
