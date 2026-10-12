#include "CameraComponent.h"
#include "gfx/renderers/SceneRenderer.h"
#include "runtime/GameObject.h"

namespace BHive
{

	void CameraComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(Camera, IsPrimary);
	}

	void CameraComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ar(Camera, IsPrimary);
		MarkDirty();
	}

	void CameraComponent::SubmitCamera(SceneRenderer &sceneRenderer) const
	{
		const auto view = GetWorldTransform().Inverse().ToMat4();
		sceneRenderer.SetCamera(Camera.GetProjection(), view);
	}

	REFLECT(CameraComponent)
	{
		BEGIN_REFLECT(CameraComponent)(META_DATA(ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY(Camera) REFLECT_PROPERTY(IsPrimary) COMPONENT_IMPL();
	}
} // namespace BHive