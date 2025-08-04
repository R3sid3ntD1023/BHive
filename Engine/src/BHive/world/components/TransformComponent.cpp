#include "GameObject.h"
#include "TransformComponent.h"

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

	void TransformComponent::Save(cereal::JSONOutputArchive &ar) const
	{
		ar(MAKE_NVP(Transform));
	}

	void TransformComponent::Load(cereal::JSONInputArchive &ar)
	{
		ar(MAKE_NVP(Transform));
	}

	RTTR_REGISTRATION
	{
		BEGIN_REFLECT(FTransform)
			.property("Translation", &FTransform::GetTranslation, rttr::select_overload<void(const glm::vec3 &)>(&FTransform::SetTranslation)) REFLECT_PROPERTY("Rotation", GetRotation, SetRotation)
				REFLECT_PROPERTY("Scale", GetScale, SetScale);
	}

	REFLECT(TransformComponent)
	{
		BEGIN_REFLECT(TransformComponent) REFLECT_PROPERTY(Transform) COMPONENT_IMPL();
	}
} // namespace BHive