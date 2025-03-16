#include "CameraComponent.h"
#include "objects/GameObject.h"
#include "renderers/Renderer.h"

namespace BHive
{
	void CameraComponent::Update(float)
	{
		if (Primary)
		{
			auto owner = GetOwner();
			auto transform = owner->GetTransform();
			Renderer::SubmitCamera(Camera.GetProjection(), transform.inverse());
		}
	}
} // namespace BHive