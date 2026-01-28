#include "CameraComponent.h"
#include "World/GameObject.h"

namespace BHive
{

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