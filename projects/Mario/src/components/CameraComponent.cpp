#include "CameraComponent.h"
#include "objects/GameObject.h"
#include "renderers/Renderer.h"

namespace BHive
{
	void CameraComponent::Update(float)
	{
		if (IsPrimary)
		{
			auto owner = GetOwner();
			auto transform = owner->GetTransform();
			Renderer::SubmitCamera(Camera.GetProjection(), transform.inverse());
		}
	}

	void CameraComponent::Render()
	{
		auto owner = GetOwner();
		auto transform = owner->GetTransform();
		FrustumViewer viewer(Camera.GetProjection(), transform);
	}

	void CameraComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(Camera, IsPrimary);
	}

	void CameraComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ar(Camera, IsPrimary);
	}
} // namespace BHive