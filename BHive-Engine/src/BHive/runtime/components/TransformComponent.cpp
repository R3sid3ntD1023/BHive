#include "TransformComponent.h"
#include "runtime/GameObject.h"

namespace BHive
{
	void TransformComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(Transform);
	}

	void TransformComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ar(Transform);
	}

	RTTR_REGISTRATION
	{
		BEGIN_REFLECT(FTransform)
		REFLECT_PROPERTY("Translation", Translation)
		REFLECT_PROPERTY("Rotation", Rotation)
		REFLECT_PROPERTY("Scale", Scale)(META_DATA(EPropertyMetaData_Default, glm::vec3(1)));
	}

	REFLECT(TransformComponent)
	{
		BEGIN_REFLECT(TransformComponent) REFLECT_PROPERTY(Transform) COMPONENT_IMPL();
	}
} // namespace BHive