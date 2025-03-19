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

	void CameraComponent::Render()
	{
		auto owner = GetOwner();
		auto transform = owner->GetTransform();
		FrustumViewer viewer(Camera.GetProjection(), transform);
	}

	REFLECT(CameraComponent)
	{
		{
			BEGIN_REFLECT(CameraComponent)
			(META_DATA(ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY(Primary)
				REFLECT_PROPERTY(Camera)
					REFLECT_METHOD(ADD_COMPONENT_FUNCTION_NAME, &GameObject::AddComponent<CameraComponent>)
						REFLECT_METHOD(REMOVE_COMPONENT_FUNCTION_NAME, &GameObject::RemoveComponent<CameraComponent>);
		}

		{
			BEGIN_REFLECT_ENUM(EProjectionType)
			(ENUM_VALUE(Perspective), ENUM_VALUE(Orthographic));
		}

		{
			BEGIN_REFLECT(SceneCamera::FPerspectiveSettings)
			REFLECT_PROPERTY(Fov)
			REFLECT_PROPERTY(Far)
			REFLECT_PROPERTY(Near);
		}

		{
			BEGIN_REFLECT(SceneCamera::FOrthographicSettings)
			REFLECT_PROPERTY(Left)
			REFLECT_PROPERTY(Right)
			REFLECT_PROPERTY(Bottom)
			REFLECT_PROPERTY(Top)
			REFLECT_PROPERTY(Far)
			REFLECT_PROPERTY(Near);
		}

		{
			BEGIN_REFLECT(SceneCamera)
			REFLECT_PROPERTY("ProjectionType", GetProjectionType, SetProjectionType)
			REFLECT_PROPERTY("Perspective", GetPerspectiveSettings, SetPerspectiveSettings)
			REFLECT_PROPERTY("Orthographic", GetOrthographicSettings, SetOrthographicSettings);
		}
	}
} // namespace BHive