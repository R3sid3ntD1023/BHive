#include "CameraComponent.h"
#include "GameObject.h"
#include "renderers/Renderer.h"

namespace BHive
{
	void CameraComponent::Update(float)
	{
		if (IsPrimary)
		{
			auto owner = GetOwner();
			auto transform = owner->GetWorldTransform();
			Renderer::SubmitCamera(Camera.GetProjection(), transform.Inverse());
		}
	}

	void CameraComponent::Render()
	{

#ifdef WITH_BHIVE_EDITOR
		auto owner = GetOwner();
		auto transform = owner->GetWorldTransform();
		FrustumViewer viewer(Camera.GetProjection(), transform);
		LineRenderer::DrawFrustum(viewer, Colors::Yellow);
#endif
	}

	void CameraComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(Camera, IsPrimary);
	}

	void CameraComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ar(Camera, IsPrimary);
	}

	REFLECT(CameraComponent)
	{
		BEGIN_REFLECT(CameraComponent)(META_DATA(ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY(Camera) REFLECT_PROPERTY(IsPrimary) COMPONENT_IMPL();
	}
} // namespace BHive